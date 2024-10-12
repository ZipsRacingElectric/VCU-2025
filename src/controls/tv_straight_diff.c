// Header
#include "tv_straight_diff.h"

// Includes
#include "peripherals.h"

tvOutput_t tvStraightDiff (float deltaTime)
{
	(void) deltaTime;
	
	tvOutput_t out = 
	{
		.valid = true,
		.frontRightTorqueRequest = pedals.appsRequest * TORQUE_REQUEST_MAX,
		.frontLeftTorqueRequest = pedals.appsRequest * TORQUE_REQUEST_MAX,
		.rearRightTorqueRequest = pedals.appsRequest * TORQUE_REQUEST_MAX,
		.rearLeftTorqueRequest = pedals.appsRequest * TORQUE_REQUEST_MAX
	};
	return out;
}