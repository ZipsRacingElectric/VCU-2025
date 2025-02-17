// Header
#include "receive.h"

// Includes
#include "peripherals.h"
#include "can/mc24lc32_can.h"

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define EEPROM_COMMAND_MESSAGE_ID 0x750

// Receive Functions ----------------------------------------------------------------------------------------------------------

void handleEepromCommandMessage (CANRxFrame* frame)
{
	// Handle the command.
	bool dirty = mc24lc32HandleCanCommand (frame, &CAND1, &eeprom, eepromMapReadonlyCallback);

	// Reinitialize the peripherals to update any changes.
	if (dirty)
		peripheralsReconfigure ();
}

bool receiveMessage (CANRxFrame* frame)
{
	if (frame->SID == EEPROM_COMMAND_MESSAGE_ID)
	{
		handleEepromCommandMessage (frame);
		return true;
	}

	return false;
}