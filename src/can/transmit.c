// Header
#include "transmit.h"

// Includes
#include "can.h"
#include "peripherals.h"
#include "state_thread.h"
#include "torque_thread.h"

// C Standard Library
#include <string.h>

// Conversions -----------------------------------------------------------------------------------------------------------------

// Percent Values (unit %)
#define PERCENT_INVERSE_FACTOR		(4095.0f / 100.0f)
#define PERCENT_TO_WORD(percent)	(uint16_t) ((percent) * PERCENT_INVERSE_FACTOR)

// Angle Values (unit Deg)
#define ANGLE_INVERSE_FACTOR		(65535.0f / 360.0f)
#define ANGLE_TO_WORD(angle)		(int16_t) ((angle) * ANGLE_INVERSE_FACTOR)

// Voltage Value (V)
#define VOLTAGE_INVERSE_FACTOR		(255.0f / 18.0f)
#define VOLTAGE_TO_WORD(voltage)	(uint8_t) ((voltage) * VOLTAGE_INVERSE_FACTOR)

// Torque Values
#define TORQUE_INVERSE_FACTOR		(255.0f / 100.0f)
#define TORQUE_TO_WORD(torque)		(uint8_t) ((torque) * TORQUE_INVERSE_FACTOR)

// Temperature Values
#define TEMPERATURE_INVERSE_FACTOR	10.0f
#define TEMPERATURE_TO_WORD(temp)	(uint16_t) ((temp) * TEMPERATURE_INVERSE_FACTOR)

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define STATUS_MESSAGE_ID				0x100
#define SENSOR_INPUT_PERCENT_MESSAGE_ID	0x600
#define DEBUG_MESSAGE_ID				0x651
#define TEMPERATURE_MESSAGE_ID			0x7A0
#define CONFIG_MESSAGE_ID				0x7A2

// Message Packing ------------------------------------------------------------------------------------------------------------

// VCU Status Word 0
#define STATUS_WORD_0_VEHICLE_STATE(state)					(((uint8_t) (state))		<< 0)
#define STATUS_WORD_0_TORQUE_PLAUSIBLE(plausible)			(((uint8_t) (plausible))	<< 2)
#define STATUS_WORD_0_PEDALS_PLAUSIBLE(plausible)			(((uint8_t) (plausible))	<< 3)
#define STATUS_WORD_0_TORQUE_DERATING(derating)				(((uint8_t) (derating))		<< 4)
#define STATUS_WORD_0_EEPROM_STATE(state)					(((uint8_t) (state))		<< 5)
#define STATUS_WORK_0_VCU_FAULT(state)						(((uint8_t) (state))		<< 7)

// VCU Status Word 1
#define STATUS_WORD_1_APPS_1_STATE(state)					(((uint8_t) (state))		<< 0)
#define STATUS_WORD_1_APPS_2_STATE(state)					(((uint8_t) (state))		<< 2)
#define STATUS_WORD_1_BSE_F_STATE(state)					(((uint8_t) (state))		<< 4)
#define STATUS_WORD_1_BSE_R_STATE(state)					(((uint8_t) (state))		<< 6)

// VCU Status Word 2
#define STATUS_WORD_2_AMK_RL_VALID(valid)					(((uint8_t) (valid))		<< 0)
#define STATUS_WORD_2_AMK_RR_VALID(valid)					(((uint8_t) (valid))		<< 1)
#define STATUS_WORD_2_AMK_FL_VALID(valid)					(((uint8_t) (valid))		<< 2)
#define STATUS_WORD_2_AMK_FR_VALID(valid)					(((uint8_t) (valid))		<< 3)
#define STATUS_WORD_2_AMK_FAULT(state)						(((uint8_t) (state == AMK_STATE_ERROR || state == AMK_STATE_INVALID)) << 4)
#define STATUS_WORD_2_SAS_STATUS(status)					(((uint8_t) (status))		<< 6)

// Functions ------------------------------------------------------------------------------------------------------------------

