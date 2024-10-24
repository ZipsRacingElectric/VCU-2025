#ifndef RECEIVE_H
#define RECEIVE_H

// CAN Receive Handler --------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.21
//
// Description: Functions for handling CAN messages that don't belong to a CAN node. These messages are not transmitted
//   periodically, but are rather used to configure the VCU.

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "hal.h"

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Handles the received CAN message, if it is intended for the VCU.
 * @param frame The received CAN message.
 * @return True if the message was handled, false otherwise. 
 */
bool receiveMessage (CANRxFrame* frame);

#endif // RECEIVE_H