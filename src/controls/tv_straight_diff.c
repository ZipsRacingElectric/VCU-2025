// Header
#include "tv_straight_diff.h"

// Includes
#include "peripherals.h"

tvOutput_t tvStraightDiff (const tvInput_t* input)
{
	float torqueRear	= input->drivingTorqueLimit / 2.0f * (1 - eepromMap->drivingTorqueBias);
	float torqueFront	= input->drivingTorqueLimit / 2.0f * eepromMap->drivingTorqueBias;

	tvOutput_t output =
	{
		.valid = true,
		.torqueRl = torqueRear,
		.torqueRr = torqueRear,
		.torqueFl = torqueFront,
		.torqueFr = torqueFront
	};
	return output;
}