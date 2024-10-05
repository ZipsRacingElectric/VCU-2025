// Header
#include "ecumaster_gps_v2.h"

// Constants ------------------------------------------------------------------------------------------------------------------



// Configuration --------------------------------------------------------------------------------------------------------------

static uint16_t addresses [ECUMASTER_GPS_MESSAGE_COUNT] =
{
	0x400, 0x401, 0x402, 0x403, 0x404
};

static canHandler_t* handlers [ECUMASTER_GPS_MESSAGE_COUNT] =
{
	&ecumasterGpsHandler0, &ecumasterGpsHandler1,
	&ecumasterGpsHandler2, &ecumasterGpsHandler3,
	&ecumasterGpsHandler4
};

const canNodeConfig_t ecumasterGpsConfig =
{
	.addresses	= addresses,
	.handlers	= handlers
};

// Handlers -------------------------------------------------------------------------------------------------------------------

void ecumasterGpsHandler0 (CANRxFrame* frame)
{
	(void) frame;
}

void ecumasterGpsHandler1 (CANRxFrame* frame)
{
	(void) frame;
}

void ecumasterGpsHandler2 (CANRxFrame* frame)
{
	(void) frame;
}

void ecumasterGpsHandler3 (CANRxFrame* frame)
{
	(void) frame;
}

void ecumasterGpsHandler4 (CANRxFrame* frame)
{
	(void) frame;
}