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

	uint16_t debugAddress0;			// 0x0070
	uint16_t debugAddress1;			// 0x0072
	uint16_t debugAddress2;			// 0x0074
	uint16_t debugAddress3;			// 0x0076

	uint8_t pad1 [136];				// 0x0078

	float chatfieldLut;				// 0x0100
} eepromMap_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool eepromMapReadonlyCallback (uint16_t addr, void** data, uint8_t* dataCount);

float eepromMapGetReadonly (uint16_t addr);

#endif // EEPROM_MAP_H