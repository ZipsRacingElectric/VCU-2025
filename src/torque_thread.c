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

// Global Data ----------------------------------------------------------------------------------------------------------------

uint8_t algoritmIndex = 0;

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

	while (true)
	{
		// Sample the sensor inputs
		pedalsSample (&pedals, chVTGetSystemTimeX ());

		// Perform torque vectoring
		tvOutput_t output = tvAlgorithms [algoritmIndex] (0.1);
		output.valid &= pedals.plausible;

		TORQUE_THREAD_PRINTF ("Torque output, algorithm %u\r\n", algoritmIndex);
		TORQUE_THREAD_PRINTF ("\tValid: %u\r\n",	output.valid);
		TORQUE_THREAD_PRINTF ("\tFL: %f\r\n",		output.torqueFl);
		TORQUE_THREAD_PRINTF ("\tFR: %f\r\n",		output.torqueFr);
		TORQUE_THREAD_PRINTF ("\tRL: %f\r\n",		output.torqueRl);
		TORQUE_THREAD_PRINTF ("\tRR: %f\r\n",		output.torqueRr);

		torquePlausible = output.valid;

		// TODO(Barach): Invalidity handling.

		// TODO(Barach): Proper timeouts.
		amkSendMotorRequest (&amkFl, true, true, true, output.torqueFl, torqueRequestLimit, 0, TIME_MS2I (100));
		amkSendMotorRequest (&amkFr, true, true, true, output.torqueFr, torqueRequestLimit, 0, TIME_MS2I (100));
		amkSendMotorRequest (&amkRl, true, true, true, output.torqueRl, torqueRequestLimit, 0, TIME_MS2I (100));
		amkSendMotorRequest (&amkRr, true, true, true, output.torqueRr, torqueRequestLimit, 0, TIME_MS2I (100));

		// Broadcast the sensor input messages
		transmitSensorInputPercent (&CAND1, TIME_MS2I (100));
		transmitSensorInputRaw (&CAND1, TIME_MS2I (100));

		// Control frequency
		// TODO(Barach): Actual timing.
		chThdSleepMilliseconds (100);
	}
}

// Functions ------------------------------------------------------------------------------------------------------------------

void torqueThreadStart (tprio_t priority)
{
	// Initialize torque vectoring code
	torqueVectoringInit ();

	// Start the torque control thread
	chThdCreateStatic (&torqueThreadWa, sizeof (torqueThreadWa), priority, torqueThread, NULL);
}