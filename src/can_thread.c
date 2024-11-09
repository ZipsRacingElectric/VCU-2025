// Header
#include "can_thread.h"

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can/receive.h"
#include "debug.h"

// ChibiOS
#include "hal.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

#if CAN_THREAD_DEBUGGING
	#define CAN_THREAD_PRINTF(format, ...) DEBUG_PRINTF ("[CAN Thread] " format, ##__VA_ARGS__)
#else
	#define CAN_THREAD_PRINTF(format, ...) while (false)
#endif // CAN_THREAD_DEBUGGING

// Global Nodes ---------------------------------------------------------------------------------------------------------------

amkInverter_t	amkFl;
amkInverter_t	amkFr;
amkInverter_t	amkRl;
amkInverter_t	amkRr;
bms_t			bms;
ecumasterGps_t	gps;

canNode_t* nodes [] =
{
	(canNode_t*) &amkFl, (canNode_t*) &amkFr, (canNode_t*) &amkRl, (canNode_t*) &amkRr,
	(canNode_t*) &bms, (canNode_t*) &gps
};

#define NODE_COUNT (sizeof (nodes) / sizeof (canNode_t*))

// Configurations -------------------------------------------------------------------------------------------------------------

#define CAN_THREAD_TIMEOUT_POLL_PERIOD TIME_MS2I (10)

/**
 * @brief Configuration of the CAN 1 & CAN 2 peripherals.
 * @note See section 32.9 of the STM32F405 Reference Manual for more details.
 */
static CANConfig canConfig =
{
	.mcr = 	CAN_MCR_ABOM |		// Automatic bus-off management.
			CAN_MCR_AWUM |		// Automatic wakeup mode.
			CAN_MCR_TXFP,		// Chronologic FIFI priority.
	.btr =	CAN_BTR_SJW (0) |	// Max 1 TQ resynchronization jump.
			CAN_BTR_TS2 (1) |	// 2 TQ for time segment 2
			CAN_BTR_TS1 (10) |	// 11 TQ for time segment 1
			CAN_BTR_BRP (2)		// Baudrate divisor of 3 (1 Mbps)
};

amkInverterConfig_t amkFlConfig =
{
	.driver			= &CAND1,
	.baseId			= 0x200,
	.timeoutPeriod	= TIME_MS2I (100),
};

amkInverterConfig_t amkFrConfig =
{
	.driver			= &CAND1,
	.baseId			= 0x201,
	.timeoutPeriod	= TIME_MS2I (100),
};

amkInverterConfig_t amkRlConfig =
{
	.driver			= &CAND1,
	.baseId			= 0x202,
	.timeoutPeriod	= TIME_MS2I (100),
};

amkInverterConfig_t amkRrConfig =
{
	.driver			= &CAND1,
	.baseId			= 0x203,
	.timeoutPeriod	= TIME_MS2I (100),
};

bmsConfig_t bmsConfig =
{
	.driver			= &CAND1,
	.timeoutPeriod	= TIME_MS2I (100),
};

ecumasterGpsConfig_t gpsConfig =
{
	.driver			= &CAND1,
	.timeoutPeriod	= TIME_MS2I (100),
};

// Thread Entrypoint ----------------------------------------------------------------------------------------------------------

static THD_WORKING_AREA (canRxThreadWa, 512);
THD_FUNCTION (canRxThread, arg)
{
	(void) arg;
	chRegSetThreadName ("can_rx");

	CANRxFrame frame;

	while (true)
	{
		// Block until the next message arrives
		msg_t result = canReceiveTimeout (&CAND1, CAN_ANY_MAILBOX, &frame, CAN_THREAD_TIMEOUT_POLL_PERIOD);
		if (result == MSG_OK)
		{
			// Find the handler of the message
			if (!canNodesReceive (nodes, NODE_COUNT, &frame))
			{
				// If no node handled the message, check if it is directly for the VCU.
				if (!receiveMessage (&frame))
					CAN_THREAD_PRINTF ("Received unknown CAN message. ID: 0x%X.\r\n", frame.SID);
			}
		}
		else if (result != MSG_TIMEOUT)
			CAN_THREAD_PRINTF ("Failed to receive from CAN1: Error %i.\r\n", result);

		canNodesCheckTimeout (nodes, NODE_COUNT);
	}
}

void canThreadStart (tprio_t priority)
{
	// CAN 1 driver initialization
	if (canStart (&CAND1, &canConfig) != MSG_OK)
		CAN_THREAD_PRINTF ("Failed to initialize CAN1.");

	palClearLine (LINE_CAN1_STBY);

	// Initialize the CAN nodes
	amkInit (&amkFl, &amkFlConfig);
	amkInit (&amkFr, &amkFrConfig);
	amkInit (&amkRl, &amkRlConfig);
	amkInit (&amkRr, &amkRrConfig);
	bmsInit (&bms, &bmsConfig);
	ecumasterInit (&gps, &gpsConfig);

	// Create the CAN thread
	chThdCreateStatic (&canRxThreadWa, sizeof (canRxThreadWa), priority, canRxThread, NULL);
}