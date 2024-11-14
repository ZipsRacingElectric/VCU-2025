#ifndef TORQUE_THREAD_H
#define TORQUE_THREAD_H

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "ch.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from the torque thread.
#define TORQUE_THREAD_DEBUGGING 1

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The maximum value of the global torque limit.
#define TORQUE_LIMIT_MAX 21.0f

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
void torqueThreadSetLimit (float torque);

/**
 * @brief Selects a torque-vectoring algorithm to use.
 * @param index The index of the algorithm to use.
 */
void torqueThreadSelectAlgorithm (uint8_t index);

#endif // TORQUE_THREAD_H