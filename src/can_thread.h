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

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from the CAN thread.
#define CAN_THREAD_DEBUGGING 0

// Global Nodes ---------------------------------------------------------------------------------------------------------------

extern amkInverter_t	amkRl;
extern amkInverter_t	amkRr;
extern amkInverter_t	amkFl;
extern amkInverter_t	amkFr;
extern bms_t			bms;
extern ecumasterGps_t	gps;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Creates and starts the CAN interface thread.
 * @param priority The priority of the thread.
 */
void canThreadStart (tprio_t priority);

#endif // CAN_THREAD_H