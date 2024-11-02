// Header
#include "transmit.h"

// Includes
#include "peripherals.h"
#include "state_thread.h"

// Conversions -----------------------------------------------------------------------------------------------------------------

// Percent Values (unit %)
#define PERCENT_INVERSE_FACTOR		(255.0f / 100.0f)
#define PERCENT_TO_WORD(percent)	(uint16_t) ((percent) * PERCENT_INVERSE_FACTOR)

// Angle Values (unit Deg)
#define ANGLE_INVERSE_FACTOR		(255.0f / 360.0f)
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

	return canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
}

msg_t transmitSensorInputs (CANDriver* driver, sysinterval_t timeout)
{
	// Byte 0: APPS-1 Value
	// Byte 1: APPS-2 Value
	// Byte 2: BSE-F Value
	// Byte 3: BSE-R Value
	// Byte 4: SAS Value

	CANTxFrame frame =
	{
		.DLC	= 5,
		.IDE	= CAN_IDE_STD,
		.SID	= SENSOR_INPUTS_MESSAGE_ID,
		.data8	=
		{
			PERCENT_TO_WORD (pedals.apps1.value * 100.0f),
			PERCENT_TO_WORD (pedals.apps1.value * 100.0f),
			PERCENT_TO_WORD (pedals.apps1.value * 100.0f),
			PERCENT_TO_WORD (pedals.apps1.value * 100.0f),
			ANGLE_TO_WORD (sas.value)
		}
	};

	return canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
}