#ifndef TORQUE_THREAD_H
#define TORQUE_THREAD_H

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "ch.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from the torque thread.
#define TORQUE_THREAD_DEBUGGING 1

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Starts the torque control thread.
 * @param priority The priority to start the thread at.
 */
void torqueThreadStart (tprio_t priority);

#endif // TORQUE_THREAD_H