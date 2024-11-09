// Header
#include "analog.h"

// Includes
#include "debug.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

#if ANALOG_DEBUGGING
	#define ANALOG_PRINTF(format, ...) DEBUG_PRINTF ("[Analog] " format, ##__VA_ARGS__)
#else
	#define ANALOG_PRINTF(format, ...) while (false)
#endif // ANALOG_DEBUGGING

// Functions ------------------------------------------------------------------------------------------------------------------

bool analogInit (analog_t* analog, analogConfig_t* config)
{
	analog->state = ANALOG_STATE_READY;

	// Set all uninitialized channels to 0
	for (uint8_t index = config->channelCount; index < ANALOG_CHANNEL_COUNT; ++index)
		config->channels [index] = 0;

	// Compute the conversion group for the ADC.
	ADCConversionGroup group =
	{
		.circular		=	false,
		.num_channels	=	config->channelCount,
		.end_cb			= 	NULL,
		.error_cb		= 	NULL,
		.cr1			=	0,
		.cr2			=	ADC_CR2_SWSTART,
		.smpr1			=	(config->sampleCycles << ADC_SMPR1_SMP15_Pos) |	// Channel 15 sample time.
							(config->sampleCycles << ADC_SMPR1_SMP14_Pos) |	// Channel 14 sample time.
							(config->sampleCycles << ADC_SMPR1_SMP13_Pos) |	// Channel 13 sample time.
							(config->sampleCycles << ADC_SMPR1_SMP12_Pos) |	// Channel 12 sample time.
							(config->sampleCycles << ADC_SMPR1_SMP11_Pos) |	// Channel 11 sample time.
							(config->sampleCycles << ADC_SMPR1_SMP10_Pos),	// Channel 10 sample time.
		.smpr2			= 	(config->sampleCycles << ADC_SMPR2_SMP9_Pos) |	// Channel 9 sample time.
							(config->sampleCycles << ADC_SMPR2_SMP8_Pos) |	// Channel 8 sample time.
							(config->sampleCycles << ADC_SMPR2_SMP7_Pos) |	// Channel 7 sample time.
							(config->sampleCycles << ADC_SMPR2_SMP6_Pos) |	// Channel 6 sample time.
							(config->sampleCycles << ADC_SMPR2_SMP5_Pos) |	// Channel 5 sample time.
							(config->sampleCycles << ADC_SMPR2_SMP4_Pos) |	// Channel 4 sample time.
							(config->sampleCycles << ADC_SMPR2_SMP3_Pos) |	// Channel 3 sample time.
							(config->sampleCycles << ADC_SMPR2_SMP2_Pos) |	// Channel 2 sample time.
							(config->sampleCycles << ADC_SMPR2_SMP1_Pos) |	// Channel 1 sample time.
							(config->sampleCycles << ADC_SMPR2_SMP0_Pos),	// Channel 0 sample time.
		.htr			=	0,												// No watchdog threshold
		.ltr			=	0,
		.sqr1			= 	(ADC_SQR1_NUM_CH (config->channelCount)) |		// Number of channels in the conversion sequence.
							(ADC_SQR1_SQ16_N (config->channels [15])) |		// Sample 16 channel number.
							(ADC_SQR1_SQ15_N (config->channels [14])) |		// Sample 15 channel number.
							(ADC_SQR1_SQ14_N (config->channels [13])) |		// Sample 14 channel number.
							(ADC_SQR1_SQ13_N (config->channels [12])),		// Sample 13 channel number.
		.sqr2			=	(ADC_SQR2_SQ12_N (config->channels [11])) |		// Sample 12 channel number.
							(ADC_SQR2_SQ11_N (config->channels [10])) |		// Sample 11 channel number.
							(ADC_SQR2_SQ10_N (config->channels [9])) |		// Sample 10 channel number.
							(ADC_SQR2_SQ9_N (config->channels [8])) |		// Sample 9 channel number.
							(ADC_SQR2_SQ8_N (config->channels [7])) |		// Sample 8 channel number.
							(ADC_SQR2_SQ7_N (config->channels [6])),		// Sample 7 channel number.
		.sqr3			=	(ADC_SQR3_SQ6_N (config->channels [5])) |		// Sample 6 channel number.
							(ADC_SQR3_SQ5_N (config->channels [4])) |		// Sample 5 channel number.
							(ADC_SQR3_SQ4_N (config->channels [3])) |		// Sample 4 channel number.
							(ADC_SQR3_SQ3_N (config->channels [2])) |		// Sample 3 channel number.
							(ADC_SQR3_SQ2_N (config->channels [1])) |		// Sample 2 channel number.
							(ADC_SQR3_SQ1_N (config->channels [0]))			// Sample 1 channel number.
	};

	// Store the configuration
	analog->driver			= config->driver;
	analog->channelCount	= config->channelCount;
	analog->group			= group;
	analog->callbacks		= config->callbacks;
	analog->handlers		= config->handlers;

	// Start the ADC driver
	msg_t result = adcStart (analog->driver, NULL);
	if (result != MSG_OK)
	{
		analog->state = ANALOG_STATE_FAILED;
		return false;
	}

	return true;
}

void analogSample (analog_t* analog)
{
	// Sample the ADC channels
	msg_t result = adcConvert (analog->driver, &analog->group, analog->samples, 1);
	if (result != MSG_OK)
	{
		analog->state = ANALOG_STATE_FAILED;
		return;
	}

	ANALOG_PRINTF ("Sampled: ");

	// Call each handler's callback
	for (uint8_t index = 0; index < analog->channelCount; ++index)
	{
		#if ANALOG_DEBUGGING
		DEBUG_PRINTF ("%u, ", analog->samples [index]);
		#endif // ANALOG_DEBUGGING

		if (analog->callbacks [index] != NULL)
			analog->callbacks [index] (analog->handlers [index], analog->samples [index]);
	}

	#if ANALOG_DEBUGGING
	DEBUG_PRINTF ("\r\n");
	#endif // ANALOG_DEBUGGING
}