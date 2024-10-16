// Header
#include "state_thread.h"

// Includes
#include "can_thread.h"
#include "peripherals.h"

// ChibiOS
#include "hal.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define STATE_CONTROL_PERIOD	TIME_MS2I (20)

#define BUZZER_TIME_PERIOD		TIME_MS2I (1000)
#define HV_INACTIVE_PERIOD		TIME_MS2I (10)

// Thread Entrypoint ----------------------------------------------------------------------------------------------------------

static THD_WORKING_AREA(stateThreadWa, 512);
THD_FUNCTION(stateThread, arg)
{
	(void) arg;
	chRegSetThreadName ("state_control");

	systime_t timePrevious = chVTGetSystemTime ();
	systime_t timeoutBuzzer = timePrevious;
	systime_t timeoutHv = timePrevious;

	while (true)
	{
		systime_t timeCurrent = chVTGetSystemTime ();

		if (vehicleState == VEHICLE_STATE_LOW_VOLTAGE)
		{
			// Transition LV to HV
			if (bms.tractiveSystemsActive)
				vehicleState = VEHICLE_STATE_HIGH_VOLTAGE;
		}
		else if (vehicleState == VEHICLE_STATE_HIGH_VOLTAGE)
		{
			// Transition HV to RTD
			if (pedals.isBraking && !pedals.isAccelerating && !palReadLine (LINE_START_BUTTON_IN))
			{
				vehicleState = VEHICLE_STATE_READY_TO_DRIVE;
				palSetLine (LINE_BUZZER);
				timeoutBuzzer = chTimeAddX (timeCurrent, BUZZER_TIME_PERIOD);
			}
		}
		else if (vehicleState == VEHICLE_STATE_READY_TO_DRIVE)
		{
			// Transition RTD to LV if the deadline expires (postpone if HV is active)
			if (bms.tractiveSystemsActive)
				timeoutHv = chTimeAddX (timeCurrent, HV_INACTIVE_PERIOD);
			else if (timeCurrent >= timeoutHv)
				vehicleState = VEHICLE_STATE_LOW_VOLTAGE;
		}

		// Stop the RTD buzzer if it is past the deadline.
		if (timeCurrent >= timeoutBuzzer)
			palClearLine (LINE_BUZZER);
		
		// Sleep until the next loop
		systime_t timeNext = chTimeAddX (timePrevious, STATE_CONTROL_PERIOD);
		timePrevious = chThdSleepUntilWindowed (timePrevious, timeNext);
	}
}

// Functions ------------------------------------------------------------------------------------------------------------------

void stateThreadStart (tprio_t priority)
{
	// Start the torque control thread
	chThdCreateStatic (&stateThreadWa, sizeof (stateThreadWa), priority, stateThread, NULL);
}