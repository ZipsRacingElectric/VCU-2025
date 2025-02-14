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
#include "peripherals/pedals.h"
#include "peripherals/steering_angle.h"

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The magic string of the EEPROM. Update this value every time the memory map changes to force manual re-programming.
#define EEPROM_MAP_STRING "VCU_2025_02_10"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	uint8_t pad0 [16];				// 0x0000

	pedalsConfig_t pedalConfig;		// 0x0010

	float drivingTorqueLimit;		// 0x0030
	float drivingTorqueBias;		// 0x0034
	float regenTorqueLimit;			// 0x0038
	float regenTorqueBias;			// 0x003C
	uint16_t torqueAlgoritmIndex;	// 0x0040

	float powerLimit;				// 0x0044
	float powerLimitPidKp;			// 0x0048
	float powerLimitPidKi;			// 0x004C
	float powerLimitPidKd;			// 0x0050
	float powerLimitPidA;			// 0x0054

	uint16_t glvBattery11v5;		// 0x0058
	uint16_t glvBattery14v4;		// 0x005A

	sasConfig_t sasConfig;			// 0x005C

	uint8_t pad1 [144];				// 0x0070

	float chatfieldLut;				// 0x0100
} eepromMap_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool eepromMapReadonlyCallback (uint16_t addr, void** data, uint8_t* dataCount);

#endif // EEPROM_MAP_H