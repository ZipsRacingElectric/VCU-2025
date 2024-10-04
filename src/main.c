// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"
#include "can_thread.h"
#include "peripherals.h"

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
	canThreadStart (NORMALPRIO, "can_thread");

	// Do nothing.
	while (true)
		chThdSleepMilliseconds (500);
}
