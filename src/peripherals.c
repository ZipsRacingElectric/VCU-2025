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

mc24lc32_t eeprom;

// Configuration --------------------------------------------------------------------------------------------------------------

static const I2CConfig i2cConfig =
{
	.op_mode		= OPMODE_I2C,
	.clock_speed	= 115200,
	.duty_cycle		= STD_DUTY_CYCLE
};

/**
 * @brief Configuration of the CAN 1 & CAN 2 peripherals.
 * @note See section 32.9 of the STM32F405 Reference Manual for more details.
 */
static const CANConfig canConfig =
{
	.mcr = 	CAN_MCR_ABOM |		// Automatic bus-off management.
			CAN_MCR_AWUM |		// Automatic wakeup mode.
			CAN_MCR_TXFP,		// Chronologic FIFI priority.
	.btr =	CAN_BTR_SJW(0) |	// Max 1 TQ resynchronization jump. 
			CAN_BTR_TS2(1) |	// 2 TQ for time segment 2
			CAN_BTR_TS1(10) |	// 11 TQ for time segment 1
			CAN_BTR_BRP(2)		// Baudrate divisor of 3 (1 Mbps)
};

/// @brief Configuration for the board EEPROM.
mc24lc32Config_t eepromConfig =
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
	
	// CAN 1 driver initialization
	if (canStart (&CAND1, &canConfig) != MSG_OK)
		PERIPHERAL_PRINTF ("Failed to initialize CAN1.");
	
	palClearLine (LINE_CAN1_STBY);
	
	// CAN 2 driver initialization
	if (canStart (&CAND2, &canConfig) != MSG_OK)
		PERIPHERAL_PRINTF ("Failed to initialize CAN2.");
		
	palClearLine (LINE_CAN2_STBY);

	// EEPROM initialization
	if (!mc24lc32Init (&eeprom, &eepromConfig))
		PERIPHERAL_PRINTF ("Failed to initialize the EEPROM.");
}