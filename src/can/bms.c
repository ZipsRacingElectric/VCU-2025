// Header
#include "bms.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

// Scaling for voltage values (unit V)
#define VOLTAGE_FACTOR			0.03125f
#define WORD_TO_VOLTAGE(word)	((word) * VOLTAGE_FACTOR)

// Scaling for temperature values (unit C)
#define TEMPERATURE_FACTOR			0.5f
#define TEMPERATURE_OFFSET			-28.0f
#define WORD_TO_TEMPERATURE(word)	((word) * TEMPERATURE_FACTOR + TEMPERATURE_OFFSET)

// Cell Voltage Message
#define CELL_VOLTAGE_MESSAGE_VOLTAGE_COUNT 4

// Temperature Message
#define TEMPERATURE_MESSAGE_TEMPERATURE_COUNT 4

// Configuration --------------------------------------------------------------------------------------------------------------

static uint16_t addresses [BMS_HANDLER_COUNT] =
{
	// Cell voltage messages
	0x700, 0x701,
	0x702, 0x703,
	0x704, 0x705,
	0x706, 0x707,
	0x708, 0x709,
	0x70A, 0x70B,
	0x70C, 0x70D,
	0x70E,

	// Temperature messages
	0x710, 0x711,
	0x712, 0x713,
	0x714, 0x715,
	0x716
};

static canHandler_t* handlers [BMS_HANDLER_COUNT] =
{
	// Cell voltage messages
	&bmsHandlerCellVoltages0,	&bmsHandlerCellVoltages8,
	&bmsHandlerCellVoltages16,	&bmsHandlerCellVoltages24,
	&bmsHandlerCellVoltages32,	&bmsHandlerCellVoltages40,
	&bmsHandlerCellVoltages48,	&bmsHandlerCellVoltages56,
	&bmsHandlerCellVoltages64,	&bmsHandlerCellVoltages72,
	&bmsHandlerCellVoltages80,	&bmsHandlerCellVoltages88,
	&bmsHandlerCellVoltages96,	&bmsHandlerCellVoltages104,
	&bmsHandlerCellVoltages112,
	
	// Temperature messages
	&bmsHandlerTemperatures0,	&bmsHandlerTemperatures8,
	&bmsHandlerTemperatures16,	&bmsHandlerTemperatures24,
	&bmsHandlerTemperatures32,	&bmsHandlerTemperatures40,
	&bmsHandlerTemperatures48
};

const canNodeConfig_t bmsConfig =
{
	.handlerCount	= BMS_HANDLER_COUNT,
	.addresses		= addresses,
	.handlers		= handlers
};

// Function Prototypes --------------------------------------------------------------------------------------------------------

void handleCellVoltages (void* node, CANRxFrame* frame, uint8_t cellIndex);

void handleTemperatures (void* node, CANRxFrame* frame, uint8_t temperatureIndex);

// Handlers -------------------------------------------------------------------------------------------------------------------

void bmsHandlerCellVoltages0 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 0); }

void bmsHandlerCellVoltages8 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 8); }

void bmsHandlerCellVoltages16 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 16); }

void bmsHandlerCellVoltages24 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 24); }

void bmsHandlerCellVoltages32 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 32); }

void bmsHandlerCellVoltages40 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 40); }

void bmsHandlerCellVoltages48 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 48); }

void bmsHandlerCellVoltages56 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 56); }

void bmsHandlerCellVoltages64 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 64); }

void bmsHandlerCellVoltages72 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 72); }

void bmsHandlerCellVoltages80 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 80); }

void bmsHandlerCellVoltages88 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 88); }

void bmsHandlerCellVoltages96 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 96); }

void bmsHandlerCellVoltages104 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 104); }

void bmsHandlerCellVoltages112 (void* node, CANRxFrame* frame) { handleCellVoltages (node, frame, 112); }

void bmsHandlerTemperatures0 (void *node, CANRxFrame *frame) { handleTemperatures (node, frame, 0); }

void bmsHandlerTemperatures8 (void *node, CANRxFrame *frame) { handleTemperatures (node, frame, 8); }

void bmsHandlerTemperatures16 (void *node, CANRxFrame *frame) { handleTemperatures (node, frame, 16); }

void bmsHandlerTemperatures24 (void *node, CANRxFrame *frame) { handleTemperatures (node, frame, 24); }

void bmsHandlerTemperatures32 (void *node, CANRxFrame *frame) { handleTemperatures (node, frame, 32); }

void bmsHandlerTemperatures40 (void *node, CANRxFrame *frame) { handleTemperatures (node, frame, 40); }

void bmsHandlerTemperatures48 (void *node, CANRxFrame *frame) { handleTemperatures (node, frame, 48); }

// Functions ------------------------------------------------------------------------------------------------------------------

void handleCellVoltages (void* node, CANRxFrame* frame, uint8_t cellIndex)
{
	bms_t* bms = (bms_t*) node;
	
	// Parse the cell voltages, 4 per message stopping at BMS_CELL_COUNT
	for (uint8_t index = 0; index < CELL_VOLTAGE_MESSAGE_VOLTAGE_COUNT &&
		cellIndex + index >= BMS_CELL_COUNT; ++index)
		bms->cellVoltages [cellIndex + index] = WORD_TO_VOLTAGE (frame->data16 [index]);
}

void handleTemperatures (void* node, CANRxFrame* frame, uint8_t temperatureIndex)
{
	bms_t* bms = (bms_t*) node;
	
	// Parse the temperatures, 4 per message stopping at BMS_TEMPERATURE_COUNT
	for (uint8_t index = 0; index < CELL_VOLTAGE_MESSAGE_VOLTAGE_COUNT &&
		temperatureIndex + index >= BMS_TEMPERATURE_COUNT; ++index)
		bms->temperatures [temperatureIndex + index] = WORD_TO_TEMPERATURE (frame->data16 [index]);
}