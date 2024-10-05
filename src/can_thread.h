#ifndef CAN_THREAD_H
#define CAN_THREAD_H

// CAN Thread -----------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.09.29
// 
// To do:
// - Implement timeout for CAN nodes.
// - A lot of CAN stuff can be constant.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can/ecumaster_gps_v2.h"
#include "can/misc_node.h"

// ChibiOS
#include "ch.h"

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from the CAN thread.
#define CAN_THREAD_DEBUGGING 1

/// @brief Timeout period for message transmission.
#define CAN_THREAD_TX_TIMEOUT TIME_MS2I(500)

// Global Nodes ---------------------------------------------------------------------------------------------------------------

extern ecumasterGps_t	gps;
extern misc_t			misc;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Creates and starts the CAN interface thread.
 * @param priority The priority of the thread.
 */
void canThreadStart (tprio_t priority);

#endif // CAN_THREAD_H