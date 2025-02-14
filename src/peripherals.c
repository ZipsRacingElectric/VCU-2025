// Header
#include "peripherals.h"

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"
#include "torque_thread.h"
#include "controls/lerp.h"
#include "controls/tv_chatfield.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

#if PERIPHERAL_DEBUGGING
	#define PERIPHERAL_PRINTF(format, ...) DEBUG_PRINTF ("[Peripherals] " format, ##__VA_ARGS__)
#else
	#define PERIPHERAL_PRINTF(format, ...) while (false)
#endif // PERIPHERAL_DEBUGGING

// Global Peripherals ---------------------------------------------------------------------------------------------------------

analog_t		adc;
mc24lc32_t		eeprom;
eepromMap_t*	eepromMap;
linearSensor_t	glvBattery;
pedals_t		pedals;
sas_t			sas;

// Configuration --------------------------------------------------------------------------------------------------------------

/// @brief Configuration for the I2C1 bus.
static I2CConfig i2cConfig =
{
	.op_mode		= OPMODE_I2C,
	.clock_speed	= 400000,
	.duty_cycle		= FAST_DUTY_CYCLE_2
};

/// @brief Configuration for the ADC1 peripheral.
static analogConfig_t adcConfig =
{
	.driver = &ADCD1,
	.channels =
	{
		ADC_CHANNEL_IN10,	// APPS-1
		ADC_CHANNEL_IN11,	// APPS-2
		ADC_CHANNEL_IN12,	// BSE-F
		ADC_CHANNEL_IN13,	// BSE-R
		ADC_CHANNEL_IN14,	// SAS
		ADC_CHANNEL_IN0		// GLV Battery
	},
	.channelCount = 6,
	.handlers =
	{
		pedalSensorUpdate,
		pedalSensorUpdate,
		pedalSensorUpdate,
		pedalSensorUpdate,
		sasUpdate,
		linearSensorUpdate
	},
	.objects =
	{
		&pedals.apps1,
		&pedals.apps2,
		&pedals.bseF,
		&pedals.bseR,
		&sas,
		&glvBattery
	}
};

/// @brief Configuration for the on-board EEPROM.
static mc24lc32Config_t eepromConfig =
{
	.addr			= 0x50,
	.i2c			= &I2CD1,
	.timeoutPeriod	= TIME_MS2I (500),
	.magicString	= EEPROM_MAP_STRING
};

/// @brief Configuration for the GVL battery voltage measurment.
/// @note Minimum and maximum voltages are loaded from the EEPROM.
static linearSensorConfig_t glvBatteryConfig =
{
	.sampleMin	= 0,
	.sampleMax	= 4095,
	.valueMin	= 0,
	.valueMax	= 0
};

// Functions ------------------------------------------------------------------------------------------------------------------

void peripheralsInit ()
{
	// I2C 3 driver initialization
	if (i2cStart (&I2CD1, &i2cConfig) != MSG_OK)
		PERIPHERAL_PRINTF ("Failed to initialize I2C 1.\r\n");

	// ADC1 initialization
	if (!analogInit (&adc, &adcConfig))
		PERIPHERAL_PRINTF ("Failed to initialize ADC 1.\r\n");

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

	eepromMap = (eepromMap_t*) eeprom.cache;

	peripheralsReconfigure ();
}

void peripheralsReconfigure (void)
{
	// Pedals initialization
	if (!pedalsInit (&pedals, &eepromMap->pedalConfig))
		PERIPHERAL_PRINTF ("Failed to initialize the pedals.");

	// SAS initialization
	if (!sasInit (&sas, &eepromMap->sasConfig))
		PERIPHERAL_PRINTF ("Failed to initialize the SAS.");

	// Torque thread configuration
	torqueThreadSetDrivingTorqueLimit (eepromMap->drivingTorqueLimit);
	torqueThreadSetRegenTorqueLimit (eepromMap->regenTorqueLimit);
	torqueThreadSelectAlgorithm (eepromMap->torqueAlgoritmIndex);
	torqueThreadSetPowerLimit (eepromMap->powerLimit);
	torqueThreadSetPowerLimitPid
	(
		eepromMap->powerLimitPidKp,
		eepromMap->powerLimitPidKi,
		eepromMap->powerLimitPidKd,
		eepromMap->powerLimitPidA
	);

	// GLV battery initialization
	uint16_t glvSample11v5 = eepromMap->glvBattery11v5;
	uint16_t glvSample14v4 = eepromMap->glvBattery14v4;
	glvBatteryConfig.valueMin = lerp2d (0, glvSample11v5, 11.5f, glvSample14v4, 14.4f);
	glvBatteryConfig.valueMax = lerp2d (4095, glvSample11v5, 11.5f, glvSample14v4, 14.4f);
	linearSensorInit (&glvBattery, &glvBatteryConfig);

	// Chatfield LUT initialization
	tvChatfieldInit ();
}