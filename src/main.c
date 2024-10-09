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

// ChibiOS
#include "hal.h"

// Debug Thread ---------------------------------------------------------------------------------------------------------------

#define DEBUG_THREAD_RUN 1

static THD_WORKING_AREA(debugThreadWa, 128);
THD_FUNCTION(debugThread, arg)
{
	(void) arg;
	chRegSetThreadName ("Debug Thread");

	while (DEBUG_THREAD_RUN)
	{
		analogSample (&adc);

		DEBUG_PRINTF ("APPS-1: %f.\r\n",	apps1.value);
		DEBUG_PRINTF ("APPS-2: %f.\r\n",	apps2.value);
		DEBUG_PRINTF ("BSE-F: %f.\r\n",	bseF.value);
		DEBUG_PRINTF ("BSE-R: %f.\r\n",	bseR.value);
		DEBUG_PRINTF ("SAS: %f.\r\n",		steeringAngle);
		DEBUG_PRINTF ("GLV: %f.\r\n",		glvBatteryVoltage);

		chThdSleepMilliseconds (500);
	}
}

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

	// Debug Thread Initialization
	chThdCreateStatic (debugThreadWa, sizeof (debugThreadWa), NORMALPRIO, debugThread, NULL);

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
}