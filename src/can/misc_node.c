// Header
#include "misc_node.h"

// Includes
#include "peripherals.h"

// Configuration --------------------------------------------------------------------------------------------------------------

static uint16_t addresses [MISC_HANDLER_COUNT] =
{
	0x100
};

static canHandler_t* handlers [MISC_HANDLER_COUNT] =
{
	&miscHandleEepromWrite
};

const canNodeConfig_t miscConfig =
{
	.handlerCount	= MISC_HANDLER_COUNT,
	.addresses		= addresses,
	.handlers		= handlers
};

// Handlers -------------------------------------------------------------------------------------------------------------------

void miscHandleEepromWrite (CANRxFrame* frame)
{
	// Bytes 0 & 1 are the address.
	uint16_t addr = frame->data8 [0] | frame->data8 [1] << 8;

	// Bytes 2 through 5 are the data.
	void* data = &frame->data8 [2];

	// Write the data to the EEPROM.
	mc24lc32WriteThrough (&eeprom, addr, data, 4);
}