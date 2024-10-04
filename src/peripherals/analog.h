#ifndef ANALOG_H
#define ANALOG_H

// ChibiOS
#include "hal.h"

typedef void (adcHandler) (void*, uint16_t);

struct analogConfig
{
	ADCDriver* driver;
	uint16_t* channels;
	adcHandler* handlers;
};

struct analog
{
	ADCDriver* driver;
	uint16_t* channels;
	adcHandler* handlers;
};

typedef struct analog analog_t;

typedef struct analogConfig analogConfig_t;

void analogInit (analog_t* adc, analogConfig_t* config);

#endif // ANALOG_H