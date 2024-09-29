// Header
#include "peripherals.h"

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"

// ChibiOS
#include "hal.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

#if PERIPHERAL_DEBUGGING
	#define PERIPHERAL_PRINTF(format, ...) DEBUG_PRINTF("[Peripherals] " format, ##__VA_ARGS__)
#else
	#define PERIPHERAL_PRINTF()
#endif // PERIPHERAL_DEBUGGING

// Global Peripherals ---------------------------------------------------------------------------------------------------------

mc24lc32_t eeprom;

// Configuration --------------------------------------------------------------------------------------------------------------

mc24lc32Config_t eepromConfig =
{
	.addr	= 0x50,
	.bus	= &I2CD3
};

// Functions ------------------------------------------------------------------------------------------------------------------

void peripheralsInit ()
{
	// EEPROM initialization
	if (!mc24lc32Init (&eeprom, &eepromConfig))
		PERIPHERAL_PRINTF ("Failed to initialize the EEPROM.");
}