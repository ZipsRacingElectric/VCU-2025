// Header
#include "eeprom_map.h"

// Memory Mapping -------------------------------------------------------------------------------------------------------------

/// @brief The magic string of the EEPROM. Update this value every time the memory map changes to force manual re-programming.
#define MAGIC_STRING "VCU_2024_11_28"

#define APPS_1_MIN_ADDR					0x10
#define APPS_1_MAX_ADDR					0x12
#define APPS_2_MIN_ADDR					0x14
#define APPS_2_MAX_ADDR					0x16

#define BSE_F_MIN_ADDR					0x18
#define BSE_F_MAX_ADDR					0x1A
#define BSE_R_MIN_ADDR					0x1C
#define BSE_R_MAX_ADDR					0x1E

#define SAS_MIN_ADDR					0x20
#define SAS_MAX_ADDR					0x22
#define SAS_ANGLE_RANGE_ADDR			0x24

#define TORQUE_LIMIT_ADDR				0x28
#define TORQUE_BIAS_ADDR				0x2C
#define REGEN_LIMIT_ADDR				0x30
#define REGEN_BIAS_ADDR					0x34
#define TORQUE_ALGORITHM_INDEX_ADDR		0x38

#define CHATFIELD_LUT_ADDR				0x3A

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

	eeprom->sasMin				= (uint16_t*) (eeprom->device.cache + SAS_MIN_ADDR);
	eeprom->sasMax				= (uint16_t*) (eeprom->device.cache + SAS_MAX_ADDR);
	eeprom->sasAngleRange		= (float*) (eeprom->device.cache + SAS_ANGLE_RANGE_ADDR);

	eeprom->torqueLimit			= (float*) (eeprom->device.cache + TORQUE_LIMIT_ADDR);
	eeprom->torqueBias			= (float*) (eeprom->device.cache + TORQUE_BIAS_ADDR);
	eeprom->regenLimit			= (float*) (eeprom->device.cache + REGEN_LIMIT_ADDR);
	eeprom->regenBias			= (float*) (eeprom->device.cache + REGEN_BIAS_ADDR);
	eeprom->torqueAlgoritmIndex	= (uint16_t*) (eeprom->device.cache + TORQUE_ALGORITHM_INDEX_ADDR);

	eeprom->chatfieldLut		= (float**) (eeprom->device.cache + CHATFIELD_LUT_ADDR);

	mc24lc32Config_t mc24lc32Config =
	{
		.addr			= config->addr,
		.i2c			= config->i2c,
		.magicString	= MAGIC_STRING
	};
	return mc24lc32Init (&eeprom->device, &mc24lc32Config);
}