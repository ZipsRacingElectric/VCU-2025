#ifndef PERIPHERALS_H
#define PERIPHERALS_H

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/eeprom_map.h"
#include "peripherals/pedals.h"
#include "peripherals/steering_angle.h"

// Debugging ------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from peripherals.
#define PERIPHERAL_DEBUGGING 1

// Global Peripherals ---------------------------------------------------------------------------------------------------------

extern eepromMap_t	eeprom;
extern float		glvBatteryVoltage;
extern pedals_t		pedals;
extern sas_t		sas;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the devices peripherals.
 */
void peripheralsInit (void);

/**
 * @brief Re-initializes the device peripherals after a change has been made to the device EEPROM.
 */
void peripheralsReinit (void);

#endif // PERIPHERALS_H