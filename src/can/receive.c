// Header
#include "receive.h"

// Includes
#include "peripherals.h"
#include "transmit.h"

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define EEPROM_COMMAND_MESSAGE_ID							0x750

// Message Packaging ----------------------------------------------------------------------------------------------------------

// EEPROM Command Message
#define EEPROM_COMMAND_MESSAGE_READ_NOT_WRITE(word)			(((word) & 0b00000001) == 0b00000001)
#define EEPROM_COMMAND_MESSAGE_DATA_NOT_VALIDATION(word)	(((word) & 0b00000010) == 0b00000010)
#define EEPROM_COMMAND_MESSAGE_IS_VALID(word)				(((word) & 0b00000100) == 0b00000100)
#define EEPROM_COMMAND_MESSAGE_DATA_COUNT(word)				((((word) & 0b00001100) >> 2) + 1)

// Timeouts -------------------------------------------------------------------------------------------------------------------

#define EEPROM_RESPONSE_MESSAGE_TIMEOUT						TIME_MS2I (100)

// Receive Functions ----------------------------------------------------------------------------------------------------------

void handleEepromCommandMessage (CANRxFrame* frame)
{
	// EEPROM Command Message: (ID 0x750)
	//   Bytes 0 to 1: Instruction
	//     Bit 0: Read / Not Write
	//     Bit 1: Data / Not Validation
	//     Bit 2: (Validation only) Is Valid
	//     Bits 2 to 3: (Data only) Data Count - 1
	//   Bytes 2 to 3: (Data only) Address
	//   Bytes 4 to 7: (Data only) Data

	bool readNotWrite = EEPROM_COMMAND_MESSAGE_READ_NOT_WRITE (frame->data16 [0]);
	bool dataNotValidation = EEPROM_COMMAND_MESSAGE_DATA_NOT_VALIDATION (frame->data16 [0]);

	if (!dataNotValidation)
	{
		if (readNotWrite)
		{ // Validation read
			bool isValid = mc24lc32IsValid (&eeprom.device);
			transmitEepromResponseValidation (&CAND1, EEPROM_RESPONSE_MESSAGE_TIMEOUT, isValid);
		}
		else
		{ // Validation write
			bool isValid = EEPROM_COMMAND_MESSAGE_IS_VALID (frame->data16 [0]);

			// Validate / invalidate the EEPROM for next boot.
			if (isValid)
				mc24lc32Validate (&eeprom.device);
			else
				mc34lc32Invalidate (&eeprom.device);

			mc24lc32Write (&eeprom.device);
		}
	}
	else
	{
		uint8_t dataCount = EEPROM_COMMAND_MESSAGE_DATA_COUNT (frame->data16 [0]);
		uint16_t address = frame->data16 [1];

		if (readNotWrite)
		{ // Data read
			uint8_t* data = eeprom.device.cache + address;
			transmitEepromResponseData (&CAND1, EEPROM_RESPONSE_MESSAGE_TIMEOUT, address, data, dataCount);
		}
		else
		{ // Data write
			// Write the changes to the EEPROM.
			uint8_t* data = frame->data8 + 4;
			mc24lc32WriteThrough (&eeprom.device, address, data, dataCount);
		}
	}

	// Reinitialize the peripherals to update any changes.
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