// Header
#include "pedals.h"

void appsUpdate (apps_t* apps, uint16_t raw)
{
	// Range check
	if (raw < apps->rawMin || raw > apps->rawMax)
		apps->valid = false;

	apps->value = (float) (raw - apps->rawMin) / (apps->rawMax - apps->rawMin);
}

void bseUpdate (bse_t* bse, uint16_t raw)
{
	// TODO(Barach): Implementation
	(void) bse;
	(void) raw;
}