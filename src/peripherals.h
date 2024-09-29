#ifndef PERIPHERALS_H
#define PERIPHERALS_H

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/mc24lc32.h"

// ChibiOS
#include "ch.h"

// Debugging ------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from peripherals.
#define PERIPHERAL_DEBUGGING 1

// Global Peripherals ---------------------------------------------------------------------------------------------------------

extern mc24lc32_t eeprom;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the devices peripherals.
 */
void peripheralsInit (void);

#endif // PERIPHERALS_H