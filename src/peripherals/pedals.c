// Header
#include "pedals.h"

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The maximum acceptable delta of the APPS-1 and APPS-2 sensor values.
#define APPS_DELTA_MAX 0.1

#define APPS_ACCELERATING_VALUE 0.1
#define BSE_BRAKING_VALUE		0.1

// Function Prototypes --------------------------------------------------------------------------------------------------------

bool pedalSensorInit (pedalSensor_t* sensor, pedalSensorConfig_t* config);

bool pedalSensorUpdate (pedalSensor_t* sensor, adcsample_t raw);

// Functions ------------------------------------------------------------------------------------------------------------------

bool pedalSensorInit (pedalSensor_t* sensor, pedalSensorConfig_t* config)
{
	// Validate the configuration
	sensor->configPlausible = config->rawMin < config->rawMax;

	// Store the configuration
	sensor->rawMin = config->rawMin;
	sensor->rawMax = config->rawMax;

	// Set values to their defaults
	sensor->plausible = false;
	sensor->value = 0.0f;
	sensor->valueRaw = 0;

	return sensor->configPlausible;
}

bool pedalSensorUpdate (pedalSensor_t* sensor, adcsample_t raw)
{
	// Config check
	if (!sensor->configPlausible)
	{
		sensor->plausible = false;
		return false;
	}

	sensor->valueRaw = raw;

	// Range check
	sensor->plausible = raw >= sensor->rawMin && raw <= sensor->rawMax;

	// Linear interpolation
	sensor->value = (float) (raw - sensor->rawMin) / (sensor->rawMax - sensor->rawMin);
	return sensor->plausible;
}

bool pedalsInit (pedals_t* pedals, pedalsConfig_t* config)
{
	// Store the configuration.
	pedals->config = config;

	// Compute the conversion group for the ADC.
	ADCConversionGroup group =
	{
		.circular		=	false,
		.num_channels	=	4,
		.end_cb			= 	NULL,
		.error_cb		= 	NULL,
		.cr1			=	0,
		.cr2			=	ADC_CR2_SWSTART,									// ADC is started by software.
		.smpr1			=	(ADC_SAMPLE_480 << ADC_SMPR1_SMP15_Pos) |			// Channel 15 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR1_SMP14_Pos) |			// Channel 14 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR1_SMP13_Pos) |			// Channel 13 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR1_SMP12_Pos) |			// Channel 12 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR1_SMP11_Pos) |			// Channel 11 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR1_SMP10_Pos),			// Channel 10 sample time.
		.smpr2			= 	(ADC_SAMPLE_480 << ADC_SMPR2_SMP9_Pos) |			// Channel 9 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP8_Pos) |			// Channel 8 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP7_Pos) |			// Channel 7 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP6_Pos) |			// Channel 6 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP5_Pos) |			// Channel 5 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP4_Pos) |			// Channel 4 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP3_Pos) |			// Channel 3 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP2_Pos) |			// Channel 2 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP1_Pos) |			// Channel 1 sample time.
							(ADC_SAMPLE_480 << ADC_SMPR2_SMP0_Pos),				// Channel 0 sample time.
		.htr			=	0,													// No watchdog threshold.
		.ltr			=	0,
		.sqr1			= 	0,
		.sqr2			=	0,
		.sqr3			=	(ADC_SQR3_SQ4_N (config->bseRConfig.channel)) |		// Sample 4 channel number.
							(ADC_SQR3_SQ3_N (config->bseFConfig.channel)) |		// Sample 3 channel number.
							(ADC_SQR3_SQ2_N (config->apps2Config.channel)) |	// Sample 2 channel number.
							(ADC_SQR3_SQ1_N (config->apps1Config.channel))		// Sample 1 channel number.
	};
	pedals->adcGroup = group;

	// Configure and validate the individual sensors
	bool result = pedalSensorInit (&pedals->apps1, &config->apps1Config);
	result &= pedalSensorInit (&pedals->apps2, &config->apps2Config);
	result &= pedalSensorInit (&pedals->bseF, &config->bseFConfig);
	result &= pedalSensorInit (&pedals->bseR, &config->bseRConfig);
	return result;
}

void pedalsSample (pedals_t* pedals, systime_t timeCurrent)
{
	// TODO(Barach): Deadlines
	(void) timeCurrent;

	// Sample the ADC.
	adcConvert (pedals->config->adc, &pedals->adcGroup, pedals->adcBuffer, 1);

	// Validate the sensors individually.
	pedalSensorUpdate (&pedals->apps1, pedals->adcBuffer [0]);
	pedalSensorUpdate (&pedals->apps2, pedals->adcBuffer [1]);
	pedalSensorUpdate (&pedals->bseF, pedals->adcBuffer [2]);
	pedalSensorUpdate (&pedals->bseR, pedals->adcBuffer [3]);

	// Calculate the average of both sensors.
	pedals->appsRequest	= (pedals->apps1.value + pedals->apps2.value) / 2.0;
	pedals->bseRequest = (pedals->bseF.value + pedals->bseR.value) / 2.0;

	pedals->isAccelerating	= pedals->appsRequest > APPS_ACCELERATING_VALUE;
	pedals->isBraking		= pedals->bseRequest > BSE_BRAKING_VALUE;

	// APPS 10% check (TODO(Barach): Rule no.)
	float appsDelta = pedals->apps1.value - pedals->apps2.value;
	pedals->apps10PercentPlausible = appsDelta <= APPS_DELTA_MAX && appsDelta >= -APPS_DELTA_MAX;

	// TODO(Barach): APPS 25/5

	// TODO(Barach): 100ms timeout
	pedals->plausible =
		pedals->apps1.plausible && pedals->apps2.plausible &&
		pedals->bseF.plausible && pedals->bseR.plausible &&
		pedals->apps10PercentPlausible;
}