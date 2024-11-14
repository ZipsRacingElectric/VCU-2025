#ifndef TORQUE_VECTORING_H
#define TORQUE_VECTORING_H

// Includes -------------------------------------------------------------------------------------------------------------------

// C Standard Library
#include <stdbool.h>

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The maximum requestable torque value, in Nm.
#define TORQUE_REQUEST_MAX 21.0f

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	bool valid;
	float torqueFl;
	float torqueFr;
	float torqueRl;
	float torqueRr;
} tvOutput_t;

typedef tvOutput_t (tvFunction_t) (float deltaTime, float torqueLimit);

#endif // TORQUE_VECTORING_H