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

/// @brief Datatype representing a CAN node's message receive event handler. A node should check the ID of the received frame
/// to determine whether or not the message came from it. The handler should return true to indicate the message was recognized
/// and handled, and return false otherwise.
typedef bool (canReceiveHandler_t) (void* node, CANRxFrame* frame);

#define CAN_NODE_FIELDS				\
	CANDriver*		driver;			\
	canReceiveHandler_t*	handler

/**
 * @brief Polymorphic base object representing a node in a CAN bus.
 * @note When deriving this struct, use the @c CAN_NODE_FIELDS macro to define
 * the first set of fields.
 */
struct canNode
{
	CAN_NODE_FIELDS;
};

typedef struct canNode canNode_t;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the CAN node using the given configuration.
 * @param node The node to initialize.
 * @param handler The node's receive handler.
 * @param driver The driver for the bus this node is connected to.
 */
void canNodeInit (canNode_t* node, canReceiveHandler_t* handler, CANDriver* driver);

#endif // CAN_NODE_H