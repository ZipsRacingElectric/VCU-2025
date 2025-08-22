#ifndef CAN_H
#define CAN_H

// CAN Thread -----------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.09.29
//
// Description: Objects related the the VCU's CAN interface. The VCU uses 2 CAN busses. The first CAN bus is the vehicle's
//   main bus. All sensors not directly handled by the VCU are accessed via this bus. The second CAN bus is the inverter bus,
//   containing only the VCU and the 4 inverters. The VCU acts as a bridge between these busses, where all CAN messages
//   received on the inverter bus are re-transmitted on the main bus. The bridging is done for debugging and data-logging
//   purposes. The separation of the two busses is to avoid CAN errors from the inverters.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can/amk_inverter.h"
#include "can/bms.h"
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

/// @brief The accumulator's battery management system.
extern bms_t bms;

/// @brief The ECUMaster GPS/IMU.
extern ecumasterGps_t gps;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes both of the VCU's CAN interfaces.
 * @param priority The priority to start the CAN threads at.
 * @return False if a fatal error occurred, true otherwise.
 */
bool canInterfaceInit (tprio_t priority);

#endif // CAN_H