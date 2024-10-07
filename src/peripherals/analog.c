// Header
#include "analog.h"

// Includes
#include "eeprom.h"

bool analogInit (analog_t* analog)
{
	ADCConversionGroup group =
	{
		.circular		=	false,
		.num_channels	=	6,
		.end_cb			= 	NULL,
		.error_cb		= 	NULL,
		.cr1			=	0,
		.cr2			=	ADC_CR2_SWSTART,
		.smpr1			=	(ADC_SAMPLE_480	<< ADC_SMPR1_SMP10_Pos) |		// Channel 10: 480 cycles per sample.
							(ADC_SAMPLE_480	<< ADC_SMPR1_SMP11_Pos) |		// Channel 11: 480 cycles per sample.
							(ADC_SAMPLE_480	<< ADC_SMPR1_SMP12_Pos) |		// Channel 12: 480 cycles per sample.
							(ADC_SAMPLE_480	<< ADC_SMPR1_SMP13_Pos) |		// Channel 13: 480 cycles per sample.
							(ADC_SAMPLE_480	<< ADC_SMPR1_SMP14_Pos),		// Channel 14: 480 cycles per sample.
		.smpr2			= 	(ADC_SAMPLE_480	<< ADC_SMPR2_SMP0_Pos),			// Channel 0: 480 cycles per sample.
		.htr			=	0,
		.ltr			=	0,
		.sqr1			= 	(6 << ADC_SQR1_L_Pos),							// 6 samples in the sequence
		.sqr2			=	0,
		.sqr3			=	(ADC_CHANNEL_IN10	<< ADC_SQR3_SQ1_Pos) |		// Sample 1 is APPS-1
							(ADC_CHANNEL_IN11	<< ADC_SQR3_SQ2_Pos) |		// Sample 2 is APPS-2
							(ADC_CHANNEL_IN12	<< ADC_SQR3_SQ3_Pos) |		// Sample 3 is BSE-F
							(ADC_CHANNEL_IN13	<< ADC_SQR3_SQ4_Pos) |		// Sample 4 is BSE-R
							(ADC_CHANNEL_IN14	<< ADC_SQR3_SQ5_Pos) |		// Sample 5 is the SAS
							(ADC_CHANNEL_IN0	<< ADC_SQR3_SQ6_Pos)		// Sample 6 is the GLV Battery
	};

	analog->driver	= &ADCD1;
	analog->group	= group;

	analog->apps1.rawMin	= *apps1Min;
	analog->apps1.rawMax	= *apps1Max;
	analog->apps2.rawMin	= *apps2Min;
	analog->apps2.rawMax	= *apps2Max;
	analog->bseF.rawMin		= *bseFMin;
	analog->bseF.rawMax		= *bseFMax;
	analog->bseR.rawMin		= *bseRMin;
	analog->bseR.rawMax		= *bseRMax;

	return adcStart (analog->driver, NULL) == MSG_OK;
}

void analogSample (analog_t* analog)
{
	// Sample the ADC channels
	msg_t result = adcConvert (analog->driver, &analog->group, analog->samples, 1);
	// TODO(Barach): Device state

	appsUpdate (&analog->apps1, analog->samples [0]);
	appsUpdate (&analog->apps2, analog->samples [1]);
	bseUpdate (&analog->bseF, analog->samples [2]);
	bseUpdate (&analog->bseR, analog->samples [3]);
	
	// TODO(Barach): Conversions
	analog->steeringAngle = analog->samples [4];
	analog->glvVoltage = analog->samples [5];
}