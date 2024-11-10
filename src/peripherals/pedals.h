#ifndef APPS_H
#define APPS_H

// ChibiOS
#include "hal.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define PEDALS_SENSOR_COUNT 4

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	/// @brief The ADC channel to sample from.
	uint8_t channel;

	/// @brief The minimum raw ADC measurement of the sensor.
	uint16_t rawMin;

	/// @brief The maximum raw ADC measurement of the sensor.
	uint16_t rawMax;

} pedalSensorConfig_t;

/**
 * @brief Structure representing a pedal sensor (APPS / BSE).
 */
typedef struct
{
	uint16_t rawMin;
	uint16_t rawMax;
	float value;
	uint16_t valueRaw;
	bool plausible;
	bool configPlausible;
} pedalSensor_t;

typedef struct
{
	ADCDriver* adc;
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
	pedalsConfig_t* config;
	ADCConversionGroup adcGroup;
	adcsample_t adcBuffer [PEDALS_SENSOR_COUNT];

	pedalSensor_t apps1;
	pedalSensor_t apps2;
	pedalSensor_t bseF;
	pedalSensor_t bseR;

	bool plausible;
	bool apps10PercentPlausible;

	bool isAccelerating;
	bool isBraking;

	float appsRequest;
	float bseRequest;

} pedals_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool pedalsInit (pedals_t* pedals, pedalsConfig_t* config);

void pedalsSample (pedals_t* pedals, systime_t timeCurrent);

#endif // APPS_H