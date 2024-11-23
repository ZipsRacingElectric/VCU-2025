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

/// @brief The global torque limit. Configured by the driver.
float torqueLimit = 0.0f;

/// @brief The index of the selected torque-vectoring algorithm.
uint8_t algoritmIndex = 0;

/// @brief The array of selectable torque-vectoring algorithms.
#define TV_ALGORITHM_COUNT (sizeof (tvAlgorithms) / sizeof (tvFunction_t*))
static tvFunction_t* tvAlgorithms [] =
{
	&tvStraightDiff, &tvChatfield
};

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
		tvOutput_t output = tvAlgorithms [algoritmIndex] (0.1f, torqueLimit);
		stateThreadSetTorquePlausibility (output.valid && pedals.plausible);

		if (vehicleState == VEHICLE_STATE_READY_TO_DRIVE && torquePlausible)
		{
			amkSendMotorRequest (&amkFl, true, true, true, output.torqueFl, torqueLimit, 0, TORQUE_THREAD_PERIOD / 4);
			amkSendMotorRequest (&amkFr, true, true, true, output.torqueFr, torqueLimit, 0, TORQUE_THREAD_PERIOD / 4);
			amkSendMotorRequest (&amkRl, true, true, true, output.torqueRl, torqueLimit, 0, TORQUE_THREAD_PERIOD / 4);
			amkSendMotorRequest (&amkRr, true, true, true, output.torqueRr, torqueLimit, 0, TORQUE_THREAD_PERIOD / 4);
		}
		else
		{
			amkSendMotorRequest (&amkFl, false, false, false, 0, 0, 0, TORQUE_THREAD_PERIOD / 4);
			amkSendMotorRequest (&amkFr, false, false, false, 0, 0, 0, TORQUE_THREAD_PERIOD / 4);
			amkSendMotorRequest (&amkRl, false, false, false, 0, 0, 0, TORQUE_THREAD_PERIOD / 4);
			amkSendMotorRequest (&amkRr, false, false, false, 0, 0, 0, TORQUE_THREAD_PERIOD / 4);
		}

		// Broadcast the sensor input messages
		transmitSensorInputPercent (&CAND1, TIME_MS2I (100));
		transmitSensorInputRaw (&CAND1, TIME_MS2I (100));

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

void torqueThreadSetLimit (float torque)
{
	if (torque <= TORQUE_LIMIT_MAX && torque >= 0)
		torqueLimit = torque;
}

void torqueThreadSelectAlgorithm (uint8_t index)
{
	if (index < TV_ALGORITHM_COUNT)
		algoritmIndex = index;
}