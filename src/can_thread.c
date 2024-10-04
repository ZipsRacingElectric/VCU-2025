// Header
#include "can_thread.h"

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"

// ChibiOS
#include "hal.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

#if CAN_THREAD_DEBUGGING
	#define CAN_THREAD_PRINTF(format, ...) DEBUG_PRINTF("[CAN Thread] " format, ##__VA_ARGS__)
#else
	#define CAN_THREAD_PRINTF()
#endif // CAN_THREAD_DEBUGGING

// Thread Entrypoint ----------------------------------------------------------------------------------------------------------

static THD_WORKING_AREA(canThreadWa, 128);
THD_FUNCTION(canThread, name)
{
	chRegSetThreadName ((const char*) name);

	CANTxFrame frame1 =
	{
		.DLC = 8,
		.RTR = 0,
		.IDE = CAN_IDE_STD,
		.SID = 0x123,
		.data8 =
		{
			0x01,
			0x23,
			0x45,
			0x67,
			0x89,
			0xAB,
			0xCD,
			0xEF
		}
	};

	while (true)
	{
		if (canTransmit (&CAND1, CAN_ANY_MAILBOX, &frame1, TIME_INFINITE) != MSG_OK)
			CAN_THREAD_PRINTF ("Failed to transmit on CAN1");

		chThdSleepMilliseconds (500);
	}
}

void canThreadStart (tprio_t priority, const char* name)
{
	// Create the CAN thread
	chThdCreateStatic (&canThreadWa, sizeof (canThreadWa), priority, canThread, (void*) name);
}