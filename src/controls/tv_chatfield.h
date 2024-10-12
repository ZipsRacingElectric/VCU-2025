#ifndef TV_CHATFIELD_H
#define TV_CHATFIELD_H

// Chris Chatfield's Torque Vectoring Algorithm -------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.12
//
// Description: Chris Chatfield's torque vectoring model, as described in his masters thesis "Analysis of Torque Vectoring
//   Systems Through Tire and Vehicle Model Simulation".
//
// Sensor Requirements:
// - Pedals
// - Steering Angle

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "torque_vectoring.h"

// Functions ------------------------------------------------------------------------------------------------------------------

tvOutput_t tvChatfield (float deltaTime);

#endif // TV_CHATFIELD_H