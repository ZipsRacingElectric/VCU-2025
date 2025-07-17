// Header
#include "can_thread.h"

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can/receive.h"
#include "can/transmit.h"

// ChibiOS
#include "hal.h"

// C Standard Library
#include <string.h>

// Global Nodes ---------------------------------------------------------------------------------------------------------------

amkInverter_t	amks [AMK_COUNT];
ecumasterGps_t	gps;

canNode_t* nodes [] =
{
	(canNode_t*) &amkRl, (canNode_t*) &amkRr, (canNode_t*) &amkFl, (canNode_t*) &amkFr,
	(canNode_t*) &gps
};

#define NODE_COUNT (sizeof (nodes) / sizeof (canNode_t*))

// Configurations -------------------------------------------------------------------------------------------------------------

#define CAN_THREAD_TIMEOUT_POLL_PERIOD TIME_MS2I (10)

#define CAN_TX_THREAD_PERIOD TIME_MS2I (1000)

/**
 * @brief Configuration of the CAN 1 & CAN 2 peripherals.
 * @note See section 32.9 of the STM32F405 Reference Manual for more details.
 */
static const CANConfig CAN1_CONFIG =
{
	.mcr = 	CAN_MCR_ABOM |		// Automatic bus-off management.
			CAN_MCR_AWUM |		// Automatic wakeup mode.
			CAN_MCR_TXFP,		// Chronologic FIFI priority.
	.btr =	CAN_BTR_SJW (0) |	// Max 1 TQ resynchronization jump.
			CAN_BTR_TS2 (1) |	// 2 TQ for time segment 2
			CAN_BTR_TS1 (10) |	// 11 TQ for time segment 1
			CAN_BTR_BRP (2)		// Baudrate divisor of 3 (1 Mbps)
};

static const amkInverterConfig_t AMK_CONFIGS [AMK_COUNT] =
{
	// RL
	{
		.mainDriver		= &CAND2,
		.bridgeDriver	= &CAND1,
		.baseId			= 0x200,
		.timeoutPeriod	= TIME_MS2I (100),
	},
	// RR
	{
		.mainDriver		= &CAND2,
		.bridgeDriver	= &CAND1,
		.baseId			= 0x201,
		.timeoutPeriod	= TIME_MS2I (100),
	},
	// FL
	{
		.mainDriver		= &CAND2,
		.bridgeDriver	= &CAND1,
		.baseId			= 0x202,
		.timeoutPeriod	= TIME_MS2I (100),
	},
	// FR
	{
		.mainDriver		= &CAND2,
		.bridgeDriver	= &CAND1,
		.baseId			= 0x203,
		.timeoutPeriod	= TIME_MS2I (100),
	}
};

static const ecumasterGpsConfig_t GPS_CONFIG =
{
	.driver			= &CAND1,
	.timeoutPeriod	= TIME_MS2I (100),
};

// Thread Entrypoint ----------------------------------------------------------------------------------------------------------

static THD_WORKING_AREA (can1RxThreadWa, 512);
THD_FUNCTION (can1RxThread, arg)
{
	(void) arg;
	chRegSetThreadName ("can1_rx");

	CANRxFrame rxFrame;

	while (true)
	{
		// Block until the next message arrives
		msg_t result = canReceiveTimeout (&CAND1, CAN_ANY_MAILBOX, &rxFrame, CAN_THREAD_TIMEOUT_POLL_PERIOD);

		if (result == MSG_OK)
		{
			// Find the handler of the message
			if (!canNodesReceive (nodes, NODE_COUNT, &rxFrame))
			{
				// If no node handled the message, check if it is directly for the VCU.
				receiveMessage (&rxFrame);
			}
		}
	}
}

static THD_WORKING_AREA (can2RxThreadWa, 512);
THD_FUNCTION (can2RxThread, arg)
{
	(void) arg;
	chRegSetThreadName ("can2_rx");

	CANRxFrame rxFrame;

	systime_t timeCurrent = chVTGetSystemTimeX ();
	systime_t timePrevious;

	while (true)
	{
		// Block until the next message arrives
		msg_t result = canReceiveTimeout (&CAND2, CAN_ANY_MAILBOX, &rxFrame, CAN_THREAD_TIMEOUT_POLL_PERIOD);
		timePrevious = timeCurrent;
		timeCurrent = chVTGetSystemTimeX ();

		if (result == MSG_OK)
		{
			// Find the handler of the message
			if (!canNodesReceive (nodes, NODE_COUNT, &rxFrame))
			{
				// If no node handled the message, check if it is directly for the VCU.
				receiveMessage (&rxFrame);
			}
		}

		CANTxFrame txFrame;

			txFrame.SID = rxFrame.SID;
			txFrame.DLC = rxFrame.DLC;
			txFrame.IDE = rxFrame.IDE;
			memcpy (txFrame.data8, rxFrame.data8, rxFrame.DLC);

			canTransmitTimeout (&CAND1, CAN_ANY_MAILBOX, &txFrame, TIME_MS2I (500));

		canNodesCheckTimeout (nodes, NODE_COUNT, timePrevious, timeCurrent);
	}
}

static THD_WORKING_AREA (can1TxThreadWa, 512);
THD_FUNCTION (can1TxThread, arg)
{
	(void) arg;
	chRegSetThreadName ("can1_tx");

	systime_t timeCurrent = chVTGetSystemTimeX ();
	while (true)
	{
		// Sleep until next loop.
		systime_t timeNext = chTimeAddX (timeCurrent, CAN_TX_THREAD_PERIOD);
		chThdSleepUntilWindowed (timeCurrent, timeNext);
		timeCurrent = chVTGetSystemTimeX ();

		transmitConfig0Message (&CAND1, CAN_TX_THREAD_PERIOD);
		transmitConfig2Message (&CAND1, CAN_TX_THREAD_PERIOD);
		transmitConfig3Message (&CAND1, CAN_TX_THREAD_PERIOD);
	}
}

bool canThreadStart (tprio_t priority)
{
	// CAN 1 driver initialization
	if (canStart (&CAND1, &CAN1_CONFIG) != MSG_OK)
		return false;

	palClearLine (LINE_CAN1_STBY);

	// CAN 2 driver initialization
	if (canStart (&CAND2, &CAN1_CONFIG) != MSG_OK)
		return false;

	palClearLine (LINE_CAN2_STBY);

	// Initialize the CAN nodes
	for (uint8_t index = 0; index < AMK_COUNT; ++index)
		amkInit (amks + index, AMK_CONFIGS + index);

	ecumasterInit (&gps, &GPS_CONFIG);

	// Create the CAN 1 RX thread
	chThdCreateStatic (&can1RxThreadWa, sizeof (can1RxThreadWa), priority, can1RxThread, NULL);

	// Create the CAN 2 RX thread
	chThdCreateStatic (&can2RxThreadWa, sizeof (can2RxThreadWa), priority, can2RxThread, NULL);

	// // Create the CAN 1 TX thread
	// chThdCreateStatic (&can1TxThreadWa, sizeof (can1TxThreadWa), LOWPRIO, can1TxThread, NULL);

	return true;
}