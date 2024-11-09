#ifndef STEERING_ANGLE_H
#define STEERING_ANGLE_H

// Steering Angle Sensor (SAS) ------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.24
//
// Description: Object and functions related to the steering angle sensor.

// Includes -------------------------------------------------------------------------------------------------------------------

// C Standard Library
#include <stdbool.h>
#include <stdint.h>

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	uint16_t rawMin;
	uint16_t rawMax;
	float range;
} sasConfig_t;

typedef struct
{
	uint16_t rawMax;
	uint16_t rawMin;
	float range;
	float value;
	bool valueValid;
	bool configValid;
} sas_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool sasInit (sas_t* sas, sasConfig_t* config);

void sasCallback (void* handler, uint16_t raw);

#endif // STEERING_ANGLE_H