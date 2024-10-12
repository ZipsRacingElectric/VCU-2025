// Header
#include "amk_inverter.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define TORQUE_REQUEST_TORQUE_FACTOR 100

// Functions ------------------------------------------------------------------------------------------------------------------

msg_t amkSendTorqueRequest (CANDriver* driver, uint16_t id, float motor1Torque, float motor2Torque, sysinterval_t timeout)
{
	// TODO(Barach): This is just for debugging, none of this is accurate to how the inverters expect data.
	int16_t torque1 = (int16_t) (motor1Torque * TORQUE_REQUEST_TORQUE_FACTOR);
	int16_t torque2 = (int16_t) (motor2Torque * TORQUE_REQUEST_TORQUE_FACTOR);

	CANTxFrame transmit =
	{
		.DLC = 4,
		.IDE = CAN_IDE_STD,
		.SID = id,
		.data16 =
		{
			torque1,
			torque2
		}
	};

	return canTransmitTimeout(driver, CAN_ANY_MAILBOX, &transmit, timeout);
}