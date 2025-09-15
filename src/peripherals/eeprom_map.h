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
#include "controls/tv_const_bias.h"
#include "controls/tv_linear_bias.h"

// Constants ------------------------------------------------------------------------------------------------------------------

/// @brief The magic string of the EEPROM. Update this value every time the memory map changes to force manual re-programming.
#define EEPROM_MAP_STRING "VCU_2025_08_22"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef struct
{
	uint8_t pad0 [16];					// 0x0000

	pedalsConfig_t pedalConfig;			// 0x0010

	float drivingTorqueLimit;			// 0x0030
	float regenTorqueLimit;				// 0x0034
	uint16_t torqueAlgoritmIndex;		// 0x0038

	float powerLimit;					// 0x003C
	float powerLimitPidKp;				// 0x0040
	float powerLimitPidKi;				// 0x0044
	float powerLimitPidKd;				// 0x0048
	float powerLimitPidA;				// 0x004C

	uint16_t glvBattery11v5;			// 0x0050
	uint16_t glvBattery14v4;			// 0x0052

	uint8_t pad1 [12];

	sasConfig_t sasConfig;				// 0x0060

	// Straight-diff TV config
	tvConstBiasConfig_t sdConfig;		// 0x0074

	// Linear-steering TV config
	tvLinearBiasConfig_t lsConfig;		// 0x0084

	// Linear-steering (slalom) TV config
	tvLinearBiasConfig_t lssConfig;		// 0x00A0

	uint8_t pad2 [36];					// 0x00BC

	float regenLightRequest;			// 0x00E0
	float regenHardRequest;				// 0x00E4
	float regenDeratingThrottleStart;	// 0x00E8
	float regenDeratingThrottleEnd;		// 0x00EC
	float regenDeratingSpeedEnd;		// 0x00F0
	float regenDeratingSpeedStart;		// 0x00F4

	bool sasEnabled;					// 0x00F8
	uint8_t sasAddr;					// 0x00F9
} eepromMap_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool eepromReadonlyRead (void* object, uint16_t addr, void* data, uint16_t dataCount);

bool eepromWriteonlyWrite (void* object, uint16_t addr, const void* data, uint16_t dataCount);

#endif // EEPROM_MAP_H