// Header
#include "eeprom_map.h"

// Includes
#include "peripherals.h"
#include "torque_thread.h"

// Global Data ----------------------------------------------------------------------------------------------------------------

typedef enum
{
	EEPROM_TYPE_UINT8_T		= 0,
	EEPROM_TYPE_UINT16_T	= 1,
	EEPROM_TYPE_UINT32_T	= 2,
	EEPROM_TYPE_FLOAT		= 3,
} eepromTypes_t;

#define READONLY_COUNT (sizeof (READONLY_ADDRS) / sizeof (READONLY_ADDRS [0]))
static uint16_t READONLY_ADDRS [] =
{
	0x1000,
	0x1002,
	0x1004,
	0x1006,
	0x1008,
	0x100A,
	0x100C,
	0x1010,
	0x1014,
	0x1018
};

static eepromTypes_t READONLY_TYPES [] =
{
	EEPROM_TYPE_UINT16_T,
	EEPROM_TYPE_UINT16_T,
	EEPROM_TYPE_UINT16_T,
	EEPROM_TYPE_UINT16_T,
	EEPROM_TYPE_UINT16_T,
	EEPROM_TYPE_UINT16_T,
	EEPROM_TYPE_FLOAT,
	EEPROM_TYPE_FLOAT,
	EEPROM_TYPE_FLOAT,
	EEPROM_TYPE_FLOAT
};

static void* READONLY_DATA [READONLY_COUNT] =
{
	&pedals.apps1.sample,
	&pedals.apps2.sample,
	&pedals.bseF.sample,
	&pedals.bseR.sample,
	&sas.sample,
	&glvBattery.sample,
	&torqueRequest.torqueRl,
	&torqueRequest.torqueRr,
	&torqueRequest.torqueFl,
	&torqueRequest.torqueFr
};

static uint8_t READONLY_DATACOUNTS [READONLY_COUNT] =
{
	sizeof (pedals.apps1.sample),
	sizeof (pedals.apps2.sample),
	sizeof (pedals.bseF.sample),
	sizeof (pedals.bseR.sample),
	sizeof (sas.sample),
	sizeof (glvBattery.sample),
	sizeof (torqueRequest.torqueRl),
	sizeof (torqueRequest.torqueRr),
	sizeof (torqueRequest.torqueFl),
	sizeof (torqueRequest.torqueFr)
};

// Functions ------------------------------------------------------------------------------------------------------------------

bool eepromMapReadonlyCallback (uint16_t addr, void** data, uint8_t* dataCount)
{
	for (uint16_t index = 0; index < READONLY_COUNT; ++index)
	{
		if (addr != READONLY_ADDRS [index])
			continue;

		*data = READONLY_DATA [index];
		*dataCount = READONLY_DATACOUNTS [index];

		return true;
	}

	return false;
}

float eepromMapGetReadonly (uint16_t addr)
{
	for (uint16_t index = 0; index < READONLY_COUNT; ++index)
	{
		if (addr != READONLY_ADDRS [index])
			continue;

		switch (READONLY_TYPES [index])
		{
		case EEPROM_TYPE_UINT8_T:
			return (float) (*((uint8_t*) READONLY_DATA [index]));
		case EEPROM_TYPE_UINT16_T:
			return (float) (*((uint16_t*) READONLY_DATA [index]));
		case EEPROM_TYPE_UINT32_T:
			return (float) (*((uint32_t*) READONLY_DATA [index]));
		case EEPROM_TYPE_FLOAT:
			return *((float*) READONLY_DATA [index]);
		}
	}

	return 0.0f;
}