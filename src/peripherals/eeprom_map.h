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
	uint8_t pad0 [16];				// 0x000

	pedalsConfig_t pedalConfig;		// 0x010

	float drivingTorqueLimit;		// 0x030
	float drivingTorqueBias;		// 0x034
	float regenTorqueLimit;			// 0x038
	float regenTorqueBias;			// 0x03C
	uint16_t torqueAlgoritmIndex;	// 0x040

	float powerLimit;				// 0x044
	float powerLimitPidKp;			// 0x048
	float powerLimitPidKi;			// 0x04C
	float powerLimitPidKd;			// 0x050
	float powerLimitPidA;			// 0x054

	uint16_t glvBattery11v5;		// 0x058
	uint16_t glvBattery14v4;		// 0x05A

	sasConfig_t sasConfig;			// 0x05C

	uint16_t debugAddress0;			// 0x070
	uint16_t debugAddress1;			// 0x072
	uint16_t debugAddress2;			// 0x074
	uint16_t debugAddress3;			// 0x076

	uint8_t pad1 [136];				// 0x078

	float chatfieldLut;				// 0x100
} eepromMap_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool eepromReadonlyRead (void* object, uint16_t addr, void* data, uint16_t dataCount);

bool eepromWriteonlyWrite (void* object, uint16_t addr, const void* data, uint16_t dataCount);

#endif // EEPROM_MAP_H