#ifndef AMK_INVERTER_H
#define AMK_INVERTER_H

// AMK Inverter ---------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.12
// 
// Description: Object representing the AMK Racing Kit Inverter CAN node.
//
// To do:
// - Node implementations.
// - Transmit implementations.
// - Transmit should expect a node reference, rather than ID.

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "hal.h"

// Transmit -------------------------------------------------------------------------------------------------------------------

msg_t amkSendTorqueRequest (CANDriver* driver, uint16_t id, float motor1Torque, float motor2Torque, sysinterval_t timeout);

#endif // AMK_INVERTER_H