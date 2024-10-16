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

#define BMS_HANDLER_COUNT		22
#define BMS_CELL_COUNT			120
#define BMS_TEMPERATURE_COUNT	50

// Datatypes ------------------------------------------------------------------------------------------------------------------

struct bms
{
	CAN_NODE_FIELDS;
	bool tractiveSystemsActive;
	float cellVoltages [BMS_CELL_COUNT];
	float temperatures [BMS_TEMPERATURE_COUNT];
};

typedef struct bms bms_t;

// Configuration --------------------------------------------------------------------------------------------------------------

extern const canNodeConfig_t bmsConfig;

// Handlers -------------------------------------------------------------------------------------------------------------------

void bmsHandlerCellVoltages0 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages8 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages16 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages24 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages32 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages40 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages48 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages56 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages64 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages72 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages80 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages88 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages96 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages104 (void* node, CANRxFrame* frame);

void bmsHandlerCellVoltages112 (void* node, CANRxFrame* frame);

void bmsHandlerTemperatures0 (void* node, CANRxFrame* frame);

void bmsHandlerTemperatures8 (void* node, CANRxFrame* frame);

void bmsHandlerTemperatures16 (void* node, CANRxFrame* frame);

void bmsHandlerTemperatures24 (void* node, CANRxFrame* frame);

void bmsHandlerTemperatures32 (void* node, CANRxFrame* frame);

void bmsHandlerTemperatures40 (void* node, CANRxFrame* frame);

void bmsHandlerTemperatures48 (void* node, CANRxFrame* frame);

#endif // BMS_H