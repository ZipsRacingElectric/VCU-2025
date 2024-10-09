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
extern apps_t		apps1;
extern apps_t		apps2;
extern bse_t		bseF;
extern bse_t		bseR;
extern mc24lc32_t	eeprom;
extern float		steeringAngle;
extern float		glvBatteryVoltage;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the devices peripherals.
 */
void peripheralsInit (void);

/**
 * @brief Gets a code representing the state of the device's peripherals. 
 */
uint32_t peripheralsGetState (void);

#endif // PERIPHERALS_H