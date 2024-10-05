#ifndef CAN_NODE_H
#define CAN_NODE_H

// CAN Node -------------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.05
//
// Description: Base object representing a node in a CAN bus. This object provides a standard interface for an object that
//   broadcasts periodic CAN messages.

// Includes -------------------------------------------------------------------------------------------------------------------

// ChibiOS
#include "hal.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

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

typedef struct canNode canNode_t;

struct canNodeConfig
{
	uint8_t handlerCount;
	uint16_t* addresses;
	canHandler_t** handlers;
};

typedef struct canNodeConfig canNodeConfig_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the CAN node using the given configuration.
 * @param node The node to initialize.
 * @param config The configuration to use.
 */
void canNodeInit (canNode_t* node, const canNodeConfig_t* config);

#endif // CAN_NODE_H