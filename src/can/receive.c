// Header
#include "receive.h"

// Includes
#include "peripherals.h"
#include "can/eeprom_can.h"

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define EEPROM_COMMAND_MESSAGE_ID 0x750

// Receive Functions ----------------------------------------------------------------------------------------------------------

bool receiveMessage (CANRxFrame* frame)
{
	if (frame->SID == EEPROM_COMMAND_MESSAGE_ID)
	{
		eepromHandleCanCommand (frame, &CAND1, (eeprom_t*) &virtualMemory);
		return true;
	}

	return false;
}