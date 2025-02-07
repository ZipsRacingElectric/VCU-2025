// Header
#include "steering_angle.h"

// Includes
#include "controls/lerp.h"

#define ZERO_SAMPLE 2048

bool sasInit (sas_t* sas, sasConfig_t* config)
{
	// Store the configuration
	sas->config = config;

	// Validate the configuration
	if (config->sampleNegative >= ZERO_SAMPLE || ZERO_SAMPLE >= config->samplePositive)
		sas->state = LINEAR_SENSOR_CONFIG_INVALID;
	else
		sas->state = LINEAR_SENSOR_VALUE_INVALID;

	// Set values to their defaults
	sas->value = 0.0f;

	return sas->state != LINEAR_SENSOR_CONFIG_INVALID;
}

void sasUpdate (void* object, adcsample_t sample)
{
	sas_t* sas = (sas_t*) object;

	// Apply the sample offset and 4096 modulus
	sample += sas->config->sampleOffset;
	if (sample >= 4096)
		sample -= 4096;

	// Store the sample
	sas->sample = sample;

	// If the config is invalid, don't check anything else.
	if (sas->state == LINEAR_SENSOR_CONFIG_INVALID)
		return;

	// Check the sample is in the valid range
	if (sample < sas->config->sampleNegative || sample > sas->config->samplePositive)
	{
		sas->state = LINEAR_SENSOR_VALUE_INVALID;
		sas->value = 0;
	}

	sas->state = LINEAR_SENSOR_VALID;

	if (sample > ZERO_SAMPLE)
	{
		// Map zero sample to zero angle, positive sample to positive angle.
		sas->value = lerp2d (sample, ZERO_SAMPLE, 0, sas->config->samplePositive, sas->config->anglePositive);

		// Deadzone check
		if (sas->value <= sas->config->angleDeadzone / 2.0f)
			sas->value = 0.0f;
	}
	else
	{
		// Map negative sample to negative angle, zero sample to zero angle.
		sas->value = lerp2d (sample, sas->config->sampleNegative, sas->config->angleNegative, ZERO_SAMPLE, 0);

		// Deadzone check
		if (sas->value >= -sas->config->angleDeadzone / 2.0f)
			sas->value = 0.0f;
	}
}