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
	#define CAN_THREAD_PRINTF(format, ...) while(false)
#endif // CAN_THREAD_DEBUGGING

// Global Nodes ---------------------------------------------------------------------------------------------------------------

amkInverter_t	amkFl;
amkInverter_t	amkFr;
amkInverter_t	amkRl;
amkInverter_t	amkRr;
ecumasterGps_t	gps;
misc_t			misc;

canNode_t* nodes [] =
{
	(canNode_t*) &amkFl, (canNode_t*) &amkFr, (canNode_t*) &amkRl, (canNode_t*) &amkRr, (canNode_t*) &gps, (canNode_t*) &misc
};

// Configuration --------------------------------------------------------------------------------------------------------------

/**
 * @brief Configuration of the CAN 1 & CAN 2 peripherals.
 * @note See section 32.9 of the STM32F405 Reference Manual for more details.
 */
static CANConfig canConfig =
{
	.mcr = 	CAN_MCR_ABOM |		// Automatic bus-off management.
			CAN_MCR_AWUM |		// Automatic wakeup mode.
			CAN_MCR_TXFP,		// Chronologic FIFI priority.
	.btr =	CAN_BTR_SJW(0) |	// Max 1 TQ resynchronization jump. 
			CAN_BTR_TS2(1) |	// 2 TQ for time segment 2
			CAN_BTR_TS1(10) |	// 11 TQ for time segment 1
			CAN_BTR_BRP(2)		// Baudrate divisor of 3 (1 Mbps)
};

#define INVERTER_FL_BASE_ID 0x200
#define INVERTER_FR_BASE_ID 0x206
#define INVERTER_RL_BASE_ID 0x20C
#define INVERTER_RR_BASE_ID 0x212

// Functions ------------------------------------------------------------------------------------------------------------------

bool handleRxFrame (CANRxFrame* rxFrame);

bool handleRxFrame (CANRxFrame* rxFrame)
{
	// Search each node's handler for the correct one.
	for (uint8_t nodeIndex = 0; nodeIndex < sizeof (nodes) / sizeof (canNode_t*); ++nodeIndex)
	{
		for (uint8_t handlerIndex = 0; handlerIndex < nodes [nodeIndex]->handlerCount; ++handlerIndex)
		{
			if (rxFrame->SID == nodes [nodeIndex]->addresses [handlerIndex])
			{
				// Call the handler
				nodes [nodeIndex]->handlers [handlerIndex] (rxFrame);
				return true;
			}
		}
	}

	return false;
}

// Thread Entrypoints ---------------------------------------------------------------------------------------------------------

static THD_WORKING_AREA(canTxThreadWa, 512);
THD_FUNCTION(canTxThread, arg)
{
	(void) arg;
	chRegSetThreadName ("can_tx");

	CANTxFrame txFrame =
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
		msg_t result = canTransmitTimeout (&CAND1, CAN_ANY_MAILBOX, &txFrame, CAN_THREAD_TX_TIMEOUT);
		if (result != MSG_OK)
			CAN_THREAD_PRINTF ("Failed to transmit on CAN1: Error %i.\r\n", result);

		chThdSleepMilliseconds (500);
	}
}

static THD_WORKING_AREA(canRxThreadWa, 512);
THD_FUNCTION(canRxThread, arg)
{
	(void) arg;
	chRegSetThreadName ("can_rx");

	CANRxFrame rxFrame;

	while (true)
	{
		// Block until the next message arrives
		msg_t result = canReceiveTimeout (&CAND1, CAN_ANY_MAILBOX, &rxFrame, TIME_INFINITE);
		if (result != MSG_OK)
		{
			CAN_THREAD_PRINTF ("Failed to receive from CAN1: Error %i.\r\n", result);
			continue;
		}

		// Find the handler of the message
		bool handled = handleRxFrame (&rxFrame);
		if (!handled)
			CAN_THREAD_PRINTF ("Received unknown CAN message. ID: 0x%X.\r\n", rxFrame.SID);
	}
}

void canThreadStart (tprio_t priority)
{
	// CAN 1 driver initialization
	if (canStart (&CAND1, &canConfig) != MSG_OK)
		CAN_THREAD_PRINTF ("Failed to initialize CAN1.");
	
	palClearLine (LINE_CAN1_STBY);

	// Initialize the CAN nodes
	amkInit (&amkFl, INVERTER_FL_BASE_ID, &CAND1);
	amkInit (&amkFr, INVERTER_FR_BASE_ID, &CAND1);
	amkInit (&amkRl, INVERTER_RL_BASE_ID, &CAND1);
	amkInit (&amkRr, INVERTER_RR_BASE_ID, &CAND1);
	canNodeInit ((canNode_t*) &gps, &ecumasterGpsConfig, &CAND1);
	canNodeInit ((canNode_t*) &misc, &miscConfig, &CAND1);
	
	// Create the CAN threads
	chThdCreateStatic (&canTxThreadWa, sizeof (canTxThreadWa), priority, canTxThread, NULL);
	chThdCreateStatic (&canRxThreadWa, sizeof (canRxThreadWa), priority, canRxThread, NULL);
}