// Header
#include "transmit.h"

// Includes
#include "can_thread.h"
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

// Debug Value
#define DEBUG_INVERSE_FACTOR		(10.0f)
#define DEBUG_TO_WORD(debug)		(int16_t) ((debug) * DEBUG_INVERSE_FACTOR)

// Torque Values
#define TORQUE_INVERSE_FACTOR		(255.0f / 100.0f)
#define TORQUE_TO_WORD(torque)		(uint8_t) ((torque) * TORQUE_INVERSE_FACTOR)

// Ratio Values
#define RATIO_INVERSE_FACTOR		(255.0f)
#define RATIO_TO_WORD(ratio)		(uint8_t) ((ratio) * RATIO_INVERSE_FACTOR)

// Config Angle Values
#define CFG_ANGLE_INVERSE_FACTOR	(255.0f / 100.0f)
#define CFG_ANGLE_TO_WORD(angle)	(uint8_t) ((angle) * CFG_ANGLE_INVERSE_FACTOR)

// Config Spring Rate Values
#define CFG_SPRING_INVERSE_FACTOR	(255.0f / 1200.0f)
#define CFG_SPRING_TO_WORD(angle)	(uint8_t) ((angle) * CFG_SPRING_INVERSE_FACTOR)

// Config Height Values
#define CFG_HEIGHT_INVERSE_FACTOR	(255.0f / 500.0f)
#define CFG_HEIGHT_TO_WORD(angle)	(uint8_t) ((angle) * CFG_SPRING_INVERSE_FACTOR)

// Message IDs ----------------------------------------------------------------------------------------------------------------

#define STATUS_MESSAGE_ID				0x100
#define SENSOR_INPUT_PERCENT_MESSAGE_ID	0x600
#define DEBUG_MESSAGE_ID				0x651
#define CONFIG_0_MESSAGE_ID				0x7A0
#define CONFIG_2_MESSAGE_ID				0x7A2
#define CONFIG_3_MESSAGE_ID				0x7A3

// Message Packing ------------------------------------------------------------------------------------------------------------

// VCU Status Word 0
#define STATUS_WORD_0_VEHICLE_STATE(state)					(((uint16_t) (state))		<< 0)
#define STATUS_WORD_0_TORQUE_PLAUSIBLE(plausible)			(((uint16_t) (plausible))	<< 2)
#define STATUS_WORD_0_PEDALS_PLAUSIBLE(plausible)			(((uint16_t) (plausible))	<< 3)
#define STATUS_WORD_0_TORQUE_DERATING(derating)				(((uint16_t) (derating))	<< 4)
#define STATUS_WORD_0_EEPROM_STATE(state)					(((uint16_t) (state))		<< 5)
#define STATUS_WORD_0_CAN_PLAUSIBLE(plausible)				(((uint16_t) (plausible))	<< 7)

// VCU Status Word 1
#define STATUS_WORD_1_APPS_1_STATE(state)					(((uint16_t) (state))		<< 0)
#define STATUS_WORD_1_APPS_2_STATE(state)					(((uint16_t) (state))		<< 2)
#define STATUS_WORD_1_BSE_F_STATE(state)					(((uint16_t) (state))		<< 4)
#define STATUS_WORD_1_BSE_R_STATE(state)					(((uint16_t) (state))		<< 6)

// VCU Status Word 2
#define STATUS_WORD_2_AMK_RL_VALID(valid)					(((uint16_t) (valid))		<< 0)
#define STATUS_WORD_2_AMK_RR_VALID(valid)					(((uint16_t) (valid))		<< 1)
#define STATUS_WORD_2_AMK_FL_VALID(valid)					(((uint16_t) (valid))		<< 2)
#define STATUS_WORD_2_AMK_FR_VALID(valid)					(((uint16_t) (valid))		<< 3)
#define STATUS_WORD_2_GPS_VALID(valid)						(((uint16_t) (valid))		<< 4)

// Functions ------------------------------------------------------------------------------------------------------------------

