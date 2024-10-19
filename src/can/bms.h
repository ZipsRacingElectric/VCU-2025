#ifndef BMS_H
#define BMS_H

// Battery Management System CAN Node -----------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.16
// 
// Description: Object representing the BMS CAN node.
//
// To do:
// - Handler implementations.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define BMS_CELL_COUNT			144
#define BMS_TEMPERATURE_COUNT	60

// Datatypes ------------------------------------------------------------------------------------------------------------------

struct bmsConfig
{
	canNodeConfig_t nodeConfig;
};

typedef struct bmsConfig bmsConfig_t;

struct bms
{
	CAN_NODE_FIELDS;
	
	bool tractiveSystemsActive;
	
	float cellVoltages [BMS_CELL_COUNT];
	float temperatures [BMS_TEMPERATURE_COUNT];

	uint16_t voltMessageBaseId;
	uint16_t tempMessageBaseId;
	uint8_t voltMessageCount;
	uint8_t tempMessageCount;
};

typedef struct bms bms_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void bmsInit (bms_t* bms, bmsConfig_t* config);

bool bmsReceiveHandler (void* node, CANRxFrame* frame);

#endif // BMS_H