// Header
#include "torque_vectoring.h"

// Global Data ----------------------------------------------------------------------------------------------------------------

float torqueRequestLimit;

// Functions ------------------------------------------------------------------------------------------------------------------

void torqueVectoringInit ()
{
	// TODO(Barach): Load default from EEPROM, expose changes via CAN.
	torqueRequestLimit = TORQUE_REQUEST_MAX;
}