// Header
#include "eeprom_map.h"

// Memory Mapping -------------------------------------------------------------------------------------------------------------

/// @brief The magic string of the EEPROM. Update this value every time the memory map changes to force manual re-programming.
#define MAGIC_STRING "VCU_2025_01_31"

#define APPS_1_MIN_ADDR					0x10
#define APPS_1_MAX_ADDR					0x12
#define APPS_2_MIN_ADDR					0x14
#define APPS_2_MAX_ADDR					0x16

#define BSE_F_MIN_ADDR					0x18
#define BSE_F_MAX_ADDR					0x1A
#define BSE_R_MIN_ADDR					0x1C
#define BSE_R_MAX_ADDR					0x1E

#define DRIVING_TORQUE_LIMIT_ADDR		0x28
#define DRIVING_TORQUE_BIAS_ADDR		0x2C
#define REGEN_TORQUE_LIMIT_ADDR			0x30
#define REGEN_TORQUE_BIAS_ADDR			0x34
#define TORQUE_ALGORITHM_INDEX_ADDR		0x38

#define POWER_LIMIT_ADDR				0x3C
#define POWER_LIMIT_PID_KP_ADDR			0x40
#define POWER_LIMIT_PID_KI_ADDR			0x44
#define POWER_LIMIT_PID_KD_ADDR			0x48
#define POWER_LIMIT_PID_A_ADDR			0x4C

#define GLV_BATTERY_SAMPLE_11V5_ADDR	0x50
#define GLV_BATTERY_SAMPLE_14V4_ADDR	0x52

#define SAS_SAMPLE_OFFSET_ADDR			0x54
#define SAS_POSITIVE_SAMPLE_ADDR		0x56
#define SAS_NEGATIVE_SAMPLE_ADDR		0x58
#define SAS_POSITIVE_ANGLE_ADDR			0x5C
#define SAS_NEGATIVE_ANGLE_ADDR			0x60
#define SAS_DEADZONE_RANGE_ADDR			0x64

#define CHATFIELD_LUT_ADDR				0x100

// Functions ------------------------------------------------------------------------------------------------------------------

bool eepromMapInit (eepromMap_t* eeprom, eepromMapConfig_t* config)
{
	eeprom->apps1Min 			= (uint16_t*) (eeprom->device.cache + APPS_1_MIN_ADDR);
	eeprom->apps1Max 			= (uint16_t*) (eeprom->device.cache + APPS_1_MAX_ADDR);
	eeprom->apps2Min 			= (uint16_t*) (eeprom->device.cache + APPS_2_MIN_ADDR);
	eeprom->apps2Max 			= (uint16_t*) (eeprom->device.cache + APPS_2_MAX_ADDR);

	eeprom->bseFMin 			= (uint16_t*) (eeprom->device.cache + BSE_F_MIN_ADDR);
	eeprom->bseFMax 			= (uint16_t*) (eeprom->device.cache + BSE_F_MAX_ADDR);
	eeprom->bseRMin 			= (uint16_t*) (eeprom->device.cache + BSE_R_MIN_ADDR);
	eeprom->bseRMax 			= (uint16_t*) (eeprom->device.cache + BSE_R_MAX_ADDR);

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

	eeprom->sasSampleOffset		= (uint16_t*) (eeprom->device.cache + SAS_SAMPLE_OFFSET_ADDR);
	eeprom->sasPositiveSample	= (uint16_t*) (eeprom->device.cache + SAS_POSITIVE_SAMPLE_ADDR);
	eeprom->sasNegativeSample	= (uint16_t*) (eeprom->device.cache + SAS_NEGATIVE_SAMPLE_ADDR);
	eeprom->sasPositiveAngle 	= (float*) (eeprom->device.cache + SAS_POSITIVE_ANGLE_ADDR);
	eeprom->sasNegativeAngle 	= (float*) (eeprom->device.cache + SAS_NEGATIVE_ANGLE_ADDR);
	eeprom->sasDeadzoneRange 	= (float*) (eeprom->device.cache + SAS_DEADZONE_RANGE_ADDR);

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