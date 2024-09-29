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

// Driver Configuration -------------------------------------------------------------------------------------------------------

/**
 * @brief Configuration of the CAN 1 & CAN 2 peripherals.
 * @note See section 32.9 of the STM32F405 Reference Manual for more details.
 */
static const CANConfig canConfig =
{
	.mcr = 	CAN_MCR_ABOM |		// Automatic bus-off management.
			CAN_MCR_AWUM |		// Automatic wakeup mode.
			CAN_MCR_TXFP,		// Chronologic FIFI priority.
	.btr =	CAN_BTR_SJW(0) |	// Max 1 TQ resynchronization jump. 
			CAN_BTR_TS2(1) |	// 2 TQ for time segment 2
			CAN_BTR_TS1(10) |	// 11 TQ for time segment 1
			CAN_BTR_BRP(2)		// Baudrate divisor of 3 (1 Mbps)
};

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
	// CAN 1 driver initialization
	if (canStart (&CAND1, &canConfig) != MSG_OK)
	{
		CAN_THREAD_PRINTF ("Failed to initialize CAN1");
		return;
	}
	palClearLine (LINE_CAN1_STBY);
	
	// CAN 2 driver initialization
	if (canStart (&CAND2, &canConfig) != MSG_OK)
	{
		CAN_THREAD_PRINTF ("Failed to initialize CAN2");
		return;
	}
	palClearLine (LINE_CAN2_STBY);

	// CAN thread creation
	chThdCreateStatic (&canThreadWa, sizeof (canThreadWa), priority, canThread, (void*) name);
}