// Header
#include "amk_inverter.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define TORQUE_REQUEST_ID_OFFSET 0
#define TORQUE_REQUEST_TORQUE_FACTOR 100

// Configuration --------------------------------------------------------------------------------------------------------------

static uint16_t addresses [AMK_INVERTER_HANDLER_COUNT] = {};

static canHandler_t* handlers [AMK_INVERTER_HANDLER_COUNT] = {};

static const canNodeConfig_t amkInverterConfig =
{
	.handlerCount	= AMK_INVERTER_HANDLER_COUNT,
	.addresses		= addresses,
	.handlers		= handlers
};

// Initialization -------------------------------------------------------------------------------------------------------------

void amkInit (amkInverter_t* amk, uint16_t baseId, CANDriver* driver)
{
	// Store the configuration
	amk->baseId = baseId;

	// Initialize the node
	canNodeInit ((canNode_t*) amk, &amkInverterConfig, driver);
}

// Transmit -------------------------------------------------------------------------------------------------------------------

msg_t amkSendTorqueRequest (amkInverter_t* amk, float torque1, float torque2, sysinterval_t timeout)
{
	// TODO(Barach): This is just for debugging, none of this is accurate to how the inverters expect data.
	int16_t torque1Int = (int16_t) (torque1 * TORQUE_REQUEST_TORQUE_FACTOR);
	int16_t torque2Int = (int16_t) (torque2 * TORQUE_REQUEST_TORQUE_FACTOR);

	CANTxFrame transmit =
	{
		.DLC = 4,
		.IDE = CAN_IDE_STD,
		.SID = amk->baseId + TORQUE_REQUEST_ID_OFFSET,
		.data16 =
		{
			torque1Int,
			torque2Int
		}
	};

	return canTransmitTimeout(amk->driver, CAN_ANY_MAILBOX, &transmit, timeout);
}