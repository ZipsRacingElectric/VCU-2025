#ifndef EEPROM_MAP_H
#define EEPROM_MAP_H

// EEPROM Mapping -------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.22
//
// Description: Structing mapping the data of an EEPROM data to variables.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/mc24lc32.h"

// C Standard Library
#include <stdint.h>

// Datatypes ------------------------------------------------------------------------------------------------------------------

struct eepromMapConfig
{
	/// @brief The 7-bit I2C address of the device.
	uint8_t addr;

	/// @brief The IC2 bus of the device.
	I2CDriver* i2c;
};

typedef struct eepromMapConfig eepromMapConfig_t;

struct eepromMap
{
	mc24lc32_t device;

	uint16_t* apps1Min;
	uint16_t* apps1Max;
	uint16_t* apps2Min;
	uint16_t* apps2Max;

	uint16_t* bseFMin;
	uint16_t* bseFMax;
	uint16_t* bseRMin;
	uint16_t* bseRMax;

	uint16_t* sasMin;
	uint16_t* sasMax;
	float* sasAngleRange;
};

typedef struct eepromMap eepromMap_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool eepromMapInit (eepromMap_t* eeprom, eepromMapConfig_t* config);

#endif // EEPROM_MAP_H