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
// - Handler / transmit implementations.
// - Transmit should expect a node reference, rather than ID.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define AMK_INVERTER_HANDLER_COUNT 0

// Datatypes ------------------------------------------------------------------------------------------------------------------

struct amkInverter
{
	CAN_NODE_FIELDS;
	uint16_t baseId;
};

typedef struct amkInverter amkInverter_t;

// Initialization -------------------------------------------------------------------------------------------------------------

void amkInit (amkInverter_t* amk, uint16_t baseId, CANDriver* driver);

// Transmit -------------------------------------------------------------------------------------------------------------------

/**
 * @brief Sends the specified torque request to an AMK inverter.
 * @param amk The AMK inverter to send the message to.
 * @param torque1 The torque to request from motor 1.
 * @param torque2 The torque to request from motor 2.
 * @param timeout The interval to timeout after.
 * @return The result of the operation.
 */
msg_t amkSendTorqueRequest (amkInverter_t* amk, float torque1, float torque2, sysinterval_t timeout);

#endif // AMK_INVERTER_H