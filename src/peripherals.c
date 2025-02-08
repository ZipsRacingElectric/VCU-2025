// Header
#include "peripherals.h"

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"
#include "torque_thread.h"
#include "controls/lerp.h"
#include "controls/tv_chatfield.h"

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

analog_t		adc1;
eepromMap_t		eeprom;
linearSensor_t	glvBattery;
pedals_t		pedals;
sas_t			sas;

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

/// @brief Configuration for the ADC1 peripheral.
static analogConfig_t adc1Config =
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
		linearSensorUpdate,
		linearSensorUpdate,
		linearSensorUpdate,
		linearSensorUpdate,
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
static eepromMapConfig_t eepromConfig =
{
	.addr			= 0x50,
	.i2c			= &I2CD1,
	.timeoutPeriod	= TIME_MS2I (500)
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

/// @brief Configuration for the pedal sensors.
/// @note Minimum & maximum samples are loaded from the EEPROM.
static pedalsConfig_t pedalsConfig =
{
	.apps1Config =
	{
		.sampleMin		= 0,
		.sampleMax		= 0,
		.valueMin		= 0,
		.valueMax		= 1
	},
	.apps2Config =
	{
		.sampleMin		= 0,
		.sampleMax		= 0,
		.valueMin		= 0,
		.valueMax		= 1
	},
	.bseFConfig =
	{
		.sampleMin		= 0,
		.sampleMax		= 0,
		.valueMin		= 0,
		.valueMax		= 1
	},
	.bseRConfig =
	{
		.sampleMin		= 0,
		.sampleMax		= 0,
		.valueMin		= 0,
		.valueMax		= 1
	}
};

/// @brief Configuration for the steering-angle sensor.
static sasConfig_t sasConfig =
{
	.sampleOffset	= 0,
	.sampleNegative	= 0,
	.samplePositive	= 0,
	.angleNegative	= 0,
	.anglePositive	= 0,
	.angleDeadzone	= 0
};

// Functions ------------------------------------------------------------------------------------------------------------------

void peripheralsInit ()
{
	// I2C 3 driver initialization
	if (i2cStart (&I2CD1, &i2cConfig) != MSG_OK)
		PERIPHERAL_PRINTF ("Failed to initialize I2C 1.\r\n");

	// ADC1 initialization
	if (!analogInit (&adc1, &adc1Config))
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
	// Pedals initialization
	if (eeprom.device.state == MC24LC32_STATE_READY)
	{
		pedalsConfig.apps1Config.sampleMin	= *eeprom.apps1Min;
		pedalsConfig.apps1Config.sampleMax	= *eeprom.apps1Max;
		pedalsConfig.apps2Config.sampleMin	= *eeprom.apps2Min;
		pedalsConfig.apps2Config.sampleMax	= *eeprom.apps2Max;
		pedalsConfig.bseFConfig.sampleMin	= *eeprom.bseFMin;
		pedalsConfig.bseFConfig.sampleMax	= *eeprom.bseFMax;
		pedalsConfig.bseRConfig.sampleMin	= *eeprom.bseRMin;
		pedalsConfig.bseRConfig.sampleMax	= *eeprom.bseRMax;
	}

	if (!pedalsInit (&pedals, &pedalsConfig))
		PERIPHERAL_PRINTF ("Failed to initialize the pedals.");

	// SAS initialization
	if (eeprom.device.state == MC24LC32_STATE_READY)
	{
		sasConfig.sampleOffset		= *eeprom.sasSampleOffset;
		sasConfig.sampleNegative	= *eeprom.sasNegativeSample;
		sasConfig.samplePositive	= *eeprom.sasPositiveSample;
		sasConfig.angleNegative		= *eeprom.sasNegativeAngle;
		sasConfig.anglePositive		= *eeprom.sasPositiveAngle;
		sasConfig.angleDeadzone		= *eeprom.sasDeadzoneRange;
	}

	if (!sasInit (&sas, &sasConfig))
		PERIPHERAL_PRINTF ("Failed to initialize the SAS.");

	// Torque thread configuration
	if (eeprom.device.state == MC24LC32_STATE_READY)
	{
		torqueThreadSetDrivingTorqueLimit (*eeprom.drivingTorqueLimit);
		torqueThreadSetRegenTorqueLimit (*eeprom.regenTorqueLimit);
		torqueThreadSelectAlgorithm (*eeprom.torqueAlgoritmIndex);
		torqueThreadSetPowerLimit (*eeprom.powerLimit);
		torqueThreadSetPowerLimitPid
		(
			*eeprom.powerLimitPidKp,
			*eeprom.powerLimitPidKi,
			*eeprom.powerLimitPidKd,
			*eeprom.powerLimitPidA
		);
	}

	// GLV battery initialization
	if (eeprom.device.state == MC24LC32_STATE_READY)
	{
		uint16_t glvSample11v5 = *eeprom.glvBattery11v5;
		uint16_t glvSample14v4 = *eeprom.glvBattery14v4;
		glvBatteryConfig.valueMin = lerp2d (0, glvSample11v5, 11.5f, glvSample14v4, 14.4f);
		glvBatteryConfig.valueMax = lerp2d (4095, glvSample11v5, 11.5f, glvSample14v4, 14.4f);
		linearSensorInit (&glvBattery, &glvBatteryConfig);
	}

	if (eeprom.device.state == MC24LC32_STATE_READY)
		tvChatfieldInit ();
}