#ifndef TV_STRAIGHT_DIFF_H
#define TV_STRAIGHT_DIFF_H

// Straight Differential Torque Vectoring Algorithm ---------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.12
//
// Description: Bare minimum torque vectoring algorithm. This is only intended as a fallback for when critical sensors are not
//   working, please don't run this unless you are desperate.
//
// Sensor Requirements:
// - Pedals

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "torque_vectoring.h"

// Functions ------------------------------------------------------------------------------------------------------------------

tvOutput_t tvStraightDiff (float deltaTime);

#endif // TV_STRAIGHT_DIFF_H