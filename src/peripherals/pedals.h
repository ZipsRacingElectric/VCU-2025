#ifndef APPS_H
#define APPS_H

// C Standard Library
#include <stdbool.h>
#include <stdint.h>

// Datatypes ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Structure representing an accelerator pedal position sensor.
 */
struct apps
{
	uint16_t rawMin;
	uint16_t rawMax;
	float value;
	bool valid;
};

/**
 * @brief Structure representing a brake system encoder.
 */
struct bse
{
	uint16_t rawMin;
	uint16_t rawMax;
	float value;
	bool valid;
};

typedef struct apps apps_t;

typedef struct bse bse_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void appsUpdate (apps_t* apps, uint16_t raw);

void bseUpdate (bse_t* bse, uint16_t raw);

#endif // APPS_H