msg_t transmitStatusMessage (CANDriver* driver, sysinterval_t timeout)
{
	// Byte 0:
	//   Bits 0 & 1: Vehicle state
	//   Bit 2: Torque plausible
	//   Bit 3: Pedals plausible
	//   Bit 4: Torque derating
	//   Bits 5 & 6: EEPROM state
	//   Bit 7: VCU fault
	// Byte 1:
	//   Bit 0: APPS-1 Plausible
	//   Bit 1: APPS-1 Config Plausible
	//   Bit 2: APPS-2 Plausible
	//   Bit 3: APPS-2 Config Plausible
	//   Bit 4: BSE-F Plausible
	//   Bit 5: BSE-F Config Plausible
	//   Bit 6: BSE-R Plausible
	//   Bit 7: BSE-R Config Plausible
	// Byte 2:
	//   Bit 0: AMK RL valid
	//   Bit 1: AMK RR valid
	//   Bit 2: AMK FL valid
	//   Bit 3: AMK FR valid
	//   Bits 4 AMK fault
	//   Bits 6 & 7: SAS status
	// Byte 3: GLV battery voltage

	bool amkRlValid = amkGetValidityLock (&amkRl);
	bool amkRrValid = amkGetValidityLock (&amkRr);
	bool amkFlValid = amkGetValidityLock (&amkFl);
	bool amkFrValid = amkGetValidityLock (&amkFr);

	CANTxFrame frame =
	{
		.DLC	= 4,
		.IDE	= CAN_IDE_STD,
		.SID	= STATUS_MESSAGE_ID,
		.data8	=
		{
			STATUS_WORD_0_VEHICLE_STATE (vehicleState) |
			STATUS_WORD_0_TORQUE_PLAUSIBLE (torquePlausible) |
			STATUS_WORD_0_PEDALS_PLAUSIBLE (pedals.plausible) |
			STATUS_WORD_0_TORQUE_DERATING (torqueDerating) |
			STATUS_WORD_0_EEPROM_STATE (physicalEeprom.state) |
			STATUS_WORK_0_VCU_FAULT (vcuFault),
			STATUS_WORD_1_APPS_1_STATE (pedals.apps1.state) |
			STATUS_WORD_1_APPS_2_STATE (pedals.apps2.state) |
			STATUS_WORD_1_BSE_F_STATE (pedals.bseF.state) |
			STATUS_WORD_1_BSE_R_STATE (pedals.bseR.state),
			STATUS_WORD_2_AMK_RL_VALID (amkRlValid) |
			STATUS_WORD_2_AMK_RR_VALID (amkRrValid) |
			STATUS_WORD_2_AMK_FL_VALID (amkFlValid) |
			STATUS_WORD_2_AMK_FR_VALID (amkFrValid) |
			STATUS_WORD_2_AMK_FAULT (amksState) |
			STATUS_WORD_2_SAS_STATUS (sas.state),
			VOLTAGE_TO_WORD (glvBattery.value)
		}
	};

	return canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
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

	return canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
}

msg_t transmitTemperaturesMessage (CANDriver* driver, sysinterval_t timeout)
{
	CANTxFrame frame =
	{
		.DLC	= 4,
		.IDE	= CAN_IDE_STD,
		.SID	= TEMPERATURE_MESSAGE_ID,
		.data16	=
		{
			TEMPERATURE_TO_WORD (temperatureInverterMax),
			TEMPERATURE_TO_WORD (temperatureMotorMax)
		}
	};

	return canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
}

msg_t transmitConfigMessage (CANDriver* driver, sysinterval_t timeout)
{
	CANTxFrame frame =
	{
		.DLC	= 4,
		.IDE	= CAN_IDE_STD,
		.SID	= CONFIG_MESSAGE_ID,
		.data8	=
		{
			TORQUE_TO_WORD (drivingTorqueLimit),
			TORQUE_TO_WORD (regenTorqueLimit),
			physicalEepromMap->torqueAlgoritmIndex
		}
	};

	return canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
}