// Header
#include "peripherals.h"

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"
#include "torque_thread.h"

// ChibiOS
#include "hal.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define R24 10000.0f
#define R32 2290.0f

// Macros ---------------------------------------------------------------------------------------------------------------------

#if PERIPHERAL_DEBUGGING
	#define PERIPHERAL_PRINTF(format, ...) DEBUG_PRINTF ("[Peripherals] " format, ##__VA_ARGS__)
#else
	#define PERIPHERAL_PRINTF(format, ...) while (false)
#endif // PERIPHERAL_DEBUGGING

// Global Peripherals ---------------------------------------------------------------------------------------------------------

eepromMap_t	eeprom;
float		glvBatteryVoltage;
pedals_t	pedals;
sas_t		sas;

// Functions Prototypes -------------------------------------------------------------------------------------------------------

void glvBatteryCallback (void* arg, uint16_t value);

// Configuration --------------------------------------------------------------------------------------------------------------

/// @brief Configuration for the I2C1 bus.
static I2CConfig i2cConfig =
{
	.op_mode		= OPMODE_I2C,
	.clock_speed	= 400000,
	.duty_cycle		= FAST_DUTY_CYCLE_2
};

/// @brief Configuration for the on-board EEPROM.
static eepromMapConfig_t eepromConfig =
{
	.addr	= 0x50,
	.i2c	= &I2CD1
};

/// @brief Configuration for the pedal sensors.
/// @note Minimum & maximum values are loaded from the EEPROM.
static pedalsConfig_t pedalsConfig =
{
	.adc = &ADCD1,
	.apps1Config =
	{
		.channel	= ADC_CHANNEL_IN10,
		.rawMin		= 0,
		.rawMax		= 0
	},
	.apps2Config =
	{
		.channel	= ADC_CHANNEL_IN11,
		.rawMin		= 0,
		.rawMax		= 0
	},
	.bseFConfig =
	{
		.channel	= ADC_CHANNEL_IN12,
		.rawMin		= 0,
		.rawMax		= 0
	},
	.bseRConfig =
	{
		.channel	= ADC_CHANNEL_IN13,
		.rawMin		= 0,
		.rawMax		= 0
	}
};

/// @brief Configuration for the steering-angle sensor. (computed at runtime).
static sasConfig_t sasConfig;

// Functions ------------------------------------------------------------------------------------------------------------------

void peripheralsInit ()
{
	// I2C 3 driver initialization
	if (i2cStart (&I2CD1, &i2cConfig) != MSG_OK)
		PERIPHERAL_PRINTF ("Failed to initialize I2C 1.\r\n");

	// ADC initialization
	if (adcStart (&ADCD1, NULL) != MSG_OK)
		PERIPHERAL_PRINTF ("Failed to initialize ADC 1.\r\n");

	// EEPROM initialization
	if (!eepromMapInit (&eeprom, &eepromConfig))
	{
		switch (eeprom.device.state)
		{
		case MC24LC32_STATE_INVALID:
			PERIPHERAL_PRINTF ("EEPROM memory is invalid.\r\n");
			break;
		default:
			PERIPHERAL_PRINTF ("Failed to initialize the EEPROM.\r\n");
			break;
		}
	}

	peripheralsReconfigure ();
}

void peripheralsReconfigure (void)
{
	if (eeprom.device.state != MC24LC32_STATE_READY)
		return;

	// Pedals initialization
	pedalsConfig.apps1Config.rawMin	= *eeprom.apps1Min;
	pedalsConfig.apps1Config.rawMax	= *eeprom.apps1Max;
	pedalsConfig.apps2Config.rawMin	= *eeprom.apps2Min;
	pedalsConfig.apps2Config.rawMax	= *eeprom.apps2Max;
	pedalsConfig.bseFConfig.rawMin	= *eeprom.bseFMin;
	pedalsConfig.bseFConfig.rawMax	= *eeprom.bseFMax;
	pedalsConfig.bseRConfig.rawMin	= *eeprom.bseRMin;
	pedalsConfig.bseRConfig.rawMax	= *eeprom.bseRMax;

	if (!pedalsInit (&pedals, &pedalsConfig))
		PERIPHERAL_PRINTF ("Failed to initialize the pedals.");

	// SAS initialization
	sasConfig.rawMin = *eeprom.sasMin;
	sasConfig.rawMax = *eeprom.sasMax;

	if (!sasInit (&sas, &sasConfig))
		PERIPHERAL_PRINTF ("Failed to initialize the SAS.");

	// Torque thread configuration
	torqueThreadSetLimit (*eeprom.torqueLimit);
	torqueThreadSelectAlgorithm (*eeprom.torqueAlgoritmIndex);
}

void glvBatteryCallback (void* arg, uint16_t value)
{
	(void) arg;
	glvBatteryVoltage = (3.3f * value / 4096.0f) * (R24 + R32) / R32;
}