// Header
#include "transmit.h"

// Includes
#include "can/can_node.h"
#include "peripherals.h"
#include "state_thread.h"

// Conversions -----------------------------------------------------------------------------------------------------------------

// Percent Values (unit %)
#define PERCENT_INVERSE_FACTOR		(4095.0f / 100.0f)
#define PERCENT_TO_WORD(percent)	(uint16_t) ((percent) * PERCENT_INVERSE_FACTOR)

// Angle Values (unit Deg)
#define ANGLE_INVERSE_FACTOR		(65535.0f / 360.0f)
#define ANGLE_TO_WORD(angle)		(int16_t) ((angle) * ANGLE_INVERSE_FACTOR)

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define STATUS_MESSAGE_ID			0x100
#define SENSOR_INPUTS_MESSAGE_ID	0x600

// Functions ------------------------------------------------------------------------------------------------------------------

msg_t transmitStatusMessage (CANDriver* driver, sysinterval_t timeout)
{
	// Byte 0
	//   Bit 0 & 1: Vehicle state
	//   Bit 2: Torque plausible
	//   Bit 3:

	CANTxFrame frame =
	{
		.DLC	= 1,
		.IDE	= CAN_IDE_STD,
		.SID	= STATUS_MESSAGE_ID,
		.data8	=
		{
			vehicleState & 0b11
		}
	};

	msg_t result = canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
	if (result != MSG_OK)
		canFaultCallback (result);
	return result;
}

msg_t transmitSensorInputs (CANDriver* driver, sysinterval_t timeout)
{
	// Byte 0:
	//   Bits 0 to 7: APPS-1 Value bits 0 to 7
	// Byte 1:
	//   Bits 0 to 3: APPS-1 Value bits 8 to 11
	//   Bits 4 to 7: APPS-2 Value bits 0 to 3
	// Byte 2:
	//   Bits 0 to 7: APPS-2 Value bits 4 to 11
	// Byte 3:
	//   Bits 0 to 7: BSE-F Value bits 0 to 7
	// Byte 4:
	//   Bits 0 to 3: BSE-F Value bits 8 to 11
	//   Bits 4 to 7: BSE-R Value bits 0 to 3
	// Byte 5:
	//   Bits 0 to 7: BSE-R Value bits 4 to 11
	// Byte 6:
	//   Bits 0 to 7: SAS Value LO byte
	// Byte 7:
	//   Bits 0 to 7: SAS Value HI byte

	uint16_t apps1Word	= PERCENT_TO_WORD (pedals.apps1.value * 100.0f);
	uint16_t apps2Word	= PERCENT_TO_WORD (pedals.apps2.value * 100.0f);
	uint16_t bseFWord	= PERCENT_TO_WORD (pedals.bseF.value * 100.0f);
	uint16_t bseRWord	= PERCENT_TO_WORD (pedals.bseR.value * 100.0f);
	int16_t sasWord		= ANGLE_TO_WORD (sas.value);

	CANTxFrame frame =
	{
		.DLC	= 8,
		.IDE	= CAN_IDE_STD,
		.SID	= SENSOR_INPUTS_MESSAGE_ID,
		.data8	=
		{
			apps1Word & 0xFF,
			((apps2Word << 4) & 0xF0) | ((apps1Word >> 4) & 0x0F),
			(apps2Word >> 4) & 0xFF,
			(bseFWord & 0xFF),
			((bseRWord << 4) & 0xF0) | ((bseFWord >> 4) & 0x0F),
			(bseRWord >> 4) & 0xFF,
			sasWord & 0xFF,
			(sasWord >> 8) & 0xFF
		}
	};

	msg_t result = canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
	if (result != MSG_OK)
		canFaultCallback (result);
	return result;
}