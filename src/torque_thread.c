// Header
#include "torque_thread.h"

// Includes
#include "can_thread.h"
#include "debug.h"
#include "peripherals.h"
#include "controls/torque_vectoring.h"
#include "controls/tv_chatfield.h"
#include "controls/tv_straight_diff.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

#if TORQUE_THREAD_DEBUGGING
	#define TORQUE_THREAD_PRINTF(format, ...) DEBUG_PRINTF("[Torque Thread] " format, ##__VA_ARGS__)
#else
	#define TORQUE_THREAD_PRINTF(format, ...) while(false)
#endif // TORQUE_THREAD_DEBUGGING

// Global Data ----------------------------------------------------------------------------------------------------------------

uint8_t algoritmIndex = 0;

static tvFunction_t* tvAlgorithms [] =
{
	&tvStraightDiff, &tvChatfield
};

// Thread Entrypoint ----------------------------------------------------------------------------------------------------------

static THD_WORKING_AREA(tvThreadWa, 512);
THD_FUNCTION(tvThread, arg)
{
	(void) arg;
	chRegSetThreadName ("torque_control");

	while (true)
	{
		// Sample the sensor inputs
		analogSample (&adc);
		pedalsUpdate (&pedals);

		// Perform torque vectoring
		tvOutput_t output = tvAlgorithms [algoritmIndex] (0.1);

		TORQUE_THREAD_PRINTF ("Torque output, algorithm %u\r\n", algoritmIndex);
		TORQUE_THREAD_PRINTF ("\tValid: %u\r\n",		output.valid);
		TORQUE_THREAD_PRINTF ("\tFront Left: %f\r\n",	output.frontLeftTorqueRequest);
		TORQUE_THREAD_PRINTF ("\tFront Right: %f\r\n",	output.frontRightTorqueRequest);
		TORQUE_THREAD_PRINTF ("\tRear Left: %f\r\n",	output.rearLeftTorqueRequest);
		TORQUE_THREAD_PRINTF ("\tRear Right: %f\r\n",	output.rearRightTorqueRequest);

		// TODO(Barach): Proper timeouts.
		msg_t result = amkSendTorqueRequest (&inverterLeft, output.frontLeftTorqueRequest,
			output.rearLeftTorqueRequest, TIME_MS2I(100));

		if (result != MSG_OK)
		{
			TORQUE_THREAD_PRINTF ("CAN Error: %i", result);
			break;
		}

		result = amkSendTorqueRequest (&inverterRight, output.frontRightTorqueRequest,
			output.rearRightTorqueRequest, TIME_MS2I(100));

		if (result != MSG_OK)
		{
			TORQUE_THREAD_PRINTF ("CAN Error: %i", result);
			break;
		}

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
	chThdCreateStatic (&tvThreadWa, sizeof (tvThreadWa), priority, tvThread, NULL);
}