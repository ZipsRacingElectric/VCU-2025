// Vehicle Control Unit -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.09.21
//
// TODO(Barach):
// - Tune thread priorities.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"
#include "can_thread.h"
#include "peripherals.h"
#include "state_thread.h"
#include "torque_thread.h"

// ChibiOS
#include "hal.h"

// Entrypoint -----------------------------------------------------------------------------------------------------------------

int main (void)
{
	// ChibiOS Initialization
	halInit ();
	chSysInit ();

	// Debug Initialization
	debugInit ("Vehicle Control Board, Revision AB");

	// Peripheral initialization
	peripheralsInit ();

	// CAN thread initialization
	canThreadStart (NORMALPRIO);

	// State thread initialization.
	stateThreadStart (NORMALPRIO);

	// Torque thread initialization
	torqueThreadStart (NORMALPRIO);

	// Do nothing.
	while (true)
		chThdSleepMilliseconds (500);
}

// Interrupts -----------------------------------------------------------------------------------------------------------------

void hardFaultCallback (void)
{
	// Open the shutdown loop
	palClearLine (LINE_SHUTDOWN_EN);

	// Set the fault LED
	palSetLine (LINE_VCU_FLT);
}