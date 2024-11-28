// Header
#include "tv_chatfield.h"

// Includes
#include "peripherals.h"

// Global Constants -----------------------------------------------------------------------------------------------------------

float** lookupTable;

// Function Prototypes --------------------------------------------------------------------------------------------------------

uint16_t getThrottleIndex (float steeringValue);

uint16_t getAngleIndex (float throttleValue);

float getLutRl (const tvInput_t* input, uint16_t throttleIndex, uint16_t angleIndex);

float getLutRr (const tvInput_t* input, uint16_t throttleIndex, uint16_t angleIndex);

float getLutFl (const tvInput_t* input, uint16_t throttleIndex, uint16_t angleIndex);

float getLutFr (const tvInput_t* input, uint16_t throttleIndex, uint16_t angleIndex);

// Functions ------------------------------------------------------------------------------------------------------------------

void tvChatfieldInit (void)
{
	lookupTable = eeprom.chatfieldLut;
}

tvOutput_t tvChatfield (const tvInput_t* input)
{
	// Get the loopup table indices.
	uint16_t throttleIndex	= getThrottleIndex (pedals.appsRequest);
	uint16_t angleIndex		= getAngleIndex (sas.value);

	tvOutput_t output =
	{
		.valid = true,
		.torqueRl = getLutRl (input, throttleIndex, angleIndex),
		.torqueRr = getLutRr (input, throttleIndex, angleIndex),
		.torqueFl = getLutFl (input, throttleIndex, angleIndex),
		.torqueFr = getLutFr (input, throttleIndex, angleIndex)
	};
	return output;
}

uint16_t getThrottleIndex (float steeringValue)
{
	// Clamp the input value to the table's range.
	if (steeringValue < -TV_CHATFIELD_ANGLE_RANGE)
		steeringValue = -TV_CHATFIELD_ANGLE_RANGE;
	if (steeringValue > TV_CHATFIELD_ANGLE_RANGE)
		steeringValue = TV_CHATFIELD_ANGLE_RANGE;

	// Linear interpolation from [-range, range] to [0, width]. (Add 0.5 to make flooring act like rounding).
	return (uint16_t) ((sas.value + TV_CHATFIELD_ANGLE_RANGE) / TV_CHATFIELD_ANGLE_RESOLUTION + 0.5f);
}

uint16_t getAngleIndex (float throttleValue)
{
	// Clamp the input value to the table's range.
	if (throttleValue < 0)
		throttleValue = 0;
	if (throttleValue > TV_CHATFIELD_THROTTLE_RANGE)
		throttleValue = TV_CHATFIELD_THROTTLE_RANGE;

	// Linear interpolation from [0, range] to [0, width]. (Add 0.5 to make flooring act like rounding).
	return (uint16_t) (throttleValue / TV_CHATFIELD_THROTTLE_RESOLUTION + 0.5f);
}

float getLutRl (const tvInput_t* input, uint16_t throttleIndex, uint16_t angleIndex)
{
	float value = lookupTable [throttleIndex][angleIndex];

	// Torque / regen scaling
	if (value >= 0)
		value *= input->torqueLimit / 2.0f * input->torqueBias;
	else
	 	value = input->regenLimit / 2.0f * input->regenBias;

	return value;
}

float getLutRr (const tvInput_t* input, uint16_t throttleIndex, uint16_t angleIndex)
{
	// Right LUT is mirror of left LUT.
	angleIndex = TV_CHATFIELD_LUT_ANGLE_WIDTH - angleIndex - 1;
	float value = lookupTable [throttleIndex][angleIndex];

	// Torque / regen scaling
	if (value >= 0)
		value *= input->torqueLimit / 2.0f * input->torqueBias;
	else
	 	value = input->regenLimit / 2.0f * input->regenBias;

	return value;
}

float getLutFl (const tvInput_t* input, uint16_t throttleIndex, uint16_t angleIndex)
{
	float value = lookupTable [throttleIndex][angleIndex];

	// Torque / regen scaling
	if (value >= 0)
		value *= input->torqueLimit / 2.0f * (1 - input->torqueBias);
	else
	 	value = input->regenLimit / 2.0f * (1 - input->regenBias);

	return value;
}

float getLutFr (const tvInput_t* input, uint16_t throttleIndex, uint16_t angleIndex)
{
	// Right LUT is mirror of left LUT.
	angleIndex = TV_CHATFIELD_LUT_ANGLE_WIDTH - angleIndex - 1;
	float value = lookupTable [throttleIndex][angleIndex];

	// Torque / regen scaling
	if (value >= 0)
		value *= input->torqueLimit / 2.0f * (1 - input->torqueBias);
	else
	 	value = input->regenLimit / 2.0f * (1 - input->regenBias);

	return value;
}