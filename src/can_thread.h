#ifndef CAN_THREAD_H
#define CAN_THREAD_H

// CAN Thread -----------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// 
// To do:
// - Implement timeout for CAN nodes.
// - A lot of CAN stuff can be constant.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can/ecumaster_gps_v2.h"

// ChibiOS
#include "ch.h"

// Debugging ------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from the CAN thread.
#define CAN_THREAD_DEBUGGING 1

// Constants ------------------------------------------------------------------------------------------------------------------

#define CAN_THREAD_TX_TIMEOUT_MS 500

// Global Nodes ---------------------------------------------------------------------------------------------------------------

extern ecumasterGps_t gps;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Creates and starts the CAN interface thread.
 * @param priority The priority of the thread.
 */
void canThreadStart (tprio_t priority);

#endif // CAN_THREAD_H