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

#define BUZZER_TIME_PERIOD		TIME_MS2I (1000)
#define HV_INACTIVE_PERIOD		TIME_MS2I (20)

#define HV_VOLTAGE				60.0f

// Global Data ----------------------------------------------------------------------------------------------------------------

vehicleState_t vehicleState	= VEHICLE_STATE_LOW_VOLTAGE;
bool torquePlausible		= true;
bool canFault				= false;

// Function Prototypes --------------------------------------------------------------------------------------------------------

void stateThreadUpdate (systime_t timePrevious, systime_t timeCurrent, systime_t* timeoutBuzzer, systime_t* timeoutHv);

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

		stateThreadUpdate (timePrevious, timeCurrent, &timeoutBuzzer, &timeoutHv);

		++messageCounter;
		if (messageCounter >= STATE_MESSAGE_PRESCALAR)
		{
			// TODO(Barach): Proper timeout.
			transmitStatusMessage (&CAND1, TIME_MS2I (100));
			messageCounter = 0;
		}

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

void stateThreadUpdate (systime_t timePrevious, systime_t timeCurrent, systime_t* timeoutBuzzer, systime_t* timeoutHv)
{
	// If a failure occured previously, transition to LV and attempt to recover.
	if (vehicleState == VEHICLE_STATE_FAILED)
		vehicleState = VEHICLE_STATE_LOW_VOLTAGE;

	bool tractiveSystemsActive = true;

	// Check RL inverter state.
	canNodeLock ((canNode_t*) &amkRl);
	if (amkRl.state != CAN_NODE_VALID)
		vehicleState = VEHICLE_STATE_FAILED;

	// TODO(Barach): Voltage check
	// tractiveSystemsActive &= amkRl.dcBusVoltage > HV_VOLTAGE;

	canNodeUnlock ((canNode_t*) &amkRl);

	// TODO(Barch): Check other inverters.
	// // Check RR inverter state.
	// canNodeLock ((canNode_t*) &amkRr);
	// if (amkRr.state != CAN_NODE_VALID)
	// 	vehicleState = VEHICLE_STATE_FAILED;

	// tractiveSystemsActive &= amkRr.dcBusVoltage > HV_VOLTAGE;

	// canNodeUnlock ((canNode_t*) &amkRr);

	// // Check FL inverter state.
	// canNodeLock ((canNode_t*) &amkFl);
	// if (amkFl.state != CAN_NODE_VALID)
	// 	vehicleState = VEHICLE_STATE_FAILED;

	// tractiveSystemsActive &= amkFl.dcBusVoltage > HV_VOLTAGE;

	// canNodeUnlock ((canNode_t*) &amkFl);

	// // Check FR inverter state.
	// canNodeLock ((canNode_t*) &amkFr);
	// if (amkFr.state != CAN_NODE_VALID)
	// 	vehicleState = VEHICLE_STATE_FAILED;

	// tractiveSystemsActive &= amkFr.dcBusVoltage > HV_VOLTAGE;

	// canNodeUnlock ((canNode_t*) &amkFr);

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
		if (pedals.isBraking && !pedals.isAccelerating && !palReadLine (LINE_START_BUTTON_IN))
		{
			vehicleState = VEHICLE_STATE_READY_TO_DRIVE;
			palSetLine (LINE_BUZZER);
			*timeoutBuzzer = chTimeAddX (timeCurrent, BUZZER_TIME_PERIOD);
		}

		// Check TS are still active
		if (tractiveSystemsActive)
		{
			// Postpone the deadline if TS are active.
			*timeoutHv = chTimeAddX (timeCurrent, HV_INACTIVE_PERIOD);
		}
		else if (chTimeIsInRangeX (timeCurrent, timePrevious, *timeoutHv))
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
			*timeoutHv = chTimeAddX (timeCurrent, HV_INACTIVE_PERIOD);
		}
		else if (!chTimeIsInRangeX (timeCurrent, timePrevious, *timeoutHv))
		{
			// Transition RTD to LV if the deadline expires.
			vehicleState = VEHICLE_STATE_LOW_VOLTAGE;
		}
	}

	// Stop the RTD buzzer if it is past the deadline.
	if (!chTimeIsInRangeX (timeCurrent, timePrevious, *timeoutBuzzer))
		palClearLine (LINE_BUZZER);
}