msg_t transmitStatusMessage (CANDriver* driver, sysinterval_t timeout)
{
	// TODO(Barach): Overhaul this to incorporate all internal object states.

	// Byte 0:
	//   Bits 0 & 1: Vehicle state
	//   Bit 2: Torque plausible
	//   Bit 3: Pedals plausible
	//   Bit 4: Torque derating
	//   Bits 5 & 6: EEPROM state
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
	// Byte 2:
	//   Bit 0: AMK RL valid
	//   Bit 1: AMK RR valid
	//   Bit 2: AMK FL valid
	//   Bit 3: AMK FR valid
	//   Bit 4: GPS valid
	// Byte 3: GLV battery voltage

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
			STATUS_WORD_0_EEPROM_STATE (eeprom.state) |
			STATUS_WORD_0_TORQUE_DERATING (torqueDerating) |
			STATUS_WORD_0_CAN_PLAUSIBLE (canPlausible),
			STATUS_WORD_1_APPS_1_STATE (pedals.apps1.state) |
			STATUS_WORD_1_APPS_2_STATE (pedals.apps2.state) |
			STATUS_WORD_1_BSE_F_STATE (pedals.bseF.state) |
			STATUS_WORD_1_BSE_R_STATE (pedals.bseR.state),
			STATUS_WORD_2_AMK_RL_VALID (amkGetValidityLock (&amkRl)) |
			STATUS_WORD_2_AMK_RR_VALID (amkGetValidityLock (&amkRr)) |
			STATUS_WORD_2_AMK_FL_VALID (amkGetValidityLock (&amkFl)) |
			STATUS_WORD_2_AMK_FR_VALID (amkGetValidityLock (&amkFr)) |
			// TODO(Barach): Check GPS fix here.
			STATUS_WORD_2_GPS_VALID (gps.state == CAN_NODE_VALID),
			VOLTAGE_TO_WORD (glvBattery.value)
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

msg_t transmitDebugMessage (CANDriver* driver, sysinterval_t timeout)
{
	(void) driver;
	(void) timeout;

	// TODO(Barach): Implementation.

	// CANTxFrame frame =
	// {
	// 	.DLC	= 8,
	// 	.IDE	= CAN_IDE_STD,
	// 	.SID	= DEBUG_MESSAGE_ID,
	// 	.data16	=
	// 	{
	// 		DEBUG_TO_WORD (eepromMapGetReadonly (eepromMap->debugAddress0)),
	// 		DEBUG_TO_WORD (eepromMapGetReadonly (eepromMap->debugAddress1)),
	// 		DEBUG_TO_WORD (eepromMapGetReadonly (eepromMap->debugAddress2)),
	// 		DEBUG_TO_WORD (eepromMapGetReadonly (eepromMap->debugAddress3))
	// 	}
	// };

	// msg_t result = canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
	// if (result != MSG_OK)
	// 	canFaultCallback (result);
	// return result;

	return MSG_OK;
}

msg_t transmitConfig0Message (CANDriver* driver, sysinterval_t timeout)
{
	CANTxFrame frame =
	{
		.DLC	= 4,
		.IDE	= CAN_IDE_STD,
		.SID	= CONFIG_0_MESSAGE_ID,
		.data8	=
		{
			TORQUE_TO_WORD (drivingTorqueLimit),
			RATIO_TO_WORD (eepromMap->drivingTorqueBias),
			RATIO_TO_WORD (eepromMap->linearSasBiasMax),
			eepromMap->torqueAlgoritmIndex
		}
	};

	return canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
}

msg_t transmitConfig2Message (CANDriver* driver, sysinterval_t timeout)
{
	CANTxFrame frame =
	{
		.DLC	= 8,
		.IDE	= CAN_IDE_STD,
		.SID	= CONFIG_2_MESSAGE_ID,
		.data8	=
		{
			CFG_ANGLE_TO_WORD (eepromMap->camberFront),
			CFG_ANGLE_TO_WORD (eepromMap->camberRear),
			CFG_ANGLE_TO_WORD (eepromMap->castorFront),
			CFG_ANGLE_TO_WORD (eepromMap->castorRear),
			(eepromMap->reboundFront & 0xF) | ((eepromMap->reboundRear << 4) & 0xF0),
			(eepromMap->lowSpeedDampingFront & 0xF) | ((eepromMap->lowSpeedDampingRear << 4) & 0xF0),
			(eepromMap->highSpeedDampingFront & 0xF) | ((eepromMap->highSpeedDampingRear << 4) & 0xF0),
			eepromMap->arbStage
		}
	};

	return canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
}

msg_t transmitConfig3Message (CANDriver* driver, sysinterval_t timeout)
{
	CANTxFrame frame =
	{
		.DLC	= 8,
		.IDE	= CAN_IDE_STD,
		.SID	= CONFIG_3_MESSAGE_ID,
		.data8	=
		{
			CFG_SPRING_TO_WORD (eepromMap->springRateFront),
			CFG_SPRING_TO_WORD (eepromMap->springRateRear),
			CFG_HEIGHT_TO_WORD (eepromMap->rideHeightFront),
			CFG_HEIGHT_TO_WORD (eepromMap->rideHeightRear),
			CFG_ANGLE_TO_WORD (eepromMap->toeFront),
			CFG_ANGLE_TO_WORD (eepromMap->toeRear)
		}
	};

	return canTransmitTimeout (driver, CAN_ANY_MAILBOX, &frame, timeout);
}