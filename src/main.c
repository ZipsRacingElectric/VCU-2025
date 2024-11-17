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

	// Set the fault LEDs
	palClearLine (LINE_VCU_FLT);
	palClearLine (LINE_IND_RED);
	palSetLine (LINE_IND_GRN);
	palSetLine (LINE_IND_BLU);
}

void canFaultCallback (msg_t result)
{
	(void) result;
	canFault = true;
}