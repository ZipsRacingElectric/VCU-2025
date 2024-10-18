// Header
#include "amk_inverter.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

// Message IDs
#define MOTOR_REQUEST_ID_OFFSET 0
#define MOTOR_FEEDBACK_ID_OFFSET 1

// Scaling for torque values (unit Nm)
#define TORQUE_FACTOR			0.0098f
#define TORQUE_INVERSE_FACTOR	102.040816326530f
#define TORQUE_TO_WORD(torque)	(int16_t) ((torque) * TORQUE_INVERSE_FACTOR)
#define WORD_TO_TORQUE(word)	((word) * TORQUE_FACTOR)

// AMK Control word
#define CONTROL_WORD_INVERTER_ON(bit)	(((uint16_t) (bit)) << 8)
#define CONTROL_WORD_DC_ON(bit)			(((uint16_t) (bit)) << 9)
#define CONTROL_WORD_ENABLE(bit)		(((uint16_t) (bit)) << 10)
#define CONTROL_WORD_ERROR_RESET(bit)	(((uint16_t) (bit)) << 11)

// AMK Status word


// Functions ------------------------------------------------------------------------------------------------------------------

void amkInit (amkInverter_t* amk, amkInverterConfig_t* config)
{
	// Store the configuration
	amk->baseId = config->baseId;

	// Initialize the node
	canNodeInit ((canNode_t*) amk, amkReceiveHandler, config->driver);
}

// Transmit Functions ---------------------------------------------------------------------------------------------------------

msg_t amkSendMotorRequest (amkInverter_t* amk, bool inverterEnabled, bool dcEnabled, bool driverEnabled, float torqueRequest,
	float torqueLimitPositive, float torqueLimitNegative, sysinterval_t timeout)
{
	// Motor Request Message: (ID Offset 0x000)
	//   Bytes 0 to 1: Control word (uint16_t).
	//     Bits 0 to 7: Reserved
	//     Bit 8: Inverter enable
	//     Bit 9: DC enable
	//     Bit 10: Driver enable
	//     Bit 11: Error reset
	//     Bits 12 to 15: Reserved
	//   Bytes 2 to 3: Torque setpoint (int16_t).
	//     0.1% of rated torque (0.0098 Nm / LSB)
	//   Bytes 4 to 5: Positive torque limit (int16_t).
	//     0.1% of rated torque (0.0098 Nm / LSB)
	//   Bytes 6 to 7: Negative torque limit (int16_t).
	//     0.1% of rated torque (0.0098 Nm / LSB)

	uint16_t controlWord = CONTROL_WORD_INVERTER_ON(inverterEnabled) | CONTROL_WORD_DC_ON(dcEnabled) |
		CONTROL_WORD_ENABLE(driverEnabled);
	int16_t torqueRequestInt		= TORQUE_TO_WORD (torqueRequest);
	int16_t torqueLimitPositiveInt	= TORQUE_TO_WORD (torqueLimitPositive);
	int16_t torqueLimitNegativeInt	= TORQUE_TO_WORD (torqueLimitNegative);

	CANTxFrame transmit =
	{
		.DLC = 8,
		.IDE = CAN_IDE_STD,
		.SID = amk->baseId + MOTOR_REQUEST_ID_OFFSET,
		.data16 =
		{
			controlWord,
			torqueRequestInt,
			torqueLimitPositiveInt,
			torqueLimitNegativeInt
		}
	};

	return canTransmitTimeout(amk->driver, CAN_ANY_MAILBOX, &transmit, timeout);
}

// Receive Functions ----------------------------------------------------------------------------------------------------------

void amkHandleMotorFeedback (amkInverter_t* amk, CANRxFrame* frame);

void amkHandleMotorFeedback (amkInverter_t* amk, CANRxFrame* frame)
{
	// TODO(Barach): Implementation
	(void) amk;
	(void) frame;
}

bool amkReceiveHandler (void* node, CANRxFrame* frame)
{
	amkInverter_t* amk = (amkInverter_t*) node;
	uint16_t id = frame->SID;

	// Motor feedback message
	if (id == amk->baseId + MOTOR_FEEDBACK_ID_OFFSET)
	{
		amkHandleMotorFeedback (amk, frame);
		return true;
	}

	// No matching ID
	return false;
}