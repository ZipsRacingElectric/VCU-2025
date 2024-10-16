#ifndef APPS_H
#define APPS_H

// C Standard Library
#include <stdbool.h>
#include <stdint.h>

// Datatypes ------------------------------------------------------------------------------------------------------------------

struct pedalSensorConfig
{
	uint16_t rawMin;
	uint16_t rawMax;
};

typedef struct pedalSensorConfig pedalSensorConfig_t;

/**
 * @brief Structure representing a pedal sensor (APPS / BSE).
 */
struct pedalSensor
{
	uint16_t rawMin;
	uint16_t rawMax;
	float value;
	bool valueValid;
	bool configValid;
};

typedef struct pedalSensor pedalSensor_t;

struct pedalsConfig
{
	pedalSensorConfig_t apps1Config;
	pedalSensorConfig_t apps2Config;
	pedalSensorConfig_t bseFConfig;
	pedalSensorConfig_t bseRConfig;
};

typedef struct pedalsConfig pedalsConfig_t;

/**
 * @brief Structure representing the pedals of the vehicle.
 */
struct pedals
{
	pedalSensor_t apps1;
	pedalSensor_t apps2;
	pedalSensor_t bseF;
	pedalSensor_t bseR;
	float appsRequest;
	float bseRequest;
	bool isAccelerating;
	bool isBraking;
};

typedef struct pedals pedals_t; 

// Functions ------------------------------------------------------------------------------------------------------------------

bool pedalSensorInit (pedalSensor_t* sensor, pedalSensorConfig_t* config);

void pedalSensorCallback (void* handler, uint16_t raw);

bool pedalsInit (pedals_t* pedals, pedalsConfig_t* config);

void pedalsUpdate (pedals_t* pedals);

#endif // APPS_H