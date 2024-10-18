// Header
#include "can_node.h"

void canNodeInit (canNode_t* node, canReceiveHandler_t* handler, CANDriver* driver)
{
	node->driver	= driver;
	node->handler	= handler;
}