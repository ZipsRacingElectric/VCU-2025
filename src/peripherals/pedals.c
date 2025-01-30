// Header
#include "pedals.h"

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The maximum acceptable delta of the APPS-1 and APPS-2 sensor values.
#define APPS_DELTA_MAX 0.1

#define APPS_ACCELERATING_VALUE 0.1
#define BSE_BRAKING_VALUE		0.1

// Functions ------------------------------------------------------------------------------------------------------------------

bool pedalsInit (pedals_t* pedals, pedalsConfig_t* config)
{
	// Configure and validate the individual sensors
	bool result = linearSensorInit (&pedals->apps1, &config->apps1Config);
	result &= linearSensorInit (&pedals->apps2, &config->apps2Config);
	result &= linearSensorInit (&pedals->bseF, &config->bseFConfig);
	result &= linearSensorInit (&pedals->bseR, &config->bseRConfig);
	return result;
}

void pedalsUpdate (pedals_t* pedals, systime_t timeCurrent)
{
	// TODO(Barach): Deadlines
	(void) timeCurrent;

	// Calculate the average of both sensors.
	pedals->appsRequest	= (pedals->apps1.value + pedals->apps2.value) / 2.0;
	pedals->bseRequest = (pedals->bseF.value + pedals->bseR.value) / 2.0;

	pedals->isAccelerating	= pedals->appsRequest > APPS_ACCELERATING_VALUE;
	pedals->isBraking		= pedals->bseRequest > BSE_BRAKING_VALUE;

	// APPS 10% check (TODO(Barach): Rule no.)
	float appsDelta = pedals->apps1.value - pedals->apps2.value;
	pedals->apps10PercentPlausible = appsDelta <= APPS_DELTA_MAX && appsDelta >= -APPS_DELTA_MAX;

	// TODO(Barach): APPS 25/5

	// TODO(Barach): 100ms timeout
	pedals->plausible =
		pedals->apps1.state == LINEAR_SENSOR_VALID &&
		pedals->apps2.state == LINEAR_SENSOR_VALID &&
		pedals->bseF.state == LINEAR_SENSOR_VALID &&
		pedals->bseR.state == LINEAR_SENSOR_VALID &&
		pedals->apps10PercentPlausible;
}