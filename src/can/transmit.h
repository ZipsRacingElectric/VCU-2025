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
 * @brief Transmits the VCU sensor input raw message, given the current sensor inputs.
 * @param driver The CAN driver to use.
 * @param timeout The interval to timeout after.
 * @return The result of the CAN operation.
 */
msg_t transmitSensorInputRaw (CANDriver* driver, sysinterval_t timeout);

/**
 * @brief Transmits the VCU debug message, given the specified values.
 * @param driver The CAN driver to use.
 * @param timeout The interval to timeout after.
 * @return The result of the CAN operation.
 */
msg_t transmitDebugMessage (CANDriver* driver, float value0, float value1, float value2, float value3, sysinterval_t timeout);

#endif // TRANSMIT_H