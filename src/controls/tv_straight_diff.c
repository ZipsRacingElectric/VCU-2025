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
		.torqueFl = pedals.appsRequest * torqueRequestLimit,
		.torqueFr = pedals.appsRequest * torqueRequestLimit,
		.torqueRl = pedals.appsRequest * torqueRequestLimit,
		.torqueRr = pedals.appsRequest * torqueRequestLimit
	};
	return out;
}