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
#include "can/bms.h"
#include "can/ecumaster_gps_v2.h"

// ChibiOS
#include "ch.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from the CAN thread.
#define CAN_THREAD_DEBUGGING 0

/// @brief The number of AMK inverters on the CAN bus.
#define AMK_COUNT 4

// Global Nodes ---------------------------------------------------------------------------------------------------------------

#define amkRl (amks [0])
#define amkRr (amks [1])
#define amkFl (amks [2])
#define amkFr (amks [3])
extern amkInverter_t	amks [AMK_COUNT];
extern bms_t			bms;
extern ecumasterGps_t	gps;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Creates and starts the CAN interface thread.
 * @param priority The priority of the thread.
 */
void canThreadStart (tprio_t priority);

#endif // CAN_THREAD_H