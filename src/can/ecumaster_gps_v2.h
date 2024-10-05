#ifndef ECUMASTER_GPS_V2_H
#define ECUMASTER_GPS_V2_H

// Includes
#include "can_node.h"

#define ECUMASTER_GPS_MESSAGE_COUNT 5

struct ecumasterGps
{
	CAN_NODE_FIELDS
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

extern const canNodeConfig_t ecumasterGpsConfig;

void ecumasterGpsHandler0 (CANRxFrame* frame);

void ecumasterGpsHandler1 (CANRxFrame* frame);

void ecumasterGpsHandler2 (CANRxFrame* frame);

void ecumasterGpsHandler3 (CANRxFrame* frame);

void ecumasterGpsHandler4 (CANRxFrame* frame);

#endif // ECUMASTER_GPS_V2_H