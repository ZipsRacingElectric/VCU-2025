// Header
#include "steering_angle.h"

bool sasInit (sas_t* sas, sasConfig_t* config)
{
	// Validate the configuration
	sas->configValid = config->rawMin < config->rawMax;
	
	// Store the configuration
	sas->rawMin = config->rawMin;
	sas->rawMax = config->rawMax;
	
	// Set values to their defaults
	sas->valueValid = false;
	sas->value = 0.0f;

	return sas->configValid;
}

void sasCallback (void* handler, uint16_t raw)
{
	sas_t* sas = (sas_t*) handler;

	// Config check
	if (!sas->configValid)
	{
		sas->valueValid = false;
		return;
	}

	// Range check
	sas->valueValid = raw < sas->rawMin || raw > sas->rawMax;

	// Linear interpolation
	sas->value = (float) (raw - sas->rawMin) / (sas->rawMax - sas->rawMin) * 2.0f * sas->range - sas->range;
}