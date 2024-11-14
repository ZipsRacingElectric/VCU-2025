// Header
#include "tv_straight_diff.h"

// Includes
#include "peripherals.h"

tvOutput_t tvStraightDiff (float deltaTime, float torqueLimit)
{
	(void) deltaTime;

	tvOutput_t out =
	{
		.valid = true,
		.torqueFl = pedals.appsRequest * torqueLimit,
		.torqueFr = pedals.appsRequest * torqueLimit,
		.torqueRl = pedals.appsRequest * torqueLimit,
		.torqueRr = pedals.appsRequest * torqueLimit
	};
	return out;
}