// Header
#include "eeprom_map.h"

// Includes
#include "peripherals.h"
#include "torque_thread.h"

// C Standard Library
#include <string.h>

// Global Data ----------------------------------------------------------------------------------------------------------------

#define READONLY_COUNT (sizeof (READONLY_ADDRS) / sizeof (READONLY_ADDRS [0]))
static const uint16_t READONLY_ADDRS [] =
{
	0x000,
	0x002,
	0x004,
	0x006,
	0x008,
	0x00A,
	0x00C,
	0x010,
	0x014,
	0x018
};

static const void* READONLY_DATA [READONLY_COUNT] =
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

static const uint16_t READONLY_SIZES [READONLY_COUNT] =
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

bool eepromReadonlyRead (void* object, uint16_t addr, void* data, uint16_t dataCount)
{
	(void) object;

	for (uint16_t index = 0; index < READONLY_COUNT; ++index)
	{
		if (addr != READONLY_ADDRS [index])
			continue;

		if (dataCount != READONLY_SIZES [index])
			return false;

		memcpy (data, READONLY_DATA [index], dataCount);
		return true;
	}

	return false;
}

bool eepromWriteonlyWrite (void* object, uint16_t addr, const void* data, uint16_t dataCount)
{
	// TODO(Barach): Implementation.
	(void) object;
	(void) addr;
	(void) data;
	(void) dataCount;

	return false;
}