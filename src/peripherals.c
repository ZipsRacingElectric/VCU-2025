// Header
#include "peripherals.h"

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"

// ChibiOS
#include "hal.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

#if PERIPHERAL_DEBUGGING
	#define PERIPHERAL_PRINTF(format, ...) DEBUG_PRINTF("[Peripherals] " format "\r\n", ##__VA_ARGS__)
#else
	#define PERIPHERAL_PRINTF()
#endif // PERIPHERAL_DEBUGGING

// Global Peripherals ---------------------------------------------------------------------------------------------------------

analog_t	analog;
mc24lc32_t	eeprom;

// Configuration --------------------------------------------------------------------------------------------------------------

/// @brief Configuration for the I2C1 bus.
static I2CConfig i2cConfig =
{
	.op_mode		= OPMODE_I2C,
	.clock_speed	= 115200,
	.duty_cycle		= STD_DUTY_CYCLE
};

/// @brief Configuration for the on-board EEPROM.
static mc24lc32Config_t eepromConfig =
{
	.addr			= 0x50,
	.i2c			= &I2CD1,
	.magicString	= "VCU"
};

// Functions ------------------------------------------------------------------------------------------------------------------

void peripheralsInit ()
{
	// I2C 3 driver initialization
	if (i2cStart (&I2CD1, &i2cConfig) != MSG_OK)
		PERIPHERAL_PRINTF ("Failed to initialize I2C1.");

	// EEPROM initialization
	if (!mc24lc32Init (&eeprom, &eepromConfig))
		PERIPHERAL_PRINTF ("Failed to initialize the EEPROM.");

	// Analog sensor initialization
	if (!analogInit (&analog))
		PERIPHERAL_PRINTF ("Failed to initialize the analog sensors");
}