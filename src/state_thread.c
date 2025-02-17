// Header
#include "state_thread.h"

// Includes
#include "can_thread.h"
#include "can/transmit.h"
#include "peripherals.h"

// ChibiOS
#include "hal.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define STATE_CONTROL_PERIOD	TIME_MS2I (10)
#define STATE_MESSAGE_PRESCALAR	10

#define BUZZER_TIME_PERIOD		TIME_MS2I (3000)
#define HV_INACTIVE_PERIOD		TIME_MS2I (100)

// Global Data ----------------------------------------------------------------------------------------------------------------

vehicleState_t vehicleState	= VEHICLE_STATE_LOW_VOLTAGE;
bool torquePlausible		= true;
bool torqueDerating			= false;
bool canPlausible			= true;

// Thread Entrypoint ----------------------------------------------------------------------------------------------------------

static THD_WORKING_AREA (stateThreadWa, 512);
THD_FUNCTION (stateThread, arg)
{
	(void) arg;
	chRegSetThreadName ("state_control");

	systime_t timePrevious = chVTGetSystemTime ();
	systime_t timeoutBuzzer = timePrevious;
	systime_t timeoutHv = timePrevious;

	uint16_t messageCounter = 0;

	while (true)
	{
		systime_t timeCurrent = chVTGetSystemTime ();

		// If a failure occured previously, transition to LV and attempt to recover.
		if (vehicleState == VEHICLE_STATE_FAILED)
			vehicleState = VEHICLE_STATE_LOW_VOLTAGE;

		// TODO(Barach): Replace once all inverters are running.
		amkInverterState_t amksState = amksGetState (amks, 1);
		if (amksState == AMK_STATE_INVALID || amksState == AMK_STATE_ERROR)
			vehicleState = VEHICLE_STATE_FAILED;

		bool tractiveSystemsActive = amksState == AMK_STATE_READY_HIGH_VOLTAGE || amksState == AMK_STATE_READY_ENERGIZED;

		// Check vehicle state
		if (vehicleState == VEHICLE_STATE_LOW_VOLTAGE)
		{
			// Transition LV to HV
			if (tractiveSystemsActive)
				vehicleState = VEHICLE_STATE_HIGH_VOLTAGE;
		}
		else if (vehicleState == VEHICLE_STATE_HIGH_VOLTAGE)
		{
			// Transition HV to RTD
			if (pedals.braking && !pedals.accelerating && !palReadLine (LINE_START_BUTTON_IN))
			{
				vehicleState = VEHICLE_STATE_READY_TO_DRIVE;
				palSetLine (LINE_BUZZER);
				timeoutBuzzer = chTimeAddX (timeCurrent, BUZZER_TIME_PERIOD);
			}

			// Check TS are still active
			if (tractiveSystemsActive)
			{
				// Postpone the deadline if TS are active.
				timeoutHv = chTimeAddX (timeCurrent, HV_INACTIVE_PERIOD);
			}
			else if (chTimeIsInRangeX (timeCurrent, timePrevious, timeoutHv))
			{
				// Transition HV to LV if the deadline expires.
				vehicleState = VEHICLE_STATE_LOW_VOLTAGE;
			}
		}
		else if (vehicleState == VEHICLE_STATE_READY_TO_DRIVE)
		{
			// Check TS are still active
			if (tractiveSystemsActive)
			{
				// Postpone the deadline if TS are active.
				timeoutHv = chTimeAddX (timeCurrent, HV_INACTIVE_PERIOD);
			}
			else if (!chTimeIsInRangeX (timeCurrent, timePrevious, timeoutHv))
			{
				// Transition RTD to LV if the deadline expires.
				vehicleState = VEHICLE_STATE_LOW_VOLTAGE;
			}
		}

		// Stop the RTD buzzer if it is past the deadline.
		if (!chTimeIsInRangeX (timeCurrent, timePrevious, timeoutBuzzer))
			palClearLine (LINE_BUZZER);

		++messageCounter;
		if (messageCounter >= STATE_MESSAGE_PRESCALAR)
		{
			transmitStatusMessage (&CAND1, STATE_CONTROL_PERIOD);
			messageCounter = 0;
		}

		palWriteLine (LINE_VCU_FLT, !torquePlausible || vehicleState == VEHICLE_STATE_FAILED);

		// Brake light
		palWriteLine (LINE_BRK_LIGHT, pedals.braking);

		// Broadcast the sensor input message and debug message
		transmitSensorInputPercent (&CAND1, STATE_CONTROL_PERIOD);
		transmitDebugMessage (&CAND1, STATE_CONTROL_PERIOD);

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

void stateThreadSetTorquePlausibility (bool plausible, bool derating)
{
	// If implausible, set the fault indicator now.
	if (!plausible)
		palWriteLine (LINE_VCU_FLT, true);

	torquePlausible = plausible;
	torqueDerating = derating;
}

void canFaultCallback (msg_t result)
{
	(void) result;
	canPlausible = false;
}