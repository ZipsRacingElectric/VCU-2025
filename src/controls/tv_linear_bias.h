#ifndef TV_SAS_LINEAR_H
#define TV_SAS_LINEAR_H

// Linear Steering Angle Sensor Torque Vectoring Algorithm --------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.07.12
//
// Description: A torque vectoring algorithm where the left-to-right bias is a linear function of the vehicle's steering angle.
//   This algorithm is a simple open-loop controller, meant to roughly emulate the behavior of a differential without the
//   complexity of negative feedback.
//
// Sensor Requirements:
//   - Steering-angle-sensor
//   - Wheel speed

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "torque_vectoring.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	// TODO(Barach): Docs
	float wheelSpeedBiasBegin;
	float wheelSpeedBiasEnd;
	float frontRearBiasBegin;
	float frontRearBiasEnd;

	float steeringAngleBiasBegin;
	float steeringAngleBiasEnd;
	float leftRightBiasEnd;
} tvLinearBiasConfig_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/// @brief Entrypoint to the torque vectoring algorithm.
tvOutput_t tvLinearBias (const tvInput_t* input, const void* configPointer);

#endif // TV_SAS_LINEAR_H