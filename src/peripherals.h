#ifndef PERIPHERALS_H
#define PERIPHERALS_H

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/analog.h"
#include "peripherals/pedals.h"
#include "peripherals/mc24lc32.h"

// Debugging ------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from peripherals.
#define PERIPHERAL_DEBUGGING 1

// Global Peripherals ---------------------------------------------------------------------------------------------------------

extern analog_t		adc;
extern pedals_t		pedals;
extern mc24lc32_t	eeprom;
extern float		steeringAngle;
extern float		glvBatteryVoltage;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the devices peripherals.
 */
void peripheralsInit (void);

#endif // PERIPHERALS_H