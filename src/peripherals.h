#ifndef PERIPHERALS_H
#define PERIPHERALS_H

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/pedals.h"

// Common Includes
#include "mc24lc32.h"

// Debugging ------------------------------------------------------------------------------------------------------------------

/// @brief Enables / disables debugging messages from peripherals.
#define PERIPHERAL_DEBUGGING 1

// Global Peripherals ---------------------------------------------------------------------------------------------------------

extern mc24lc32_t	eeprom;
extern apps_t		apps1;
extern apps_t		apps2;
extern bse_t		bseF;
extern bse_t		bseR;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the devices peripherals.
 */
void peripheralsInit (void);

#endif // PERIPHERALS_H