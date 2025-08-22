// Header
#include "torque_thread.h"

// Includes
#include "can.h"
#include "controls/pid_controller.h"
#include "controls/torque_vectoring.h"
#include "controls/tv_const_bias.h"
#include "controls/tv_linear_bias.h"
#include "peripherals.h"
#include "state_thread.h"
#include "controls/lerp.h"

// C Standard Library
#include <float.h>

// Constants ------------------------------------------------------------------------------------------------------------------

#define TORQUE_THREAD_PERIOD TIME_MS2I (10)
#define TORQUE_THREAD_PERIOD_S (TIME_I2US (TORQUE_THREAD_PERIOD) / 1000000.0f)
#define TORQUE_THREAD_CAN_MESSAGE_TIMEOUT (TORQUE_THREAD_PERIOD / 4)

#define CUMULATIVE_TORQUE_TOLERANCE 0.05f

// Global Data ----------------------------------------------------------------------------------------------------------------

/// @brief The last calculated torque request.
tvOutput_t torqueRequest;

/// @brief The cumulative driving (positive) torque limit.
float drivingTorqueLimit = 0.0f;

/// @brief The cumulative regenerative (negative) torque limit.
static float regenTorqueLimit = 0.0f;

/// @brief The index of the selected torque-vectoring algorithm.
static uint8_t algoritmIndex = 0;

static const tvConstBiasConfig_t STF_L_CONFIG =
{
	.frontRearBias = 1,
	.leftRightBias = 1
};

static const tvConstBiasConfig_t STF_R_CONFIG =
{
	.frontRearBias = 1,
	.leftRightBias = 0
};

/// @brief The array of selectable torque-vectoring algorithms.
#define TV_ALGORITHM_COUNT (sizeof (tvAlgorithms) / sizeof (tvAlgorithm_t))
static tvAlgorithm_t tvAlgorithms [] =
{
	{
		// Straight-diff
		.entrypoint	= &tvConstBias,
		.config		= &eepromMap->sdConfig
	},
	{
		// Linear-steering
		.entrypoint	= &tvLinearBias,
		.config		= &eepromMap->lsConfig
	},
	{
		// Linear-steering (slalom)
		.entrypoint	= &tvLinearBias,
		.config		= &eepromMap->lssConfig
	},
	{
		// Single-tire-fire (left)
		.entrypoint	= &tvConstBias,
		.config		= &STF_L_CONFIG
	},
	{
		// Single-tire-fire (right)
		.entrypoint	= &tvConstBias,
		.config		= &STF_R_CONFIG
	}
};

/**
 * @brief PID controller responsible for enforcing the global power limit. The y variable represents the vehicle's cumulative
 * power consumption, while the x variable represents the ratio to scale the torque requests by. The set-point is fixed at the
 * power limit, while the output value is clamped from [-1, 0]. This means the controller only has the ability to reduce the
 * requested torque, and this reduction only occurs when the power consumption exceeds the set-point.
 */
static pidController_t powerLimitPid =
{
	.kp			= 0,
	.ki			= 0,
	.kd			= 0,
	.ySetPoint	= 0,
	.ypPrime	= 0,
	.yiPrime	= 0,
	.x			= 0,
	.xp			= 0,
	.xi			= 0,
	.xd			= 0,
};

/// @brief Stores the previous value of the @c powerLimitPid controller's derivative term.
static float powerLimitPidXdPrime = 0.0f;

/// @brief The measurment gain of the @c powerLimitPid controller's low-pass filtering.
static float powerLimitPidA = 0;

// Function Prototypes --------------------------------------------------------------------------------------------------------

/**
 * @brief Calculates the input structure to pass to the selected torque-vectoring algorithm.
 * @param deltaTime The amount of time that has passed since the last call to this function.
 * @return The calculated input structure.
 */
tvInput_t requestCalculateInput (float deltaTime);

/**
 * @brief Executes the selected torque-vectoring algorithm, calculating an amount of torque to request of each inverter.
 * @note This functino only calculates the torque request, power limiting and validation should be performed using
 * @c requestApplyPowerLimit and @c requestValidate , respectively.
 * @param input The input structure, as returned by @c requestCalculateInput .
 * @return The output structure containing the torque to request.
 */
