// Header
#include "torque_thread.h"

// Includes
#include "can_thread.h"
#include "can/transmit.h"
#include "controls/pid_controller.h"
#include "controls/torque_vectoring.h"
#include "controls/tv_chatfield.h"
#include "controls/tv_straight_diff.h"
#include "debug.h"
#include "peripherals.h"
#include "state_thread.h"

// C Standard Library
#include <float.h>

// Constants ------------------------------------------------------------------------------------------------------------------

#define TORQUE_THREAD_PERIOD TIME_MS2I (10)

// Global Data ----------------------------------------------------------------------------------------------------------------

/// @brief The driver's configured torque settings.
static tvInput_t torqueConfig =
{
	.torqueLimit	= 0.0f,
	.torqueBias		= 0.0f,
	.regenLimit		= 0.0f,
	.regenBias		= 0.0f,
	.deltaTime		= 0.0f
};

/// @brief The index of the selected torque-vectoring algorithm.
static uint8_t algoritmIndex = 0;

/// @brief The array of selectable torque-vectoring algorithms.
#define TV_ALGORITHM_COUNT (sizeof (tvAlgorithms) / sizeof (tvFunction_t*))
static tvFunction_t* tvAlgorithms [] =
{
	&tvStraightDiff, &tvChatfield
};

/// @brief PID controller responsible for enforcing the global power limit. The y variable is the cumulative power consumption,
/// the x variable is the ratio to reduce the torque request by. The set-point is fixed at the power limit, while the output
/// value is clamped from 0 to 1. This means that the controller only has the ability to reduce the power consumption, and this
/// reduction will only occur when the current power consumption exceeds the set-point, which is the target power limit.
static pidController_t powerLimitPid =
{
	.kp			= 0,
	.ki			= 0,
	.kd			= 0,
	.ySetPoint	= 0,
	.iPrime		= 0,
	.dPrime		= 0
};

// Function Prototypes --------------------------------------------------------------------------------------------------------

/**
 * @brief Calculates the amount of torque to request to each inverter based on the vehicle's state. This function is
 * responsible for invoking the selected torque-vectoring algorithm. This function should be called regularly, regardless of
 * whether the request is to be used.
 * @param deltaTime The amount of time that has passed since the last call to this function.
 * @return The calculated request.
 */
tvOutput_t calculateRequest (float deltaTime);

/**
 * @brief Checks whether a torque request is valid.
 * @param output The request to validate.
 */
void validateRequest (tvOutput_t* output);

/**
 * @brief Reduces the calculated torque request based on the current power consumption of the inverters. Power is reduced
 * equally across all 4 inverters to not interfere with torque distribution.
 * @param output The torque request to apply the limit to.
 * @param deltaTime The amount of time that has passed since the last call to this function.
 * @return True if the requested torque was limited, false otherwise.
 */
bool applyPowerLimit (tvOutput_t* output, float deltaTime);

// Thread Entrypoint ----------------------------------------------------------------------------------------------------------

