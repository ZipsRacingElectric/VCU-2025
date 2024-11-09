// Header
#include "pedals.h"

bool pedalSensorInit (pedalSensor_t* sensor, pedalSensorConfig_t* config)
{
	// Validate the configuration
	sensor->configValid = config->rawMin < config->rawMax;

	// Store the configuration
	sensor->rawMin = config->rawMin;
	sensor->rawMax = config->rawMax;

	// Set values to their defaults
	sensor->valueValid = false;
	sensor->value = 0.0f;

	return sensor->configValid;
}

void pedalSensorCallback (void* handler, uint16_t raw)
{
	pedalSensor_t* sensor = (pedalSensor_t*) handler;

	// Config check
	if (!sensor->configValid)
	{
		sensor->valueValid = false;
		return;
	}

	// Range check
	sensor->valueValid = raw < sensor->rawMin || raw > sensor->rawMax;

	// Linear interpolation
	sensor->value = (float) (raw - sensor->rawMin) / (sensor->rawMax - sensor->rawMin);
}

bool pedalsInit (pedals_t* pedals, pedalsConfig_t* config)
{
	// Configure and validate the individual sensors
	bool result = pedalSensorInit (&pedals->apps1, &config->apps1Config);
	result &= pedalSensorInit (&pedals->apps2, &config->apps2Config);
	result &= pedalSensorInit (&pedals->bseF, &config->bseFConfig);
	result &= pedalSensorInit (&pedals->bseR, &config->bseRConfig);
	return result;
}

void pedalsUpdate (pedals_t* pedals)
{
	// TODO(Barach): Validation / state
	pedals->appsRequest	= (pedals->apps1.value + pedals->apps2.value) / 2.0;
	pedals->bseRequest = (pedals->bseF.value + pedals->bseR.value) / 2.0;

	pedals->valid = pedals->apps1.valueValid && pedals->apps2.valueValid &&
		pedals->bseF.valueValid && pedals->bseR.valueValid;
}