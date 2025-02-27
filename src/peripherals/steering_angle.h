#ifndef STEERING_ANGLE_H
#define STEERING_ANGLE_H

// Steering Angle Sensor (SAS) ------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.24
//
// Description: Object and functions related to the steering angle sensor.
//
// TODO(Barach): Integrate this with the AM4096 driver somehow.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/linear_sensor.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	/// @brief The zeroing offset to apply to the sample, should shift the zero position to 2048.
	adcsample_t sampleOffset;
	/// @brief The sample correlating to the negative-most position.
	adcsample_t sampleNegative;
	/// @brief The sample correlating to the positive-most position.
	adcsample_t samplePositive;
	/// @brief The angle of the negative-most position, in radians.
	float angleNegative;
	/// @brief The angle of the positive-most position, in radians.
	float anglePositive;
	/// @brief The range of the deadzone centered at the zero position.
	float angleDeadzone;
} sasConfig_t;

typedef struct
{
	sasConfig_t*		config;
	linearSensorState_t	state;
	float				value;
	adcsample_t			sample;
} sas_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the sensor using the specified configuration.
 * @param sas The sensor to initialize.
 * @param config The configuration to use.
 * @return True if successful, false otherwise.
 */
bool sasInit (sas_t* sas, sasConfig_t* config);

/**
 * @brief Updates the value of the sensor.
 * @note This function uses a @c void* for the object reference as to make the signature usable by callbacks.
 * @param object The sensor to update (must be a @c linearSensor_t* ).
 * @param sample The read sample.
 */
void sasUpdate (void* object, adcsample_t sample);

#endif // STEERING_ANGLE_H