tvOutput_t requestCalculateOutput (tvInput_t* input);

/**
 * @brief Applies power-limiting to a torque request, if applicable. See @c powerLimitPid for more details.
 * @param request The request to power-limit.
 * @param deltaTime The amount of time that has passed since the last call to this function.
 * @return True if the request was de-rated, false otherwise.
 */
bool requestApplyPowerLimit (tvOutput_t* request, float deltaTime);

bool requestApplyRegenLimit (tvOutput_t* output);

bool torqueApplyRegenLimit (float* torque, amkInverter_t* amk);

/**
 * @brief Checks the validity of a torque request.
 * @param request The request to validate.
 * @param input The input structure that generated said request.
 * @return True if the request is valid, false otherwise.
 */
bool requestValidate (tvOutput_t* request, tvInput_t* input);

// Thread Entrypoint ----------------------------------------------------------------------------------------------------------

static THD_WORKING_AREA (torqueThreadWa, 512);
THD_FUNCTION (torqueThread, arg)
{
	(void) arg;
	chRegSetThreadName ("torque_control");

	bool button3Held = false;
	bool button5Held = false;
	systime_t timeCurrent = chVTGetSystemTimeX ();
	while (true)
	{
		// Sleep until next loop.
		systime_t timePrevious = timeCurrent;
		systime_t timeNext = chTimeAddX (timeCurrent, TORQUE_THREAD_PERIOD);
		chThdSleepUntilWindowed (timeCurrent, timeNext);
		timeCurrent = chVTGetSystemTimeX ();

		// Sample the sensor inputs.
		stmAdcSample (&adc);
		pedalsUpdate (&pedals, timePrevious, timeCurrent);
		am4096Sample (&sasAdc);

		// Calculate the torque request and apply power limiting.
		tvInput_t input = requestCalculateInput (TORQUE_THREAD_PERIOD_S);

		// Button inputs
		bool resetRequest = !palReadLine (LINE_BUTTON_2_IN);
		if (!palReadLine (LINE_BUTTON_1_IN))
		{
			// Torque Up / Down
			if (palReadLine (LINE_BUTTON_3_IN))
			{
				button3Held = false;
			}
			else if (!button3Held)
			{
				torqueThreadSetDrivingTorqueLimit (drivingTorqueLimit - 8.4f);
				button3Held = true;
			}

			if (palReadLine (LINE_BUTTON_5_IN))
			{
				button5Held = false;
			}
			else if (!button5Held)
			{
				torqueThreadSetDrivingTorqueLimit (drivingTorqueLimit + 8.4f);
				button5Held = true;
			}
		}
		else
		{
			// TODO(Barach): Temporary
			// if (palReadLine (LINE_BUTTON_3_IN) && palReadLine (LINE_BUTTON_5_IN))
			// 	linearSasBiasMax = eepromMap->linearSasBiasMax;
			// else
			// 	linearSasBiasMax = eepromMap->linearSasBiasMaxSlalom;

			// TODO(Barach): Re-enable
			// // Regen input
			// if (!palReadLine (LINE_BUTTON_3_IN) || !palReadLine (LINE_BUTTON_5_IN))
			// {
			// 	if (!palReadLine (LINE_BUTTON_3_IN) && !palReadLine (LINE_BUTTON_5_IN))
			// 		input.regenRequest = eepromMap->regenHardRequest;
			// 	else
			// 		input.regenRequest = eepromMap->regenLightRequest;
			// }
		}

		torqueRequest = requestCalculateOutput (&input);
		bool derating = requestApplyPowerLimit (&torqueRequest, TORQUE_THREAD_PERIOD_S);
		derating &= requestApplyRegenLimit (&torqueRequest);
		bool plausible = requestValidate (&torqueRequest, &input);

		// Nofify the state thread of the current plausibility.
		stateThreadSetTorquePlausibility (plausible, derating);

		if (vehicleState == VEHICLE_STATE_READY_TO_DRIVE)
		{
			if (plausible)
			{
				// Torque request message.
				amkSendTorqueRequest (&amkRl, torqueRequest.torqueRl, AMK_DRIVING_TORQUE_MAX, -AMK_REGENERATIVE_TORQUE_MAX, resetRequest, TORQUE_THREAD_CAN_MESSAGE_TIMEOUT);
				amkSendTorqueRequest (&amkRr, torqueRequest.torqueRr, AMK_DRIVING_TORQUE_MAX, -AMK_REGENERATIVE_TORQUE_MAX, resetRequest, TORQUE_THREAD_CAN_MESSAGE_TIMEOUT);
				amkSendTorqueRequest (&amkFl, torqueRequest.torqueFl, AMK_DRIVING_TORQUE_MAX, -AMK_REGENERATIVE_TORQUE_MAX, resetRequest, TORQUE_THREAD_CAN_MESSAGE_TIMEOUT);
				amkSendTorqueRequest (&amkFr, torqueRequest.torqueFr, AMK_DRIVING_TORQUE_MAX, -AMK_REGENERATIVE_TORQUE_MAX, resetRequest, TORQUE_THREAD_CAN_MESSAGE_TIMEOUT);
			}
			else
			{
				// Send 0 torque request message (keep torque limits, as lowering them in motion will trigger a fault).
				amkSendTorqueRequest (&amkRl, 0, AMK_DRIVING_TORQUE_MAX, -AMK_REGENERATIVE_TORQUE_MAX, resetRequest, TORQUE_THREAD_CAN_MESSAGE_TIMEOUT);
				amkSendTorqueRequest (&amkRr, 0, AMK_DRIVING_TORQUE_MAX, -AMK_REGENERATIVE_TORQUE_MAX, resetRequest, TORQUE_THREAD_CAN_MESSAGE_TIMEOUT);
				amkSendTorqueRequest (&amkFl, 0, AMK_DRIVING_TORQUE_MAX, -AMK_REGENERATIVE_TORQUE_MAX, resetRequest, TORQUE_THREAD_CAN_MESSAGE_TIMEOUT);
				amkSendTorqueRequest (&amkFr, 0, AMK_DRIVING_TORQUE_MAX, -AMK_REGENERATIVE_TORQUE_MAX, resetRequest, TORQUE_THREAD_CAN_MESSAGE_TIMEOUT);
			}
		}
		else
		{
			// De-energization message.
			amkSendEnergizationRequest (&amkRl, false, resetRequest, TORQUE_THREAD_CAN_MESSAGE_TIMEOUT);
			amkSendEnergizationRequest (&amkRr, false, resetRequest, TORQUE_THREAD_CAN_MESSAGE_TIMEOUT);
			amkSendEnergizationRequest (&amkFl, false, resetRequest, TORQUE_THREAD_CAN_MESSAGE_TIMEOUT);
			amkSendEnergizationRequest (&amkFr, false, resetRequest, TORQUE_THREAD_CAN_MESSAGE_TIMEOUT);
		}
	}
}

