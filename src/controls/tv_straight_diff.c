// Header
#include "tv_straight_diff.h"

// Includes
#include "peripherals.h"

tvOutput_t tvStraightDiff (const tvInput_t* input)
{
	tvOutput_t output =
	{
		.valid = true,
		.torqueRl = pedals.appsRequest * input->torqueLimit / 2.0f * input->torqueBias,
		.torqueRr = pedals.appsRequest * input->torqueLimit / 2.0f * input->torqueBias,
		.torqueFl = pedals.appsRequest * input->torqueLimit / 2.0f * (1 - input->torqueBias),
		.torqueFr = pedals.appsRequest * input->torqueLimit / 2.0f * (1 - input->torqueBias)
	};
	return output;
}