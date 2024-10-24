// Header
#include "receive.h"

// Includes
#include "peripherals.h"

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define EEPROM_MESSAGE_ID 0x750

// Message Packing ------------------------------------------------------------------------------------------------------------

// EEPROM Message
#define APPS_1_MIN_ID	0
#define APPS_1_MAX_ID	1
#define APPS_2_MIN_ID	2
#define APPS_2_MAX_ID	3
#define BSE_F_MIN_ID	4
#define BSE_F_MAX_ID	5
#define BSE_R_MIN_ID	6
#define BSE_R_MAX_ID	7

// Receive Functions ----------------------------------------------------------------------------------------------------------

void handleEepromMessage (CANRxFrame* frame)
{
	// Motor Request Message: (ID 0x750)
	//   Bytes 0 to 1: Variable ID
	//   Bytes 2 to 3: Reserved
	//   Bytes 4 to 4: Variable Data (16-bit)
	//   Bytes 4 to 7: Variable Data (32-bit) 
	//   Bytes 4 to 7: Variable Data (floating point)

	uint16_t variableId = frame->data16 [0];
	
	// TODO(Barach): This sucks.
	switch (variableId)
	{
	case APPS_1_MIN_ID:
		pedals.apps1.rawMin = frame->data16 [2];
		*eeprom.apps1Min = frame->data16 [2];
		eepromMapWriteThrough (&eeprom, &eeprom.apps1Min, sizeof (eeprom.apps1Min));
		break;
	case APPS_1_MAX_ID:
		*eeprom.apps1Max = frame->data16 [2];
		eepromMapWriteThrough (&eeprom, &eeprom.apps1Max, sizeof (eeprom.apps1Max));
		break;
	case APPS_2_MIN_ID:
		*eeprom.apps2Min = frame->data16 [2];
		eepromMapWriteThrough (&eeprom, &eeprom.apps2Min, sizeof (eeprom.apps2Min));
		break;
	case APPS_2_MAX_ID:
		*eeprom.apps2Max = frame->data16 [2];
		eepromMapWriteThrough (&eeprom, &eeprom.apps2Max, sizeof (eeprom.apps2Max));
		break;
	case BSE_F_MIN_ID:
		*eeprom.bseFMin = frame->data16 [2];
		eepromMapWriteThrough (&eeprom, &eeprom.bseFMin, sizeof (eeprom.bseFMin));
		break;
	case BSE_F_MAX_ID:
		*eeprom.bseFMax = frame->data16 [2];
		eepromMapWriteThrough (&eeprom, &eeprom.bseFMax, sizeof (eeprom.bseFMax));
		break;
	case BSE_R_MIN_ID:
		*eeprom.bseRMin = frame->data16 [2];
		eepromMapWriteThrough (&eeprom, &eeprom.bseRMin, sizeof (eeprom.bseRMin));
		break;
	case BSE_R_MAX_ID:
		*eeprom.bseRMax = frame->data16 [2];
		eepromMapWriteThrough (&eeprom, &eeprom.bseRMax, sizeof (eeprom.bseRMax));
		break;
	}
}

bool receiveMessage (CANRxFrame* frame)
{
	if (frame->SID == EEPROM_MESSAGE_ID)
	{
		handleEepromMessage (frame);
		return true;
	}
	
	return false;
}