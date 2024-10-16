#ifndef MISC_NODE_H
#define MISC_NODE_H

// Miscellaneous CAN Node -----------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.10.05
// 
// Description: Group of CAN message handles not related to a specific device.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "can_node.h"

// Constants ------------------------------------------------------------------------------------------------------------------

#define MISC_HANDLER_COUNT 1

// Datatypes ------------------------------------------------------------------------------------------------------------------

struct misc
{
	CAN_NODE_FIELDS;
};

typedef struct misc misc_t;

// Configuration --------------------------------------------------------------------------------------------------------------

extern const canNodeConfig_t miscConfig;

// Handlers -------------------------------------------------------------------------------------------------------------------

void miscHandleEepromWrite (void* node, CANRxFrame* frame);

#endif // MISC_NODE_H