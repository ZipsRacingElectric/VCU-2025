#ifndef TV_CHATFIELD_H
#define TV_CHATFIELD_H

// Chris Chatfield's Torque Vectoring Algorithm -------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.12
//
// Description: Chris Chatfield's torque vectoring model, as described in his master's thesis "Analysis of Torque Vectoring
//   Systems Through Tire and Vehicle Model Simulation".
//
// Sensor Requirements:
// - Pedals
// - Steering Angle

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "torque_vectoring.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define TV_CHATFIELD_THROTTLE_RANGE			100
#define TV_CHATFIELD_THROTTLE_RESOLUTION	5

#define TV_CHATFIELD_ANGLE_RANGE			15
#define TV_CHATFIELD_ANGLE_RESOLUTION		1

#define TV_CHATFIELD_LUT_THROTTLE_WIDTH		((TV_CHATFIELD_THROTTLE_RANGE / TV_CHATFIELD_THROTTLE_RESOLUTION + 1))
#define TV_CHATFIELD_LUT_ANGLE_WIDTH		(((2 * TV_CHATFIELD_ANGLE_RANGE) / TV_CHATFIELD_ANGLE_RESOLUTION + 1))
#define TV_CHATFIELD_LUT_SIZE				(TV_CHATFIELD_LUT_THROTTLE_WIDTH * TV_CHATFIELD_LUT_ANGLE_WIDTH)

// Functions ------------------------------------------------------------------------------------------------------------------

void tvChatfieldInit (void);

/// @brief Entrypoint to the chatfield torque vectoring algorithm.
tvOutput_t tvChatfield (const tvInput_t* input);

#endif // TV_CHATFIELD_H