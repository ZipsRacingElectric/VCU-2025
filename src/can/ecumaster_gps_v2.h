#ifndef ECUMASTER_GPS_V2_H
#define ECUMASTER_GPS_V2_H

// ECUMaster GPS to CAN V2 ----------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.05
// 
// Description: Object representing the ECUMaster GPS CAN module.
//
// To do:
// - Handler implementations.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define ECUMASTER_GPS_HANDLER_COUNT 5

// Datatypes ------------------------------------------------------------------------------------------------------------------

struct ecumasterGps
{
	CAN_NODE_FIELDS;
	float speed;
	float headingMotion;
	float headingVehicle;
	float xAngleRate;
	float yAngleRate;
	float zAngleRate;
	float xAcceleration;
	float yAcceleration;
	float zAcceleration;
};

typedef struct ecumasterGps ecumasterGps_t;

// Configuration --------------------------------------------------------------------------------------------------------------

extern const canNodeConfig_t ecumasterGpsConfig;

// Handlers -------------------------------------------------------------------------------------------------------------------

void ecumasterGpsHandler0 (void* node, CANRxFrame* frame);

void ecumasterGpsHandler1 (void* node, CANRxFrame* frame);

void ecumasterGpsHandler2 (void* node, CANRxFrame* frame);

void ecumasterGpsHandler3 (void* node, CANRxFrame* frame);

void ecumasterGpsHandler4 (void* node, CANRxFrame* frame);

#endif // ECUMASTER_GPS_V2_H