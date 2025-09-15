// Header
#include "tv_linear_bias.h"

// Includes
#include "peripherals.h"
#include "controls/lerp.h"
#include "can.h"

tvOutput_t tvLinearBias (const tvInput_t* input, const void* configPointer)
{
	const tvLinearBiasConfig_t* config = configPointer;

	// Lerp from beginning motor speed & bias to end motor speed & bias.
	float motorSpeed = (amkRl.actualSpeed + amkRr.actualSpeed) / 2.0f;
	float biasRear = lerp2dSaturated (motorSpeed,
		config->motorSpeedBiasBegin, config->frontRearBiasBegin,
		config->motorSpeedBiasEnd, config->frontRearBiasEnd);
	float biasFront = 1.0f - biasRear;

	// Lerp from beginning angle & 50% bias to end angle & bias (for both positive and negative).
	float steeringAngle = sas.value;
	float biasLeft;
	if (steeringAngle >= 0.0f)
		biasLeft = lerp2dSaturated (sas.value,
			config->steeringAngleBiasBegin, 0.5f,
			config->steeringAngleBiasEnd, config->leftRightBiasEnd);
	else
		biasLeft = lerp2dSaturated (-sas.value,
			config->steeringAngleBiasBegin, 0.5f,
			config->steeringAngleBiasEnd, 1.0f - config->leftRightBiasEnd);
	float biasRight = 1.0f - biasLeft;

	tvOutput_t output =
	{
		.valid = sas.state == ANALOG_SENSOR_VALID,
		.torqueRl = biasRear * biasLeft * input->drivingTorqueLimit,
		.torqueRr = biasRear * biasRight * input->drivingTorqueLimit,
		.torqueFl = biasFront * biasLeft * input->drivingTorqueLimit,
		.torqueFr = biasFront * biasRight * input->drivingTorqueLimit
	};
	return output;
}