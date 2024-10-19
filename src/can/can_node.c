// Header
#include "can_node.h"

// Functions ------------------------------------------------------------------------------------------------------------------

void canNodeInit (canNode_t* node, canNodeConfig_t* config, canReceiveHandler_t* receiveHandler)
{
	// Store the configuration
	node->driver			= config->driver;
	node->timeoutHandler	= config->timeoutHandler;
	node->timeoutPeriod		= config->timeoutPeriod;
	
	// Store the receive handler
	node->receiveHandler = receiveHandler;
	
	canNodeResetTimeout (node);
}

void canNodeCheckTimeout (canNode_t* node, systime_t timeCurrent)
{
	if (node->timeoutDeadline <= timeCurrent)
	{
		node->timedOut = true;

		// If a timeout handler is specified, call it.
		if (node->timeoutHandler != NULL)
			node->timeoutHandler (node);
	}
}

void canNodeResetTimeout (canNode_t* node)
{
	// Postpone the deadline
	systime_t timeCurrent = chVTGetSystemTime ();
	node->timeoutDeadline = chTimeAddX (timeCurrent, node->timeoutPeriod);
	
	// Clear the timeout flag
	node->timedOut = false;
}

bool canNodesHandleReceive(canNode_t** nodes, uint8_t nodeCount, CANRxFrame *frame)
{
	// Call each node's handler until the correct one is found.
	for (uint8_t index = 0; index < nodeCount; ++index)
		if (nodes [index]->receiveHandler (nodes [index], frame))
			return true;

	return false;
}

void canNodesCheckTimeout(canNode_t** nodes, uint8_t nodeCount)
{
	systime_t timeCurrent = chVTGetSystemTime ();

	for (uint8_t index = 0; index < nodeCount; ++index)
		canNodeCheckTimeout (nodes [index], timeCurrent);
}