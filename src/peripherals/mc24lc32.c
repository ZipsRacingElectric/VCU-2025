// Header
#include "mc24lc32.h"

// ChibiOS
#include "hal_i2c.h"

// C Standard Library
#include <string.h>

/// @brief Maximum number of bytes to be written in a single operation.
#define PAGE_SIZE 32

bool mc24lc32Init(mc24lc32_t *mc24lc32, mc24lc32Config_t *config)
{
	// Store the driver configuration
	mc24lc32->config = config;

	// Read the EEPROM contents into memory
	return mc24lc32Read (mc24lc32);
}

bool mc24lc32Read (mc24lc32_t* mc24lc32)
{
	// Acquire the I2C bus
	i2cAcquireBus(mc24lc32->config->bus);

	// Perform a sequential read starting at address 0 (section 8.3)
	uint8_t transmit [2] = { 0x00, 0x00 };
	msg_t result = i2cMasterTransmit (mc24lc32->config->bus, mc24lc32->config->addr, transmit, 2, mc24lc32->cache, MC24LC32_SIZE);
	
	// Release the bus
	i2cReleaseBus (mc24lc32->config->bus);
	
	// If the transaction failed, exit early
	if (result != MSG_OK)
		return false;

	// Check the validity of the memory
	return mc24lc32IsValid (mc24lc32);
}

bool mc24lc32Write (mc24lc32_t* mc24lc32)
{
	// Acquire the I2C bus
	i2cAcquireBus (mc24lc32->config->bus);

	// Memory allocation
	uint8_t transmit [PAGE_SIZE + 2];
	msg_t result;

	// Determine the number of page writes required
	uint8_t pageCount = MC24LC32_SIZE / PAGE_SIZE;
	for (uint8_t pageIndex = 0; pageIndex < pageCount; ++pageIndex)
	{
		uint16_t address = pageIndex * PAGE_SIZE;

		// Transactions starts with address (HI and LO bytes)
		transmit [0] = (uint8_t) (address) >> 8;
		transmit [1] = (uint8_t) (address);

		// Max of 32 bytes follow
		memcpy(transmit + 2, mc24lc32->cache + address, PAGE_SIZE);
		
		// Perform the transcation, exit early if an operation fails
		result = i2cMasterTransmit (mc24lc32->config->bus, mc24lc32->config->addr, transmit, PAGE_SIZE + 2, NULL, 0);
		if (result != MSG_OK)
			return false;
	}

	return true;
}

bool mc24lc32IsValid (mc24lc32_t* mc24lc32)
{
	// Check the magic string is correct (including terminator)
	uint8_t stringSize = strlen (mc24lc32->config->magicString) + 1;
	return strncmp ((const char*) mc24lc32->cache, mc24lc32->config->magicString, stringSize) == 0;
}

void mc24lc32Validate (mc24lc32_t* mc24lc32)
{
	// Copy the magic string into the beginning of memory (including terminator)
	uint8_t stringSize = strlen (mc24lc32->config->magicString) + 1;
	memcpy (mc24lc32->cache, mc24lc32->config->magicString, stringSize);
}