#ifndef STATE_THREAD_H
#define STATE_THREAD_H

// State Thread ---------------------------------------------------------------------------------------------------------------
//
// Description: ???
// Date Created: ???
//
// To do:
// - Header
// - Time overflow

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "ch.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

enum vehicleState
{
	VEHICLE_STATE_FAILED			= 0,
	VEHICLE_STATE_LOW_VOLTAGE		= 1,
	VEHICLE_STATE_HIGH_VOLTAGE		= 2,
	VEHICLE_STATE_READY_TO_DRIVE	= 3
};

typedef enum vehicleState vehicleState_t;

// Global Data ----------------------------------------------------------------------------------------------------------------

extern vehicleState_t vehicleState;

extern bool torquePlausible;
extern bool canFault;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Starts the vehicle state management thread.
 * @param priority The priority to start the thread at.
 */
void stateThreadStart (tprio_t priority);

#endif // STATE_THREAD_H