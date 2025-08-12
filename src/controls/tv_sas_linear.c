// Header
#include "tv_sas_linear.h"

// Includes
#include "peripherals.h"
#include "controls/lerp.h"

tvOutput_t tvSasLinear (const tvInput_t* input)
{
	float biasRear = eepromMap->drivingTorqueBias;
	float biasFront = 1 - biasRear;

	float biasRight	= lerp2d (sas.value,
		sas.config->angleNegative, eepromMap->linearSasBiasMax,
		sas.config->anglePositive, 1 - eepromMap->linearSasBiasMax);
	float biasLeft = 1 - biasRight;

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