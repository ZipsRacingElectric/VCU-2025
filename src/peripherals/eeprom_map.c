// Header
#include "eeprom_map.h"

// Memory Mapping -------------------------------------------------------------------------------------------------------------

/// @brief The magic string of the EEPROM. Update this value every time the memory map changes to force manual re-programming.
#define MAGIC_STRING "VCU_2025_02_10"

#define PEDAL_CONFIG_ADDR				0x10

#define DRIVING_TORQUE_LIMIT_ADDR		0x30
#define DRIVING_TORQUE_BIAS_ADDR		0x34
#define REGEN_TORQUE_LIMIT_ADDR			0x38
#define REGEN_TORQUE_BIAS_ADDR			0x3C
#define TORQUE_ALGORITHM_INDEX_ADDR		0x40

#define POWER_LIMIT_ADDR				0x44
#define POWER_LIMIT_PID_KP_ADDR			0x48
#define POWER_LIMIT_PID_KI_ADDR			0x4C
#define POWER_LIMIT_PID_KD_ADDR			0x50
#define POWER_LIMIT_PID_A_ADDR			0x54

#define GLV_BATTERY_SAMPLE_11V5_ADDR	0x58
#define GLV_BATTERY_SAMPLE_14V4_ADDR	0x5A

#define SAS_CONFIG_ADDR					0x5C

#define CHATFIELD_LUT_ADDR				0x100

// Functions ------------------------------------------------------------------------------------------------------------------

bool eepromMapInit (eepromMap_t* eeprom, eepromMapConfig_t* config)
{
	eeprom->pedalConfig			= (pedalsConfig_t*) (eeprom->device.cache + PEDAL_CONFIG_ADDR);

	eeprom->drivingTorqueLimit	= (float*) (eeprom->device.cache + DRIVING_TORQUE_LIMIT_ADDR);
	eeprom->drivingTorqueBias	= (float*) (eeprom->device.cache + DRIVING_TORQUE_BIAS_ADDR);
	eeprom->regenTorqueLimit	= (float*) (eeprom->device.cache + REGEN_TORQUE_LIMIT_ADDR);
	eeprom->regenTorqueBias		= (float*) (eeprom->device.cache + REGEN_TORQUE_BIAS_ADDR);
	eeprom->torqueAlgoritmIndex	= (uint16_t*) (eeprom->device.cache + TORQUE_ALGORITHM_INDEX_ADDR);

	eeprom->powerLimit			= (float*) (eeprom->device.cache + POWER_LIMIT_ADDR);
	eeprom->powerLimitPidKp		= (float*) (eeprom->device.cache + POWER_LIMIT_PID_KP_ADDR);
	eeprom->powerLimitPidKi		= (float*) (eeprom->device.cache + POWER_LIMIT_PID_KI_ADDR);
	eeprom->powerLimitPidKd		= (float*) (eeprom->device.cache + POWER_LIMIT_PID_KD_ADDR);
	eeprom->powerLimitPidA		= (float*) (eeprom->device.cache + POWER_LIMIT_PID_A_ADDR);

	eeprom->glvBattery11v5		= (uint16_t*) (eeprom->device.cache + GLV_BATTERY_SAMPLE_11V5_ADDR);
	eeprom->glvBattery14v4		= (uint16_t*) (eeprom->device.cache + GLV_BATTERY_SAMPLE_14V4_ADDR);

	eeprom->sasConfig			= (sasConfig_t*) (eeprom->device.cache + SAS_CONFIG_ADDR);

	eeprom->chatfieldLut		= (float*) (eeprom->device.cache + CHATFIELD_LUT_ADDR);

	mc24lc32Config_t mc24lc32Config =
	{
		.addr			= config->addr,
		.i2c			= config->i2c,
		.magicString	= MAGIC_STRING,
		.timeoutPeriod	= config->timeoutPeriod
	};
	return mc24lc32Init (&eeprom->device, &mc24lc32Config);
}