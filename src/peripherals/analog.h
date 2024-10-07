#ifndef ANALOG_H
#define ANALOG_H

// Analog Peripherals ---------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.05
//
// Description: Aggregate peripheral for sensors using the on-board ADC.
//
// To do:
// - This is a pretty weak way of going about things, this'll be replaced with a more flexible solution.

// Includes -------------------------------------------------------------------------------------------------------------------

// Include
#include "pedals.h"

// ChibiOS
#include "hal.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

struct analog
{
	ADCDriver*			driver;
	ADCConversionGroup	group;
	adcsample_t			samples [6];
	apps_t				apps1;
	apps_t				apps2;
	bse_t				bseF;
	bse_t				bseR;
	float				steeringAngle;
	float				glvVoltage;
};

typedef struct analog analog_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool analogInit (analog_t* analog);

void analogSample (analog_t* analog);

#endif // ANALOG_H