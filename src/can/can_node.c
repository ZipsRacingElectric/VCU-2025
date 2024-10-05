// Header
#include "can_node.h"

void canNodeInit (canNode_t* node, const canNodeConfig_t* config)
{
	node->handlerCount	= config->handlerCount;
	node->addresses		= config->addresses;
	node->handlers		= config->handlers;
}