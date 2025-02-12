#ifndef PEDALS_H
#define PEDALS_H

// Includes
#include "peripherals/linear_sensor.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	adcsample_t absoluteMin;
	adcsample_t requestMin;
	adcsample_t requestMax;
	adcsample_t absoluteMax;
} pedalSensorConfig_t;

typedef struct
{
	linearSensorState_t		state;
	pedalSensorConfig_t*	config;
	adcsample_t				sample;
	float					value;
} pedalSensor_t;

typedef struct
{
	pedalSensorConfig_t apps1Config;
	pedalSensorConfig_t apps2Config;
	pedalSensorConfig_t bseFConfig;
	pedalSensorConfig_t bseRConfig;
} pedalsConfig_t;

/**
 * @brief Structure representing the pedals of the vehicle.
 */
typedef struct
{
	// TODO(Barach): Docs

	pedalSensor_t apps1;
	pedalSensor_t apps2;
	pedalSensor_t bseF;
	pedalSensor_t bseR;

	/// @brief Indicates the complete validity of the pedal requests (includes 100ms timeout).
	bool plausible;

	/// @brief Indicates the instantaneous plausibility of the pedal requests.
	bool plausibleInst;

	/// @brief Indicates the instantaneous APPS 10% plausibility.
	bool apps10PercentPlausible;

	bool bse10PercentPlausible;

	bool apps25_5Plausible;

	systime_t plausibilityDeadline;

	bool accelerating;
	bool braking;

	float appsRequest;
	float bseRequest;

} pedals_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool pedalSensorInit (pedalSensor_t* sensor, pedalSensorConfig_t* config);

void pedalSensorUpdate (void* object, adcsample_t sample);

bool pedalsInit (pedals_t* pedals, pedalsConfig_t* config);

void pedalsUpdate (pedals_t* pedals, systime_t timePrevious, systime_t timeCurrent);

#endif // PEDALS_H