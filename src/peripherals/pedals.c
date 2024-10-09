// Header
#include "pedals.h"

void appsCallback (void* appsPtr, uint16_t raw)
{
	apps_t* apps = (apps_t*) appsPtr;

	// Range check
	if (raw < apps->rawMin || raw > apps->rawMax)
		apps->valid = false;

	// apps->value = (float) (raw - apps->rawMin) / (apps->rawMax - apps->rawMin);
	apps->value = raw;
}

void bseCallback (void* bsePtr, uint16_t raw)
{
	apps_t* bse = (apps_t*) bsePtr;

	// Range check
	if (raw < bse->rawMin || raw > bse->rawMax)
		bse->valid = false;

	bse->value = (float) (raw - bse->rawMin) / (bse->rawMax - bse->rawMin);
}