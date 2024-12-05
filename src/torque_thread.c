// Header
#include "torque_thread.h"

// Includes
#include "can_thread.h"
#include "can/transmit.h"
#include "controls/torque_vectoring.h"
#include "controls/tv_chatfield.h"
#include "controls/tv_straight_diff.h"
#include "debug.h"
#include "peripherals.h"
#include "state_thread.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

#if TORQUE_THREAD_DEBUGGING
	#define TORQUE_THREAD_PRINTF(format, ...) DEBUG_PRINTF ("[Torque Thread] " format, ##__VA_ARGS__)
#else
	#define TORQUE_THREAD_PRINTF(format, ...) while (false)
#endif // TORQUE_THREAD_DEBUGGING

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

// Function Prototypes --------------------------------------------------------------------------------------------------------

void checkValidity (tvOutput_t* output);

// Thread Entrypoint ----------------------------------------------------------------------------------------------------------

static THD_WORKING_AREA (torqueThreadWa, 512);
THD_FUNCTION (torqueThread, arg)
{
	(void) arg;
	chRegSetThreadName ("torque_control");

	systime_t timePrevious = chVTGetSystemTimeX ();
	while (true)
	{
		// Sample the sensor inputs
		pedalsSample (&pedals, chVTGetSystemTimeX ());

		// Perform torque vectoring
		torqueConfig.deltaTime = 0.1f;
		tvOutput_t output = tvAlgorithms [algoritmIndex] (&torqueConfig);
		checkValidity (&output);
		bool plas = output.valid && pedals.plausible;
		stateThreadSetTorquePlausibility (plas);

		// TODO(Barach): Power limiting.

		if (vehicleState == VEHICLE_STATE_READY_TO_DRIVE)
		{
			if (!torquePlausible)
			{
				// Send 0 torque request message (keep limits as lowering them while they are running may trigger a fault).
				// TODO(Barach): Split global torque limit and individual torque limit.
				amkSendTorqueRequest (&amkRl, 0, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
				amkSendTorqueRequest (&amkRr, 0, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
				amkSendTorqueRequest (&amkFl, 0, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
				amkSendTorqueRequest (&amkFr, 0, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
			}
			else
			{
				// Torque request message.
				amkSendTorqueRequest (&amkRl, output.torqueRl, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
				amkSendTorqueRequest (&amkRr, output.torqueRr, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
				amkSendTorqueRequest (&amkFl, output.torqueFl, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
				amkSendTorqueRequest (&amkFr, output.torqueFr, TV_MOTOR_TORQUE_MAX, -TV_MOTOR_REGEN_MAX, TORQUE_THREAD_PERIOD / 6);
			}
		}
		else
		{
			// De-energization message.
			amkSendEnergizationRequest (&amkRl, false, TORQUE_THREAD_PERIOD / 6);
			amkSendEnergizationRequest (&amkRr, false, TORQUE_THREAD_PERIOD / 6);
			amkSendEnergizationRequest (&amkFl, false, TORQUE_THREAD_PERIOD / 6);
			amkSendEnergizationRequest (&amkFr, false, TORQUE_THREAD_PERIOD / 6);
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

void checkValidity (tvOutput_t* output)
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