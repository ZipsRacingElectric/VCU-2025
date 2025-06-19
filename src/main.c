// Vehicle Control Unit -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.09.21
//
// Description: Entrypoint and interrupt handling for the vehicle control unit. See the respective threads for specific
//   responsibilities of the the VCU.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"
#include "can_thread.h"
#include "peripherals.h"
#include "state_thread.h"
#include "torque_thread.h"

// ChibiOS
#include "hal.h"

// Interrupts -----------------------------------------------------------------------------------------------------------------

void hardFaultCallback (void)
{
	// Open the shutdown loop
	palClearLine (LINE_SHUTDOWN_EN);

	// Set the fault LED
	palSetLine (LINE_VCU_FLT);
}

// Entrypoint -----------------------------------------------------------------------------------------------------------------

int main (void)
{
	// ChibiOS Initialization
	halInit ();
	chSysInit ();

	// Debug initialization
	ioline_t ledLine = LINE_LED_HEARTBEAT;
	debugHeartbeatStart (&ledLine, LOWPRIO);

	// Peripheral initialization
	if (!peripheralsInit ())
	{
		hardFaultCallback ();
		while (true);
	}

	// CAN thread initialization. Start this first as to invalidate all can nodes before any other threads attempt reading
	// any data.
	if (!canThreadStart (NORMALPRIO))
	{
		hardFaultCallback ();
		while (true);
	}

	// Torque thread initialization. Start this at the highest priority as it has the strictest timing.
	torqueThreadStart (NORMALPRIO + 1);

	// State thread initialization. Start this at a lower priority as it has the least strict timing.
	stateThreadStart (NORMALPRIO - 1);

	// Allow the shutdown loop to close.
	palSetLine (LINE_SHUTDOWN_EN);

	// Do nothing.
	while (true)
		chThdSleepMilliseconds (500);
}