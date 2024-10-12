// Header
#include "peripherals.h"

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"

// ChibiOS
#include "hal.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define R24 10000.0f
#define R32 2290.0f

// Macros ---------------------------------------------------------------------------------------------------------------------

#if PERIPHERAL_DEBUGGING
	#define PERIPHERAL_PRINTF(format, ...) DEBUG_PRINTF("[Peripherals] " format, ##__VA_ARGS__)
#else
	#define PERIPHERAL_PRINTF(format, ...) while(false)
#endif // PERIPHERAL_DEBUGGING

// Global Peripherals ---------------------------------------------------------------------------------------------------------

analog_t	adc;
pedals_t	pedals;
mc24lc32_t	eeprom;
float		steeringAngle;
float		glvBatteryVoltage;

// Functions Prototypes -------------------------------------------------------------------------------------------------------

void steeringAngleCallback (void* arg, uint16_t value);

void glvBatteryCallback (void* arg, uint16_t value);

// Configuration --------------------------------------------------------------------------------------------------------------

/// @brief Configuration for the I2C1 bus.
static I2CConfig i2cConfig =
{
	.op_mode		= OPMODE_I2C,
	.clock_speed	= 400000,
	.duty_cycle		= FAST_DUTY_CYCLE_16_9
};

/// @brief Configuration for the ADC.
static analogConfig_t adcConfig =
{
	.driver			= &ADCD1,
	.channelCount	= 6,
	.channels =
	{
		ADC_CHANNEL_IN10,
		ADC_CHANNEL_IN11,
		ADC_CHANNEL_IN12,
		ADC_CHANNEL_IN13,
		ADC_CHANNEL_IN14,
		ADC_CHANNEL_IN0
	},
	.callbacks =
	{
		pedalSensorCallback,
		pedalSensorCallback,
		pedalSensorCallback,
		pedalSensorCallback,
		steeringAngleCallback,
		glvBatteryCallback
	},
	.handlers =
	{
		&pedals.apps1,
		&pedals.apps2,
		&pedals.bseF,
		&pedals.bseR,
		NULL,
		NULL
	},
	.sampleCycles = ADC_SAMPLE_480
};

/// @brief Configuration for the on-board EEPROM.
static mc24lc32Config_t eepromConfig =
{
	.addr			= 0x50,
	.i2c			= &I2CD1,
	.magicString	= "VCU_2024_10_07"
};

/// @brief Configuration for the pedal sensors (computed at runtime).
static pedalsConfig_t pedalsConfig;

// Functions ------------------------------------------------------------------------------------------------------------------

void peripheralsInit ()
{
	// I2C 3 driver initialization
	if (i2cStart (&I2CD1, &i2cConfig) != MSG_OK)
		PERIPHERAL_PRINTF ("Failed to initialize I2C1.\r\n");

	// EEPROM initialization
	if (!mc24lc32Init (&eeprom, &eepromConfig))
	{
		switch (eeprom.state)
		{
		case MC24LC32_STATE_INVALID:
			PERIPHERAL_PRINTF ("EEPROM memory is invalid.\r\n");
			break;
		default:
			PERIPHERAL_PRINTF ("Failed to initialize the EEPROM.\r\n");
			break;
		}
	}

	// Analog sensor initialization
	if (!analogInit (&adc, &adcConfig))
		PERIPHERAL_PRINTF ("Failed to initialize the ADC.\r\n");

	// Pedals initialization
	// TODO(Barach): From EEPROM
	pedalsConfig.apps1Config.rawMin	= 0;
	pedalsConfig.apps1Config.rawMax	= 4095;
	pedalsConfig.apps2Config.rawMin	= 0;
	pedalsConfig.apps2Config.rawMax	= 4095;
	pedalsConfig.bseFConfig.rawMin	= 0;
	pedalsConfig.bseFConfig.rawMax	= 4095;
	pedalsConfig.bseRConfig.rawMin	= 0;
	pedalsConfig.bseRConfig.rawMax	= 4095;

	if (!pedalsInit (&pedals, &pedalsConfig))
		PERIPHERAL_PRINTF ("Failed to initialize the pedals.");
}

void steeringAngleCallback (void* arg, uint16_t value)
{
	(void) arg;
	// TODO(Barach): Calibration values.
	steeringAngle = (3.3f * value / 4096.0f);
}

void glvBatteryCallback (void* arg, uint16_t value)
{
	(void) arg;
	// TODO(Barach): Is is 4096 or 4095?
	glvBatteryVoltage = (3.3f * value / 4096.0f) * (R24 + R32) / R32;
}