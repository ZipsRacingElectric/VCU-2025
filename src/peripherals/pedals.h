#ifndef APPS_H
#define APPS_H

// C Standard Library
#include <stdbool.h>
#include <stdint.h>

// Datatypes ------------------------------------------------------------------------------------------------------------------

enum pedalState
{
	PEDALS_READY		= 0,
};

typedef enum pedalState pedalState_t;

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

typedef struct apps apps_t;

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

typedef struct bse bse_t;

// Functions ------------------------------------------------------------------------------------------------------------------

void appsCallback (void* appsPtr, uint16_t raw);

void bseCallback (void* bsePtr, uint16_t raw);

void pedalsCheck (pedalState_t* state, apps_t* apps1, apps_t* apps2, bse_t* bseF, bse_t* bseR);

#endif // APPS_H