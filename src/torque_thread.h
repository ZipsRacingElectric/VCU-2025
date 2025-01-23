#ifndef TORQUE_THREAD_H
#define TORQUE_THREAD_H

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "ch.h"

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Starts the torque control thread.
 * @param priority The priority to start the thread at.
 */
void torqueThreadStart (tprio_t priority);

/**
 * @brief Selects a torque-vectoring algorithm to use.
 * @param index The index of the algorithm to use.
 */
void torqueThreadSelectAlgorithm (uint8_t index);

/**
 * @brief Sets the cumulative driving (positive) torque limit.
 * @param torque The limit to set, in Nm.
 */
void torqueThreadSetDrivingTorqueLimit (float torque);

/**
 * @brief Sets the cumulative regenerative (negative) torque limit.
 * @param torque The limit to set, in Nm.
 */
void torqueThreadSetRegenTorqueLimit (float torque);

/**
 * @brief Sets the cumulative power limit of the torque thread.
 * @param powerLimit The limit to set, in Watts.
 */
void torqueThreadSetPowerLimit (float powerLimit);

/**
 * @brief Configures the PID coefficients of the power limit PID controller.
 * @param kp The proportional coefficient to set, in 1/W
 * @param ki The integral coefficient to set, in 1/Ws
 * @param kd The derivative coefficient to set, in s/W
 * @param a The measurement gain to set, in range [0, 1).
 */
void torqueThreadSetPowerLimitPid (float kp, float ki, float kd, float a);

#endif // TORQUE_THREAD_H