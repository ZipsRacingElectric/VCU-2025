// Header
#include "tv_chatfield.h"

// Includes
#include "peripherals.h"
#include "controls/lerp.h"

// C Standard Library
#include <math.h>

// Global Constants -----------------------------------------------------------------------------------------------------------

float (*lookupTable) [TV_CHATFIELD_LUT_ANGLE_WIDTH][TV_CHATFIELD_LUT_THROTTLE_WIDTH];

// Private Functions ----------------------------------------------------------------------------------------------------------

float getThrottleIndex (float throttleValue)
{
	float index = 100.0f * throttleValue / TV_CHATFIELD_THROTTLE_RESOLUTION;
	if (index < 0.0f)
		index = 0.0f;
	if (index > (uint16_t) TV_CHATFIELD_LUT_THROTTLE_WIDTH - 1.0f)
		index = (uint16_t) TV_CHATFIELD_LUT_THROTTLE_WIDTH - 1.0f;
	return index;
}

float getAngleIndex (float angleValue)
{
	float index = (angleValue + TV_CHATFIELD_ANGLE_RANGE) / TV_CHATFIELD_ANGLE_RESOLUTION;
	if (index < 0.0f)
		index = 0.0f;
	if (index > (uint16_t) TV_CHATFIELD_LUT_ANGLE_WIDTH - 1.0f)
		index = (uint16_t) TV_CHATFIELD_LUT_ANGLE_WIDTH - 1.0f;
	return index;
}

float getBiasRightHand (float throttle, float angle)
{
	float throttleIndex3 = getThrottleIndex (throttle);
	uint16_t throttleIndex1 = (uint16_t) floor (throttleIndex3);
	uint16_t throttleIndex2 = (uint16_t) ceil (throttleIndex3);

	float angleIndex3 = getAngleIndex (angle);
	uint16_t angleIndex1 = (uint16_t) floor (angleIndex3);
	uint16_t angleIndex2 = (uint16_t) ceil (angleIndex3);

	float torque11 = (*lookupTable) [angleIndex1][throttleIndex1];
	float torque12 = (*lookupTable) [angleIndex2][throttleIndex1];
	float torque21 = (*lookupTable) [angleIndex1][throttleIndex2];
	float torque22 = (*lookupTable) [angleIndex2][throttleIndex2];

	return bilinearInterpolation (throttleIndex3, angleIndex3, throttleIndex1, angleIndex1, throttleIndex2, angleIndex2,
		torque11, torque12, torque21, torque22);
}

// Functions ------------------------------------------------------------------------------------------------------------------

void tvChatfieldInit (void)
{
	// Cast the EEPROM's LUT into the correct dimension LUT.
	lookupTable = (float (*) [TV_CHATFIELD_LUT_ANGLE_WIDTH][TV_CHATFIELD_LUT_THROTTLE_WIDTH]) eeprom.chatfieldLut;
}

tvOutput_t tvChatfield (const tvInput_t* input)
{
	float throttle = pedals.appsRequest;
	float angle = sas.value;

	float biasFront = *eeprom.drivingTorqueBias;
	float biasRear = 1 - biasFront;
	float biasRightHand = getBiasRightHand (throttle, angle);
	float biasLeftHand = 1 - biasRightHand;

	tvOutput_t output =
	{
		.valid = sas.state == LINEAR_SENSOR_VALID,
		.torqueRl = biasRightHand * biasRear  * input->drivingTorqueLimit,
		.torqueRr = biasLeftHand  * biasRear  * input->drivingTorqueLimit,
		.torqueFl = biasRightHand * biasFront * input->drivingTorqueLimit,
		.torqueFr = biasLeftHand  * biasFront * input->drivingTorqueLimit
	};
	return output;
}