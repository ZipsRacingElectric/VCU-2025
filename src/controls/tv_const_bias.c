// Header
#include "tv_const_bias.h"

tvOutput_t tvConstBias (const tvInput_t* input, const void* configPointer)
{
	const tvConstBiasConfig_t* config = configPointer;

	float biasRear = config->frontRearBias;
	float biasFront = 1 - biasRear;
	float biasLeft = config->leftRightBias;
	float biasRight	= 1 - biasLeft;

	tvOutput_t output =
	{
		.valid = true,
		.torqueRl = input->drivingTorqueLimit * biasRear * biasLeft,
		.torqueRr = input->drivingTorqueLimit * biasRear * biasRight,
		.torqueFl = input->drivingTorqueLimit * biasFront * biasLeft,
		.torqueFr = input->drivingTorqueLimit * biasFront * biasRight
	};
	return output;
}