#ifndef CAN_NODE_H
#define CAN_NODE_H

// ChibiOS
#include "hal.h"

typedef void (canHandler_t) (CANRxFrame* frame);

#define CAN_NODE_FIELDS			\
	uint8_t handlerCount;		\
	uint16_t* addresses;		\
	canHandler_t** handlers;

/**
 * @brief Polymorphic base object representing a node in a CAN bus.
 * @note When deriving this struct, use the @c CAN_NODE_FIELDS macro to define
 * the first set of fields.
 */
struct canNode
{
	CAN_NODE_FIELDS
};

struct canNodeConfig
{
	uint8_t handlerCount;
	uint16_t* addresses;
	canHandler_t** handlers;
};

typedef struct canNode canNode_t;

typedef struct canNodeConfig canNodeConfig_t;

/**
 * @brief Initializes the CAN node using the given configuration.
 * @param node The node to initialize.
 * @param config The configuration to use.
 */
void canNodeInit (canNode_t* node, const canNodeConfig_t* config);

#endif // CAN_NODE_H