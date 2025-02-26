#ifndef AM4096_H
#define AM4096_H

// AM4096 Device Driver -------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2025.02.25
//
// Description: I2C driver class for the AM4096 angular encoder.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/eeprom.h"

// ChibiOS
#include "hal.h"

// Datatypes ------------------------------------------------------------------------------------------------------------------

typedef enum
{
	/// @brief Indicates a hardware failure has occurred, the device is not responding.
	AM4096_STATE_FAILED = 0,
	/// @brief Indicates the device is functioning, but the measurement is invalid.
	AM4096_STATE_INVALID = 1,
	/// @brief Indicates the device is functioning and the measurement is valid.
	AM4096_STATE_READY = 3
} am4096State_t;

typedef struct
{
	/// @brief The 7-bit I2C address of the device.
	i2caddr_t addr;

	/// @brief The IC2 bus of the device.
	I2CDriver* i2c;

	/// @brief The timeout interval of a transaction, should be at least 21ms.
	sysinterval_t timeout;
} am4096Config_t;

typedef struct
{
	EEPROM_FIELDS;
	const am4096Config_t* config;
	am4096State_t state;
	uint16_t sample;
} am4096_t;

// Functions ------------------------------------------------------------------------------------------------------------------

bool am4096Init (am4096_t* am4096, const am4096Config_t* config);

/**
 * @brief Samples the position of the sensor.
 * @param am4096 The device to sample.
 * @return True if successful, false otherwise.
 */
bool am4096Sample (am4096_t* am4096);

#endif // AM4096_H