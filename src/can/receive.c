// Header
#include "receive.h"

// Includes
#include "peripherals.h"

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define EEPROM_DATA_MESSAGE_ID		0x750
#define EEPROM_VALIDATE_MESSAGE_ID	0x751

// Receive Functions ----------------------------------------------------------------------------------------------------------

void handleEepromDataMessage (CANRxFrame* frame)
{
	// EEPROM Data Message: (ID 0x750)
	//   Bytes 0 to 1: Variable Address
	//   Byte 2: Variable Size
	//   Bytes 4 to 7: Variable Data
	uint16_t address	= frame->data16 [0];
	uint8_t dataCount	= frame->data8 [2];
	uint8_t* data		= frame->data8 + 4;

	// Write the changes to the EEPROM.
	mc24lc32WriteThrough (&eeprom.device, address, data, dataCount);

	// Reinitialize the peripherals to update any changes.
	peripheralsReconfigure ();
}

void handlerEepromValidateMessage (CANRxFrame* frame)
{
	// EEPROM Data Message: (ID 0x751)
	//   Byte 0:
	//     Bit 0: Valid

	bool valid = (frame->data8 [0] & 0b1) == 0b1;

	// Validate / invalidate the EEPROM for next boot.
	if (valid)
		mc24lc32Validate (&eeprom.device);
	else
	 	mc34lc32Invalidate (&eeprom.device);

	mc24lc32Write (&eeprom.device);
}

bool receiveMessage (CANRxFrame* frame)
{
	if (frame->SID == EEPROM_DATA_MESSAGE_ID)
	{
		handleEepromDataMessage (frame);
		return true;
	}

	if (frame->SID == EEPROM_VALIDATE_MESSAGE_ID)
	{
		handlerEepromValidateMessage (frame);
		return true;
	}
	
	return false;
}