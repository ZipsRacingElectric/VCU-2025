// Vehicle Control Unit -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.09.21
//
// To do:
// - Better fault handling. Most of this can be in common, but parts will be board specific.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"
#include "can_thread.h"
#include "peripherals.h"
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

	// Do nothing.
	while (true)
		chThdSleepMilliseconds (500);
}

// Interrupts -----------------------------------------------------------------------------------------------------------------

void HardFault_Handler (void)
{
	// Disable further interrupts
	__asm__ ("cpsid i");

	// TODO(Barach): Enter safe state.

	// Halt for the debugger
	__asm__ ("bkpt");

	// Infinite loop to halt the program
	while (true);
}