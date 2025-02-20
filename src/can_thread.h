#ifndef CAN_THREAD_H
#define CAN_THREAD_H

// CAN Thread -----------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.09.29
//
// Description: Thread for managing the VCU's CAN interface. All received messages are parsed by this thread.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can/amk_inverter.h"
#include "can/ecumaster_gps_v2.h"

// ChibiOS
#include "ch.h"

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The number of AMK inverters on the CAN bus.
#define AMK_COUNT 4

// Global Nodes ---------------------------------------------------------------------------------------------------------------

/// @brief The rear-left AMK inverter.
#define amkRl (amks [0])

/// @brief The rear-right AMK inverter.
#define amkRr (amks [1])

/// @brief The front-left AMK inverter.
#define amkFl (amks [2])

/// @brief The front-right AMK inverter.
#define amkFr (amks [3])

/// @brief Array of all the AMK inverters.
extern amkInverter_t amks [AMK_COUNT];

/// @brief The ECUMaster GPS/IMU.
extern ecumasterGps_t gps;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Creates and starts the CAN interface thread.
 * @param priority The priority of the thread.
 * @return False if a fatal error occurred, true otherwise.
 */
bool canThreadStart (tprio_t priority);

#endif // CAN_THREAD_H