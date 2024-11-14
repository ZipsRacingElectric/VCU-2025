// Header
#include "tv_chatfield.h"

tvOutput_t tvChatfield (float deltaTime, float torqueLimit)
{
	// TODO(Barach): Implementation.
	(void) deltaTime;
	(void) torqueLimit;

	tvOutput_t output =
	{
		.valid = false,
		.torqueFl = 0,
		.torqueFr = 0,
		.torqueRl = 0,
		.torqueRr = 0
	};

	return output;
}