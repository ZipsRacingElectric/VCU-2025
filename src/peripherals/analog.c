// Header
#include "analog.h"

void analogInit (analog_t* adc, analogConfig_t* config)
{
	// Copy the config
	adc->driver		= config->driver;
	adc->channels	= config->channels;
	adc->handlers	= config->handlers;

	ADCConfig driverConfig =
	{
		
	};
}