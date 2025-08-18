// Header
#include "peripherals.h"

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "torque_thread.h"
#include "controls/lerp.h"

// Global Peripherals ---------------------------------------------------------------------------------------------------------

stmAdc_t		adc;
mc24lc32_t		eeprom;
linearSensor_t	glvBattery;
pedals_t		pedals;
am4096_t		sasAdc;
sas_t			sas;
virtualEeprom_t virtualMemory;
eeprom_t		readonlyMemory;

// Configuration --------------------------------------------------------------------------------------------------------------

/// @brief Configuration for the I2C1 bus.
static const I2CConfig I2C1_CONFIG =
{
	.op_mode		= OPMODE_I2C,
	.clock_speed	= 100000,
	.duty_cycle		= STD_DUTY_CYCLE
};

static const am4096Config_t SAS_ADC_CONFIG =
{
	.addr		= 0x00, // TODO(Barach): Move this to EEPROM so it can be changed.
	.i2c		= &I2CD1,
	.sensor		= (analogSensor_t*) &sas,
	.timeout	= TIME_MS2I (5)
};

/// @brief Configuration for the ADC1 peripheral.
static const stmAdcConfig_t ADC_CONFIG =
{
	.driver = &ADCD1,
	.channels =
	{
		ADC_CHANNEL_IN10,	// APPS-1
		ADC_CHANNEL_IN11,	// APPS-2
		ADC_CHANNEL_IN13,	// BSE-F // TODO(Barach): Fix
		ADC_CHANNEL_IN13,	// BSE-R
		ADC_CHANNEL_IN0		// GLV Battery
	},
	.channelCount = 5,
	.sensors =
	{
		(analogSensor_t*) &pedals.apps1,
		(analogSensor_t*) &pedals.apps2,
		(analogSensor_t*) &pedals.bseF,
		(analogSensor_t*) &pedals.bseR,
		(analogSensor_t*) &glvBattery
	}
};

/// @brief Configuration for the on-board EEPROM.
static const mc24lc32Config_t EEPROM_CONFIG =
{
	.addr			= 0x50,
	.i2c			= &I2CD1,
	.timeout		= TIME_MS2I (500),
	.magicString	= EEPROM_MAP_STRING,
	.dirtyHook		= peripheralsReconfigure
};

static const virtualEepromConfig_t VIRTUAL_MEMORY_CONFIG =
{
	.count		= 3,
	.entries	=
	{
		{
			.eeprom = (eeprom_t*) &eeprom,
			.addr = 0x0000,
			.size = 0x1000
		},
		{
			.eeprom = &readonlyMemory,
			.addr = 0x1000,
			.size = 0x1000
		},
		{
			.eeprom = (eeprom_t*) &sasAdc,
			.addr = 0x2000,
			.size = 0x0038
		}
	}
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

bool peripheralsInit ()
{
	// I2C 1 driver initialization.
	if (i2cStart (&I2CD1, &I2C1_CONFIG) != MSG_OK)
		return false;

	// ADC 1 driver initialization.
	if (!stmAdcInit (&adc, &ADC_CONFIG))
		return false;

	// EEPROM initialization (only exit early if a failure occurred).
	if (!mc24lc32Init (&eeprom, &EEPROM_CONFIG) && eeprom.state == MC24LC32_STATE_FAILED)
		return false;

	// Virtual memory initialization.
	// TODO(Barach): Cleanup
	readonlyMemory.readHandler = eepromReadonlyRead;
	readonlyMemory.writeHandler = eepromWriteonlyWrite;
	virtualEepromInit (&virtualMemory, &VIRTUAL_MEMORY_CONFIG);

	// Re-configurable peripherals are not considered fatal.
	peripheralsReconfigure (NULL);
	return true;
}

void peripheralsReconfigure (void* arg)
{
	(void) arg;

	// Pedals initialization
	pedalsInit (&pedals, &eepromMap->pedalConfig);

	// SAS initialization
	sasInit (&sas, &eepromMap->sasConfig);
	am4096Init (&sasAdc, &SAS_ADC_CONFIG);

	// Torque thread configuration
	torqueThreadSetDrivingTorqueLimit (eepromMap->drivingTorqueLimit);
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
}