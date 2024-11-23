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

#define STATUS_MESSAGE_ID				0x100
#define SENSOR_INPUT_PERCENT_MESSAGE_ID	0x600
#define SENSOR_INPUT_RAW_MESSAGE_ID		0x601

// Message Packing ------------------------------------------------------------------------------------------------------------

// VCU Status Word 0
#define STATUS_WORD_0_VEHICLE_STATE(state)					(((uint16_t) (state))		<< 0)
#define STATUS_WORD_0_TORQUE_PLAUSIBLE(plausible)			(((uint16_t) (plausible))	<< 2)
#define STATUS_WORD_0_PEDALS_PLAUSIBLE(plausible)			(((uint16_t) (plausible))	<< 3)
#define STATUS_WORD_0_CAN_PLAUSIBLE(plausible)				(((uint16_t) (plausible))	<< 7)

// VCU Status Word 1
#define STATUS_WORD_1_APPS_1_PLAUSIBLE(plausible)			(((uint16_t) (plausible))	<< 0)
#define STATUS_WORD_1_APPS_1_CONFIG_PLAUSIBLE(plausible)	(((uint16_t) (plausible))	<< 1)
#define STATUS_WORD_1_APPS_2_PLAUSIBLE(plausible)			(((uint16_t) (plausible))	<< 2)
#define STATUS_WORD_1_APPS_2_CONFIG_PLAUSIBLE(plausible)	(((uint16_t) (plausible))	<< 3)
#define STATUS_WORD_1_BSE_F_PLAUSIBLE(plausible)			(((uint16_t) (plausible))	<< 4)
#define STATUS_WORD_1_BSE_F_CONFIG_PLAUSIBLE(plausible)		(((uint16_t) (plausible))	<< 5)
#define STATUS_WORD_1_BSE_R_PLAUSIBLE(plausible)			(((uint16_t) (plausible))	<< 6)
#define STATUS_WORD_1_BSE_R_CONFIG_PLAUSIBLE(plausible)		(((uint16_t) (plausible))	<< 7)

// Functions ------------------------------------------------------------------------------------------------------------------

msg_t transmitStatusMessage (CANDriver* driver, sysinterval_t timeout)
{
	// Byte 0:
	//   Bit 0 & 1: Vehicle state
	//   Bit 2: Torque plausible
	//   Bit 3: Pedals plausible
	//   Bit 7: CAN plausible
	// Byte 1:
	//   Bit 0: APPS-1 Plausible
	//   Bit 1: APPS-1 Config Plausible
	//   Bit 2: APPS-2 Plausible
	//   Bit 3: APPS-2 Config Plausible
	//   Bit 4: BSE-F Plausible
	//   Bit 5: BSE-F Config Plausible
	//   Bit 6: BSE-R Plausible
	//   Bit 7: BSE-R Config Plausible

	CANTxFrame frame =
	{
		.DLC	= 2,
		.IDE	= CAN_IDE_STD,
		.SID	= STATUS_MESSAGE_ID,
		.data8	=
		{
			STATUS_WORD_0_VEHICLE_STATE (vehicleState) |
			STATUS_WORD_0_TORQUE_PLAUSIBLE (torquePlausible) |
			STATUS_WORD_0_PEDALS_PLAUSIBLE (pedals.plausible) |
			STATUS_WORD_0_CAN_PLAUSIBLE (canPlausible),
			STATUS_WORD_1_APPS_1_PLAUSIBLE (pedals.apps1.plausible) |
			STATUS_WORD_1_APPS_1_CONFIG_PLAUSIBLE (pedals.apps1.configPlausible) |
			STATUS_WORD_1_APPS_2_PLAUSIBLE (pedals.apps2.plausible) |
			STATUS_WORD_1_APPS_2_CONFIG_PLAUSIBLE (pedals.apps2.configPlausible) |
			STATUS_WORD_1_BSE_F_PLAUSIBLE (pedals.bseF.plausible) |
			STATUS_WORD_1_BSE_F_CONFIG_PLAUSIBLE (pedals.bseF.configPlausible) |
			STATUS_WORD_1_BSE_R_PLAUSIBLE (pedals.bseR.plausible) |
			STATUS_WORD_1_BSE_R_CONFIG_PLAUSIBLE (pedals.bseR.configPlausible)
		}
	};

	msg_t result = canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
	if (result != MSG_OK)
		canFaultCallback (result);
	return result;
}

msg_t transmitSensorInputPercent (CANDriver* driver, sysinterval_t timeout)
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
		.SID	= SENSOR_INPUT_PERCENT_MESSAGE_ID,
		.data8	=
		{
			apps1Word & 0xFF,
			((apps2Word << 4) & 0xF0) | ((apps1Word >> 8) & 0x0F),
			(apps2Word >> 4) & 0xFF,
			(bseFWord & 0xFF),
			((bseRWord << 4) & 0xF0) | ((bseFWord >> 8) & 0x0F),
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

msg_t transmitSensorInputRaw (CANDriver* driver, sysinterval_t timeout)
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

	uint16_t apps1Word	= pedals.apps1.valueRaw;
	uint16_t apps2Word	= pedals.apps2.valueRaw;
	uint16_t bseFWord	= pedals.bseF.valueRaw;
	uint16_t bseRWord	= pedals.bseR.valueRaw;
	int16_t sasWord		= sas.valueRaw;

	CANTxFrame frame =
	{
		.DLC	= 8,
		.IDE	= CAN_IDE_STD,
		.SID	= SENSOR_INPUT_RAW_MESSAGE_ID,
		.data8	=
		{
			apps1Word & 0xFF,
			((apps2Word << 4) & 0xF0) | ((apps1Word >> 8) & 0x0F),
			(apps2Word >> 4) & 0xFF,
			(bseFWord & 0xFF),
			((bseRWord << 4) & 0xF0) | ((bseFWord >> 8) & 0x0F),
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