// Functions ------------------------------------------------------------------------------------------------------------------

void torqueThreadStart (tprio_t priority)
{
	// Start the torque control thread
	chThdCreateStatic (&torqueThreadWa, sizeof (torqueThreadWa), priority, torqueThread, NULL);
}

void torqueThreadSelectAlgorithm (uint8_t index)
{
	// Clamp index
	if (index < TV_ALGORITHM_COUNT)
		algoritmIndex = index;
}

void torqueThreadSetDrivingTorqueLimit (float torque)
{
	if (torque > AMK_DRIVING_TORQUE_MAX * AMK_COUNT)
	{
		drivingTorqueLimit = AMK_DRIVING_TORQUE_MAX * AMK_COUNT;
		return;
	}

	if (torque < 0)
	{
		drivingTorqueLimit = 0;
		return;
	}

	drivingTorqueLimit = torque;
}

void torqueThreadSetRegenTorqueLimit (float torque)
{
	if (torque > AMK_REGENERATIVE_TORQUE_MAX * AMK_COUNT)
		return;
	regenTorqueLimit = torque;
}

void torqueThreadSetPowerLimit (float powerLimit)
{
	powerLimitPid.ySetPoint = powerLimit;
}

void torqueThreadSetPowerLimitPid (float kp, float ki, float kd, float a)
{
	powerLimitPid.kp	= kp;
	powerLimitPid.ki	= ki;
	powerLimitPid.kd	= kd;
	powerLimitPidA		= a;
}

