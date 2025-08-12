#ifndef TRANSMIT_H
#define TRANSMIT_H

// VCU CAN Transmitters -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.21
//
// Description: Functions for transmitting CAN messages that aren't directed towards a specific CAN node.

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "hal.h"

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Transmits the VCU status message, given its current state.
 * @param driver The CAN driver to use.
 * @param timeout The interval to timeout after.
 * @return The result of the CAN operation.
 */
msg_t transmitStatusMessage (CANDriver* driver, sysinterval_t timeout);

/**
 * @brief Transmits the VCU sensor input percent message, given the current sensor inputs.
 * @param driver The CAN driver to use.
 * @param timeout The interval to timeout after.
 * @return The result of the CAN operation.
 */
msg_t transmitSensorInputPercent (CANDriver* driver, sysinterval_t timeout);

/**
 * @brief Transmits the VCU debug message, given the currently selected values.
 * @param driver The CAN driver to use.
 * @param timeout The interval to timeout after.
 * @return The result of the CAN operation.
 */
msg_t transmitDebugMessage (CANDriver* driver, sysinterval_t timeout);

/**
 * @brief Transmits the vehicle configuration message.
 * @param driver The CAN driver to use.
 * @param timeout The interval to timeout after.
 * @return The result of the CAN operation.
 */
msg_t transmitConfigMessage (CANDriver* driver, sysinterval_t timeout);

#endif // TRANSMIT_H