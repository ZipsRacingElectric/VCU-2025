#ifndef TORQUE_VECTORING_H
#define TORQUE_VECTORING_H

// Torque Vectoring -----------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.12
//
// Description: Datatypes and constants common to all torque vectoring algorithms.

// Includes -------------------------------------------------------------------------------------------------------------------

// C Standard Library
#include <stdbool.h>

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The maximum torque request for a single motor.
#define TV_MOTOR_TORQUE_MAX 21.0f

/// @brief The maximum regen request for a single motor.
#define TV_MOTOR_REGEN_MAX 9.8f

// Datatypes ------------------------------------------------------------------------------------------------------------------

/// @brief Input to TV algorithms.
typedef struct
{
	/// @brief The amount of time ellapsed between now and the previous call to the algorithm's update function, in seconds.
	float deltaTime;

	/// @brief The global torque limit, in Nm. The positive torque requested to a single motor should not exceed this.
	float torqueLimit;

	/// @brief The front/rear torque bias. While not required, this should loosely affect the front to rear torque
	/// distribution. 1 => full rear bias, 0 => full front bias, 0.5 => balanced bias.
	float torqueBias;

	/// @brief The global regen limit, in Nm. The magnitude of the negative torque requested to a single motor should not
	/// exceed this.
	float regenLimit;

	/// @brief The front/rear regen bias. While not required, this should loosely affect the front to rear regen torque
	/// distribution. 1 => full rear bias, 0 => full front bias, 0.5 => balanced bias.
	float regenBias;
} tvInput_t;

/// @brief Output to TV algoritms.
typedef struct
{
	/// @brief Output request validity. Indicates whether the the output of this algoritm is usable.
	bool valid;

	/// @brief The torque to request to the rear-left motor. Positive means actual torque, negative means regen torque.
	float torqueRl;

	/// @brief The torque to request to the rear-right motor. Positive means actual torque, negative means regen torque.
	float torqueRr;

	/// @brief The torque to request to the front-left motor. Positive means actual torque, negative means regen torque.
	float torqueFl;

	/// @brief The torque to request to the front-right motor. Positive means actual torque, negative means regen torque.
	float torqueFr;
} tvOutput_t;

typedef tvOutput_t (tvFunction_t) (const tvInput_t* input);

#endif // TORQUE_VECTORING_H