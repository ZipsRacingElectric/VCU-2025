// Header
#include "can_thread.h"

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can/receive.h"

// ChibiOS
#include "hal.h"

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
		.driver			= &CAND1,
		.baseId			= 0x200,
		.timeoutPeriod	= TIME_MS2I (100),
	},
	// RR
	{
		.driver			= &CAND1,
		.baseId			= 0x201,
		.timeoutPeriod	= TIME_MS2I (100),
	},
	// FL
	{
		.driver			= &CAND1,
		.baseId			= 0x202,
		.timeoutPeriod	= TIME_MS2I (100),
	},
	// FR
	{
		.driver			= &CAND1,
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

static THD_WORKING_AREA (canRxThreadWa, 512);
THD_FUNCTION (canRxThread, arg)
{
	(void) arg;
	chRegSetThreadName ("can_rx");

	CANRxFrame frame;

	systime_t timeCurrent = chVTGetSystemTimeX ();
	systime_t timePrevious;

	while (true)
	{
		// Block until the next message arrives
		msg_t result = canReceiveTimeout (&CAND1, CAN_ANY_MAILBOX, &frame, CAN_THREAD_TIMEOUT_POLL_PERIOD);
		timePrevious = timeCurrent;
		timeCurrent = chVTGetSystemTimeX ();

		if (result == MSG_OK)
		{
			// Find the handler of the message
			if (!canNodesReceive (nodes, NODE_COUNT, &frame))
			{
				// If no node handled the message, check if it is directly for the VCU.
				receiveMessage (&frame);
			}
		}

		canNodesCheckTimeout (nodes, NODE_COUNT, timePrevious, timeCurrent);
	}
}

bool canThreadStart (tprio_t priority)
{
	// CAN 1 driver initialization
	if (canStart (&CAND1, &CAN1_CONFIG) != MSG_OK)
		return false;

	palClearLine (LINE_CAN1_STBY);

	// Initialize the CAN nodes
	for (uint8_t index = 0; index < AMK_COUNT; ++index)
		amkInit (amks + index, AMK_CONFIGS + index);

	ecumasterInit (&gps, &GPS_CONFIG);

	// Create the CAN thread
	chThdCreateStatic (&canRxThreadWa, sizeof (canRxThreadWa), priority, canRxThread, NULL);
	return true;
}