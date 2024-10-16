// Header
#include "ecumaster_gps_v2.h"

// Constants ------------------------------------------------------------------------------------------------------------------



// Configuration --------------------------------------------------------------------------------------------------------------

static uint16_t addresses [ECUMASTER_GPS_HANDLER_COUNT] =
{
	0x400, 0x401, 0x402, 0x403, 0x404
};

static canHandler_t* handlers [ECUMASTER_GPS_HANDLER_COUNT] =
{
	&ecumasterGpsHandler0, &ecumasterGpsHandler1,
	&ecumasterGpsHandler2, &ecumasterGpsHandler3,
	&ecumasterGpsHandler4
};

const canNodeConfig_t ecumasterGpsConfig =
{
	.handlerCount	= ECUMASTER_GPS_HANDLER_COUNT,
	.addresses		= addresses,
	.handlers		= handlers
};

// Handlers -------------------------------------------------------------------------------------------------------------------

void ecumasterGpsHandler0 (void* node, CANRxFrame* frame)
{
	(void) node;
	(void) frame;
}

void ecumasterGpsHandler1 (void* node, CANRxFrame* frame)
{
	(void) node;
	(void) frame;
}

void ecumasterGpsHandler2 (void* node, CANRxFrame* frame)
{
	(void) node;
	(void) frame;
}

void ecumasterGpsHandler3 (void* node, CANRxFrame* frame)
{
	(void) node;
	(void) frame;
}

void ecumasterGpsHandler4 (void* node, CANRxFrame* frame)
{
	(void) node;
	(void) frame;
}