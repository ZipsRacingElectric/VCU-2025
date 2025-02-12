#ifndef EEPROM_MAP_H
#define EEPROM_MAP_H

// EEPROM Mapping -------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.22
//
// Description: Structing mapping the data of an EEPROM data to variables.
//
// TODO(Barach):
// - Consider creating a map struct that you can cast the EEPROM cache to. This would prevent alignment issues and reduce all
//   the init duplication.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/mc24lc32.h"
#include "peripherals/pedals.h"
#include "peripherals/steering_angle.h"

// C Standard Library
#include <stdint.h>

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	/// @brief The 7-bit I2C address of the device.
	uint8_t addr;

	/// @brief The IC2 bus of the device.
	I2CDriver* i2c;

	/// @brief The timeout interval for the device's acknowledgement polling. If the device does not send an acknowledgement
	/// within this timeframe, it will be considered invalid.
	sysinterval_t timeoutPeriod;
} eepromMapConfig_t;

typedef struct
{
	mc24lc32_t device;

	pedalsConfig_t* pedalConfig;

	float* drivingTorqueLimit;
	float* drivingTorqueBias;
	float* regenTorqueLimit;
	float* regenTorqueBias;
	uint16_t* torqueAlgoritmIndex;

	float* powerLimit;
	float* powerLimitPidKp;
	float* powerLimitPidKi;
	float* powerLimitPidKd;
	float* powerLimitPidA;

	uint16_t* glvBattery11v5;
	uint16_t* glvBattery14v4;

	sasConfig_t* sasConfig;

	float* chatfieldLut;
} eepromMap_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool eepromMapInit (eepromMap_t* eeprom, eepromMapConfig_t* config);

#endif // EEPROM_MAP_H