#ifndef PERIPHERALS_H
#define PERIPHERALS_H

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/analog.h"
#include "peripherals/eeprom_map.h"
#include "peripherals/linear_sensor.h"
#include "peripherals/mc24lc32.h"
#include "peripherals/pedals.h"
#include "peripherals/steering_angle.h"

// Debugging ------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from peripherals.
#define PERIPHERAL_DEBUGGING 1

// Global Peripherals ---------------------------------------------------------------------------------------------------------

extern analog_t			adc;
extern mc24lc32_t		eeprom;
extern eepromMap_t*		eepromMap;
extern linearSensor_t	glvBattery;
extern pedals_t			pedals;
extern sas_t			sas;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the devices peripherals.
 */
void peripheralsInit (void);

/**
 * @brief Re-initializes the device peripherals after a change has been made to the device EEPROM.
 */
void peripheralsReconfigure (void);

#endif // PERIPHERALS_H