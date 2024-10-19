// Header
#include "ecumaster_gps_v2.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

// Functions -------------------------------------------------------------------------------------------------------------------

void ecumasterInit (ecumasterGps_t* gps, ecumasterGpsConfig_t* config)
{	
	// Set default values
	gps->speed			= 0.0f;
	gps->headingMotion	= 0.0f;
	gps->headingVehicle	= 0.0f;
	gps->xAngleRate		= 0.0f;
	gps->yAngleRate		= 0.0f;
	gps->zAngleRate		= 0.0f;
	gps->xAcceleration	= 0.0f;
	gps->yAcceleration	= 0.0f;
	gps->zAcceleration	= 0.0f;

	// Initialize the CAN node
	canNodeInit ((canNode_t*) gps, &config->nodeConfig, ecumasterReceiveHandler);
}

// Receive Functions ----------------------------------------------------------------------------------------------------------

void ecumasterGpsHandler0 (ecumasterGps_t* gps, CANRxFrame* frame);

void ecumasterGpsHandler1 (ecumasterGps_t* gps, CANRxFrame* frame);

void ecumasterGpsHandler2 (ecumasterGps_t* gps, CANRxFrame* frame);

void ecumasterGpsHandler3 (ecumasterGps_t* gps, CANRxFrame* frame);

void ecumasterGpsHandler4 (ecumasterGps_t* gps, CANRxFrame* frame);

void ecumasterGpsHandler0 (ecumasterGps_t* gps, CANRxFrame* frame)
{
	(void) gps;
	(void) frame;
}

void ecumasterGpsHandler1 (ecumasterGps_t* gps, CANRxFrame* frame)
{
	(void) gps;
	(void) frame;
}

void ecumasterGpsHandler2 (ecumasterGps_t* gps, CANRxFrame* frame)
{
	(void) gps;
	(void) frame;
}

void ecumasterGpsHandler3 (ecumasterGps_t* gps, CANRxFrame* frame)
{
	(void) gps;
	(void) frame;
}

void ecumasterGpsHandler4 (ecumasterGps_t* gps, CANRxFrame* frame)
{
	(void) gps;
	(void) frame;
}

bool ecumasterReceiveHandler (void* node, CANRxFrame* frame)
{
	ecumasterGps_t* gps = (ecumasterGps_t*) node;
	uint16_t id = frame->SID;

	// TODO(Barach): Magic constants and bad names
	if (id == gps->baseId)
	{
		ecumasterGpsHandler0 (gps, frame);
		return true;
	}
	else if (id == gps->baseId + 1)
	{
		ecumasterGpsHandler1 (gps, frame);
		return true;
	}
	else if (id == gps->baseId + 2)
	{
		ecumasterGpsHandler2 (gps, frame);
		return true;
	}
	else if (id == gps->baseId + 3)
	{
		ecumasterGpsHandler3 (gps, frame);
		return true;
	}
	else if (id == gps->baseId + 4)
	{
		ecumasterGpsHandler4 (gps, frame);
		return true;
	}

	return false;
}