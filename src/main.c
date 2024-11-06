// Vehicle Control Unit -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.09.21
//
// To do:
// - Thread priorities.

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
	debugInit ("Vehicle Control Board, Revision AA");

	// Peripheral Initialization
	peripheralsInit ();

	// CAN Thread Initialization
	canThreadStart (NORMALPRIO);

	// Torque Thread Initialization
	torqueThreadStart (NORMALPRIO);

	// State Thread Initialization
	stateThreadStart (NORMALPRIO);

	// Do nothing.
	while (true)
		chThdSleepMilliseconds (500);
}

// Interrupts -----------------------------------------------------------------------------------------------------------------

void faultCallback (void)
{
	// Open the shutdown loop
	palClearLine (LINE_SHUTDOWN_EN);

	// Set the fault LEDs
	palSetLine (LINE_VCU_FLT);
	palSetLine (LINE_IND_RED);
	palClearLine (LINE_IND_GRN);
	palClearLine (LINE_IND_BLU);
}