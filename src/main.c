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

	// CAN thread initialization. Do this first as to invalidate all can nodes before any reads are done.
	canThreadStart (NORMALPRIO);

	// Torque thread initialization.
	torqueThreadStart (NORMALPRIO + 1);

	// State thread initialization.
	stateThreadStart (NORMALPRIO - 1);

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