tvInput_t requestCalculateInput (float deltaTime)
{
	tvInput_t input =
	{
		.deltaTime			= deltaTime,
		.drivingTorqueLimit	= pedals.appsRequest * drivingTorqueLimit,
		.regenTorqueLimit	= regenTorqueLimit,
		.regenRequest		= 0.0f
	};
	return input;
}

tvOutput_t requestCalculateOutput (tvInput_t* input)
{
	// Use the user-specified torque-vectoring algorithm to calculate the torque request.
	return tvAlgorithms [algoritmIndex].entrypoint (input, tvAlgorithms [algoritmIndex].config);
}

bool requestApplyPowerLimit (tvOutput_t* output, float deltaTime)
{
	// Calculate the cumulative power consumption of the inverters.
	float cumulativePower = amksGetCumulativePower (amks, AMK_COUNT);

	// Calculate the torque reduction ratio.
	pidCalculate (&powerLimitPid, cumulativePower, deltaTime);
	pidFilterDerivative (&powerLimitPid, powerLimitPidA, &powerLimitPidXdPrime);
	float torqueReductionRatio = pidApplyAntiWindup (&powerLimitPid, -1.0f, 0.0f) + 1.0f;

	// Scale the torque requests equally by the reduction ratio.
	output->torqueRl *= torqueReductionRatio;
	output->torqueRr *= torqueReductionRatio;
	output->torqueFl *= torqueReductionRatio;
	output->torqueFr *= torqueReductionRatio;

	// If reduction ratio is not 1, derating is occurring.
	return (1.0f - torqueReductionRatio < FLT_EPSILON);
}

bool requestApplyRegenLimit (tvOutput_t* output)
{
	return
		torqueApplyRegenLimit (&output->torqueRl, &amkRl) &&
		torqueApplyRegenLimit (&output->torqueRr, &amkRr) &&
		torqueApplyRegenLimit (&output->torqueFl, &amkFl) &&
		torqueApplyRegenLimit (&output->torqueFr, &amkFr);
}

bool torqueApplyRegenLimit (float* torque, amkInverter_t* amk)
{
	if (*torque < 0)
	{
		if (amk->actualSpeed < eepromMap->regenDeratingSpeedEnd)
		{
			*torque = 0;
			return true;
		}
		else if (amk->actualSpeed < eepromMap->regenDeratingSpeedStart)
		{
			*torque = lerp2d (amk->actualSpeed,
				eepromMap->regenDeratingSpeedEnd, 0,
				eepromMap->regenDeratingSpeedStart, 1);
			return true;
		}
		return false;
	}

	return false;
}

bool requestValidate (tvOutput_t* output, tvInput_t* input)
{
	// Check the algorithm's output validity.
	bool valid = output->valid;

	// Check the pedal plausibility.
	valid &= pedals.plausible;

	// Calculate the cumulative driving and regen torques. These are calculated separately, as regen shouldn't allow more than
	// the max driving torque to be requested, and vice versa.
	float drivingTorque = 0.0f;
	float regenTorque = 0.0f;

	if (output->torqueRl >= 0.0f)
		drivingTorque += output->torqueRl;
	else
	 	regenTorque += output->torqueRl;

	if (output->torqueRr >= 0.0f)
		drivingTorque += output->torqueRr;
	else
	 	regenTorque += output->torqueRr;;

	if (output->torqueFl >= 0.0f)
		drivingTorque += output->torqueFl;
	else
		regenTorque += output->torqueFl;

	if (output->torqueFr >= 0.0f)
		drivingTorque += output->torqueFr;
	else
	 	regenTorque += output->torqueFr;

	// Validate the cumulative torque limits are not exceeded.
	valid &= drivingTorque <= input->drivingTorqueLimit * (1 + CUMULATIVE_TORQUE_TOLERANCE);
	valid &= regenTorque >= -input->regenTorqueLimit * (1 + CUMULATIVE_TORQUE_TOLERANCE);

	return valid;
}