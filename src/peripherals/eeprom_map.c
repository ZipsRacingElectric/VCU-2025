// Header
#include "eeprom_map.h"

// Includes
#include "peripherals.h"

// Global Data ----------------------------------------------------------------------------------------------------------------

#define READONLY_COUNT (sizeof (READONLY_ADDRS) / sizeof (READONLY_ADDRS [0]))
static uint16_t READONLY_ADDRS [] =
{
	0x1000,
	0x1004,
	0x1008,
	0x100C,
	0x1010,
	0x1014
};

static void* READONLY_DATA [READONLY_COUNT] =
{
	&pedals.apps1.sample,
	&pedals.apps2.sample,
	&pedals.bseF.sample,
	&pedals.bseR.sample,
	&sas.sample,
	&glvBattery.sample,
};

static uint8_t READONLY_DATACOUNTS [READONLY_COUNT] =
{
	sizeof (pedals.apps1.sample),
	sizeof (pedals.apps2.sample),
	sizeof (pedals.bseF.sample),
	sizeof (pedals.bseR.sample),
	sizeof (sas.sample),
	sizeof (glvBattery.sample)
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