static THD_WORKING_AREA (torqueThreadWa, 512);
THD_FUNCTION (torqueThread, arg)
{
	(void) arg;
	chRegSetThreadName ("torque_control");

	systime_t timePrevious = chVTGetSystemTimeX ();
	while (true)
	{
		// Sample the sensor inputs.
		// TODO(Barach): Sample other ADC values here for simplicity.
		pedalsSample (&pedals, timePrevious);

		// Calculate the torque request and apply power limiting.
		tvOutput_t request = calculateRequest (0.01f);
		applyPowerLimit (&request, 0.01f);

		if (vehicleState == VEHICLE_STATE_READY_TO_DRIVE)
		{
			if (!torquePlausible)
			{
				// Send 0 torque request message (keep torque limits, as lowering them in motion will trigger a fault).
				amkSendTorqueRequest (&AMK_RL, 0, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
				amkSendTorqueRequest (&AMK_RR, 0, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
				amkSendTorqueRequest (&AMK_FL, 0, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
				amkSendTorqueRequest (&AMK_FR, 0, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
			}
			else
			{
				// TODO(Barach): Split global torque limit and individual torque limit.
				// Torque request message.
				amkSendTorqueRequest (&AMK_RL, request.torqueRl, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
				amkSendTorqueRequest (&AMK_RR, request.torqueRr, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
				amkSendTorqueRequest (&AMK_FL, request.torqueFl, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
				amkSendTorqueRequest (&AMK_FR, request.torqueFr, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
			}
		}
		else
		{
			// De-energization message.
			amkSendEnergizationRequest (&AMK_RL, false, TORQUE_THREAD_PERIOD / 6);
			amkSendEnergizationRequest (&AMK_RR, false, TORQUE_THREAD_PERIOD / 6);
			amkSendEnergizationRequest (&AMK_FL, false, TORQUE_THREAD_PERIOD / 6);
			amkSendEnergizationRequest (&AMK_FR, false, TORQUE_THREAD_PERIOD / 6);
		}

		// Broadcast the sensor input messages
		// TODO(Barach): Magic number 6
		transmitSensorInputPercent (&CAND1, TORQUE_THREAD_PERIOD / 6);
		transmitSensorInputRaw (&CAND1, TORQUE_THREAD_PERIOD / 6);

		// Sleep until next loop
		systime_t timeNext = chTimeAddX (timePrevious, TORQUE_THREAD_PERIOD);
		chThdSleepUntilWindowed (timePrevious, timeNext);
		timePrevious = chVTGetSystemTimeX ();
	}
}

// Functions ------------------------------------------------------------------------------------------------------------------

void torqueThreadStart (tprio_t priority)
{
	// Start the torque control thread
	chThdCreateStatic (&torqueThreadWa, sizeof (torqueThreadWa), priority, torqueThread, NULL);
}

void torqueThreadSetTorqueLimit (float torque)
{
	// Check limits
	if (torque <= TORQUE_LIMIT_MAX && torque >= 0)
		torqueConfig.torqueLimit = torque;
}

void torqueThreadSetTorqueBias (float bias)
{
	// Check limits
	if (bias <= 1.0f && bias >= 0.0f)
		torqueConfig.torqueBias = bias;
}

void torqueThreadSetRegenLimit (float regen)
{
	// Check limits
	if (regen <= REGEN_LIMIT_MAX && regen >= 0)
		torqueConfig.regenLimit = regen;
}

void torqueThreadSetRegenBias (float bias)
{
	// Check limits
	if (bias <= 1.0f && bias >= 0.0f)
		torqueConfig.regenBias = bias;
}

void torqueThreadSelectAlgorithm (uint8_t index)
{
	// Clamp index
	if (index < TV_ALGORITHM_COUNT)
		algoritmIndex = index;
}

void torqueThreadSetPowerLimit (float powerLimit)
{
	powerLimitPid.ySetPoint = powerLimit;
}

void torqueThreadSetPowerLimitPid (float kp, float ki, float kd)
{
	powerLimitPid.kp = kp;
	powerLimitPid.ki = ki;
	powerLimitPid.kd = kd;
}

tvOutput_t calculateRequest (float deltaTime)
{
	// Invoke the torque-vectoring algorithm and validate the output request.
	torqueConfig.deltaTime = deltaTime;
	tvOutput_t request = tvAlgorithms [algoritmIndex] (&torqueConfig);
	validateRequest (&request);
	// TODO(Barach): Global
	torquePlausible = request.valid && pedals.plausible;
	return request;
}

void validateRequest (tvOutput_t* output)
{
	// Validate the individual motor torque / regen limits are not exceeded.
	output->valid &= output->torqueRl <= TV_MOTOR_TORQUE_MAX && output->torqueRl >= -TV_MOTOR_REGEN_MAX;
	output->valid &= output->torqueRr <= TV_MOTOR_TORQUE_MAX && output->torqueRr >= -TV_MOTOR_REGEN_MAX;
	output->valid &= output->torqueFl <= TV_MOTOR_TORQUE_MAX && output->torqueFl >= -TV_MOTOR_REGEN_MAX;
	output->valid &= output->torqueFr <= TV_MOTOR_TORQUE_MAX && output->torqueFr >= -TV_MOTOR_REGEN_MAX;

	// Validate the cumulative torque limit is not exceeded
	// TODO(Barach): Factor pedal request into this somehow?
	float cumulativeTorque = output->torqueRl + output->torqueRr + output->torqueFl + output->torqueFr;
	output->valid &= cumulativeTorque <= torqueConfig.torqueLimit && cumulativeTorque >= -torqueConfig.regenLimit;
}

bool applyPowerLimit (tvOutput_t* output, float deltaTime)
{
	// TODO(Barach): Macro
	// Calculate the cumulative power consumption of the inverters.
	float cumulativePower = amksGetCumulativePower (amks, 1);

	float derating = pidAntiWindup (&powerLimitPid, cumulativePower, deltaTime, -1, 0);

	// Scale the torque requests equally by the reduction ratio.
	output->torqueRl *= (1 + derating);
	output->torqueRr *= (1 + derating);
	output->torqueFl *= (1 + derating);
	output->torqueFr *= (1 + derating);

	// TODO(Barach): Global
	torqueDerating = (derating < 0.0f - FLT_EPSILON);
	return torqueDerating;
}