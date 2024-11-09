#ifndef ANALOG_H
#define ANALOG_H

// Analog Inputs --------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.05
//
// Description: Peripheral wrapping the ChibiOS ADC driver.

// Includes -------------------------------------------------------------------------------------------------------------------

// Include
#include "pedals.h"

// ChibiOS
#include "hal.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from this peripheral.
#define ANALOG_DEBUGGING 1

// Constants ------------------------------------------------------------------------------------------------------------------

#define ANALOG_CHANNEL_COUNT 16

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef void (analogCallback_t) (void*, adcsample_t);

typedef enum
{
	ANALOG_STATE_READY	= 0,
	ANALOG_STATE_FAILED	= 1,
} analogState_t;

typedef struct
{
	/// @brief The ADC driver to use.
	ADCDriver* driver;
	/// @brief The number of channels in the sample sequence.
	uint8_t channelCount;
	/// @brief The sequence of channels to sample.
	adc_channels_num_t channels [ANALOG_CHANNEL_COUNT];
	/// @brief The callback for each sample in the sequence. Use null for no callback.
	analogCallback_t* callbacks [ANALOG_CHANNEL_COUNT];
	/// @brief Object to use for the handler argument of each callback.
	void* handlers [ANALOG_CHANNEL_COUNT];
	/// @brief The sampling time selection to use for every channel.
	uint32_t sampleCycles;
} analogConfig_t;

typedef struct
{
	analogState_t		state;
	ADCDriver*			driver;
	ADCConversionGroup	group;
	uint8_t				channelCount;
	adcsample_t			samples [ANALOG_CHANNEL_COUNT];
	analogCallback_t**	callbacks;
	void**				handlers;
} analog_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool analogInit (analog_t* analog, analogConfig_t* config);

void analogSample (analog_t* analog);

#endif // ANALOG_H