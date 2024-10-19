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

/**
 * @brief Event handler for receiving a CAN message. A node should check the ID of the received frame to determine whether or
 * not the message came from it. The handler should return true to indicate the message was recognized and handled, and return
 * false otherwise.
 */
typedef bool (canReceiveHandler_t) (void* node, CANRxFrame* frame);

/**
 * @brief Event handler for a CAN node timing out.
 */
typedef void (canTimeoutHandler_t) (void* node);

/**
 * @brief Configuration structure for a CAN node.
 */
struct canNodeConfig
{
	/// @brief The driver for the bus this node is connected to.
	CANDriver* driver;

	/// @brief The base CAN ID of this node. All message IDs are local offsets from this value.
	uint16_t baseId;

	/// @brief The event handler for when a timeout occurs. Use null to ignore.
	canTimeoutHandler_t* timeoutHandler;

	/// @brief The period of time to timeout after.
	sysinterval_t timeoutPeriod;
};

typedef struct canNodeConfig canNodeConfig_t;

#define CAN_NODE_FIELDS							\
	CANDriver*				driver;				\
	uint16_t				baseId;				\
	canReceiveHandler_t*	receiveHandler;		\
	canTimeoutHandler_t*	timeoutHandler;		\
	bool					timedOut;			\
	sysinterval_t			timeoutPeriod;		\
	systime_t				timeoutDeadline

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
 * @param config The configuration to use.
 * @param receiveHandler The node's receive handler.
 */
void canNodeInit (canNode_t* node, canNodeConfig_t* config, canReceiveHandler_t* receiveHandler);

/**
 * @brief Checks whether the CAN node has timed out.
 * @param node The node to use.
 * @param timeCurrent The current system time.
 */
void canNodeCheckTimeout (canNode_t* node, systime_t timeCurrent);

/**
 * @brief Resets the CAN node's timeout deadline.
 * @param node The node to use.
 */
void canNodeResetTimeout (canNode_t* node);

/**
 * @brief Finds the node that can handle a received frame and calls its handler.
 * @param nodes The array of nodes to search.
 * @param nodeCount The number of elements in the array.
 * @param frame The received frame.
 * @return True if the handler was found, false otherwise. 
 */
bool canNodesHandleReceive (canNode_t** nodes, uint8_t nodeCount, CANRxFrame* frame);

/**
 * @brief Checks whether any of the CAN nodes in an array have timed out.
 * @param nodes The array of nodes to check.
 * @param nodeCount The number of elements in the array.
 */
void canNodesCheckTimeout (canNode_t** nodes, uint8_t nodeCount);

#endif // CAN_NODE_H