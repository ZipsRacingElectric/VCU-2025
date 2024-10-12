#ifndef TORQUE_VECTORING_H
#define TORQUE_VECTORING_H

// Includes -------------------------------------------------------------------------------------------------------------------

// C Standard Library
#include <stdbool.h>

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The maximum requestable torque value, in Nm.
#define TORQUE_REQUEST_MAX 21.0f

// Global Data ----------------------------------------------------------------------------------------------------------------

/// @brief The configued torque limit, in Nm. This value is adjusted by the driver.
extern float torqueRequestLimit;

// Datatypes ------------------------------------------------------------------------------------------------------------------

struct tvOutput
{
	bool valid;
	float frontLeftTorqueRequest;
	float frontRightTorqueRequest;
	float rearLeftTorqueRequest;
	float rearRightTorqueRequest;
};

typedef struct tvOutput tvOutput_t;

typedef tvOutput_t (tvFunction_t) (float deltaTime);

// Functions ------------------------------------------------------------------------------------------------------------------

void torqueVectoringInit (void);

#endif // TORQUE_VECTORING_H