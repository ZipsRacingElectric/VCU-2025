#ifndef STATE_THREAD_H
#define STATE_THREAD_H

// State Thread ---------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.16
//
// Description: TODO(Barach)
//
// TODO(Barach):
// - Header
// - Time overflow

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "ch.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef enum
{
	/// @brief Indicates a critical error has occured, the car cannot drive until this is cleared.
	VEHICLE_STATE_FAILED = 0,

	/// @brief Indicates all GLV systems are nominal, the car is ready to be energized.
	VEHICLE_STATE_LOW_VOLTAGE = 1,

	/// @brief Indicates the car is energized, ready to enter RTD.
	VEHICLE_STATE_HIGH_VOLTAGE = 2,

	/// @brief Indicates the car is energized and driving, motors respond to pedal input (if torque plausible).
	VEHICLE_STATE_READY_TO_DRIVE = 3
} vehicleState_t;

// Global Data ----------------------------------------------------------------------------------------------------------------

/// @brief The global state of the vehicle.
extern vehicleState_t vehicleState;

/// @brief Indicates the state of the torque thread.
extern bool torquePlausible;

/// @brief Indicates whether the output torque is being derated for any reason.
extern bool torqueDerating;

/// @brief Indicates the state of the CAN thread.
extern bool canPlausible;

// Functions ------------------------------------------------------------------------------------------------------------------

void stateThreadStart (tprio_t priority);

void stateThreadSetTorquePlausibility (bool plausible, bool derating);

#endif // STATE_THREAD_H