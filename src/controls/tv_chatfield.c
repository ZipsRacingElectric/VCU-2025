// Header
#include "tv_chatfield.h"

tvOutput_t tvChatfield (float deltaTime)
{
	// TODO(Barach): Implementation.
	(void) deltaTime;

	tvOutput_t output =
	{
		.valid = false,
		.frontLeftTorqueRequest = 0,
		.frontRightTorqueRequest = 0,
		.rearRightTorqueRequest = 0,
		.rearLeftTorqueRequest = 0
	};

	return output;
}