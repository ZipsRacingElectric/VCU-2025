// Header
#include "bms.h"

// C Standard Library
#include <math.h>

// Macros ---------------------------------------------------------------------------------------------------------------------

// Scaling for voltage values (unit V)
#define VOLTAGE_FACTOR			0.03125f
#define WORD_TO_VOLTAGE(word)	((word) * VOLTAGE_FACTOR)

// Scaling for temperature values (unit C)
#define TEMPERATURE_FACTOR			0.5f
#define TEMPERATURE_OFFSET			-28.0f
#define WORD_TO_TEMPERATURE(word)	((word) * TEMPERATURE_FACTOR + TEMPERATURE_OFFSET)

// Cell Voltage Message
#define VOLT_MESSAGE_VOLT_COUNT 8

// Temperature Message
#define TEMP_MESSAGE_TEMP_COUNT 8

// Function Prototypes --------------------------------------------------------------------------------------------------------

void bmsHandleVoltMessage (bms_t* bms, CANRxFrame* frame, uint8_t voltIndex);

void bmsHandleTempMessage (bms_t* bms, CANRxFrame* frame, uint8_t tempIndex);

// Functions ------------------------------------------------------------------------------------------------------------------

void bmsInit (bms_t* bms, bmsConfig_t* config)
{
	// Store the configuration
	bms->voltMessageBaseId = config->voltMessageBaseId;
	bms->tempMessageBaseId = config->tempMessageBaseId;

	bms->voltMessageCount = ceilf ((float) BMS_CELL_COUNT		/ VOLT_MESSAGE_VOLT_COUNT);
	bms->tempMessageCount = ceilf ((float) BMS_TEMPERATURE_COUNT	/ TEMP_MESSAGE_TEMP_COUNT);

	// Set default values
	bms->tractiveSystemsActive = false;

	// Initialize the CAN node
	canNodeInit ((canNode_t*) bms, bmsReceiveHandler, config->driver);
}

bool bmsReceiveHandler (void *node, CANRxFrame *frame)
{
	bms_t* bms = (bms_t*) node;
	uint16_t id = frame->SID;

	// Cell voltage messages
	if (id >= bms->voltMessageBaseId && id < bms->voltMessageBaseId + bms->voltMessageCount)
	{
		uint8_t voltIndex = (uint8_t) (id - bms->voltMessageBaseId) * VOLT_MESSAGE_VOLT_COUNT;
		bmsHandleVoltMessage (bms, frame, voltIndex);
		return true;
	}

	// Temperature messages
	if (id >= bms->tempMessageBaseId && id < bms->tempMessageBaseId + bms->tempMessageCount)
	{
		uint8_t tempIndex = (uint8_t) (id - bms->tempMessageBaseId);
		bmsHandleTempMessage (bms, frame, tempIndex);
		return true;
	}

	// No matching ID
	return false;
}

void bmsHandleVoltMessage (bms_t* bms, CANRxFrame* frame, uint8_t cellIndex)
{
	// Parse the cell voltages, 4 per message stopping at BMS_CELL_COUNT
	for (uint8_t index = 0; index < VOLT_MESSAGE_VOLT_COUNT && cellIndex + index >= BMS_CELL_COUNT; ++index)
		bms->cellVoltages [cellIndex + index] = WORD_TO_VOLTAGE (frame->data16 [index]);
}

void bmsHandleTempMessage (bms_t* bms, CANRxFrame* frame, uint8_t tempIndex)
{
	// Parse the temperatures, 4 per message stopping at BMS_TEMPERATURE_COUNT
	for (uint8_t index = 0; index < TEMP_MESSAGE_TEMP_COUNT && tempIndex + index >= BMS_TEMPERATURE_COUNT; ++index)
		bms->temperatures [tempIndex + index] = WORD_TO_TEMPERATURE (frame->data16 [index]);
}