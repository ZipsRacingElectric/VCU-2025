#ifndef TORQUE_THREAD_H
#define TORQUE_THREAD_H

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "controls/torque_vectoring.h"

// ChibiOS
#include "ch.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from the torque thread.
#define TORQUE_THREAD_DEBUGGING 1

// Constants ------------------------------------------------------------------------------------------------------------------

// TODO(Barach): Better distinction between the cumulative torque limit and the individual torque limit.
/// @brief The maximum value of the global torque limit.
#define TORQUE_LIMIT_MAX TV_MOTOR_TORQUE_MAX * 4

/// @brief The maximum value of the global regen limit.
#define REGEN_LIMIT_MAX TV_MOTOR_REGEN_MAX * 4

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Starts the torque control thread.
 * @param priority The priority to start the thread at.
 */
void torqueThreadStart (tprio_t priority);

/**
 * @brief Sets the global torque limit.
 * @param torque The torque limit to set, in Nm. Must be less than @c TORQUE_LIMIT_MAX .
 */
void torqueThreadSetTorqueLimit (float torque);

/**
 * @brief Sets the global torque front/rear bias.
 * @param bias The bias to set, must be in range [0, 1].
 */
void torqueThreadSetTorqueBias (float bias);

/**
 * @brief Sets the global regen limit.
 * @param regen The regen limit to set, in Nm. Must be less than @c REGEN_LIMIT_MAX
 */
void torqueThreadSetRegenLimit (float regen);

/**
 * @brief Sets the global regen front/rear bias/
 * @param bias The bias to set, must be in range [0, 1].
 */
void torqueThreadSetRegenBias (float bias);

/**
 * @brief Selects a torque-vectoring algorithm to use.
 * @param index The index of the algorithm to use.
 */
void torqueThreadSelectAlgorithm (uint8_t index);

#endif // TORQUE_THREAD_H