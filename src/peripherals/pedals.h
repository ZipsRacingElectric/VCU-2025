#ifndef PEDALS_H
#define PEDALS_H

// Includes
#include "peripherals/linear_sensor.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	linearSensorConfig_t apps1Config;
	linearSensorConfig_t apps2Config;
	linearSensorConfig_t bseFConfig;
	linearSensorConfig_t bseRConfig;
} pedalsConfig_t;

/**
 * @brief Structure representing the pedals of the vehicle.
 */
typedef struct
{
	linearSensor_t apps1;
	linearSensor_t apps2;
	linearSensor_t bseF;
	linearSensor_t bseR;

	bool plausible;
	bool apps10PercentPlausible;

	bool isAccelerating;
	bool isBraking;

	float appsRequest;
	float bseRequest;

} pedals_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool pedalsInit (pedals_t* pedals, pedalsConfig_t* config);

void pedalsUpdate (pedals_t* pedals, systime_t timeCurrent);

#endif // PEDALS_H