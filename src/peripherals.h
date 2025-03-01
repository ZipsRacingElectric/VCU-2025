#ifndef PERIPHERALS_H
#define PERIPHERALS_H

// Peripherals ----------------------------------------------------------------------------------------------------------------
//
// Author: Cole Barach
// Date Created: 2024.09.29
//
// Description: Global objects representing the on-board hardware of the VCU.

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "peripherals/am4096.h"
#include "peripherals/analog_linear.h"
#include "peripherals/eeprom_map.h"
#include "peripherals/mc24lc32.h"
#include "peripherals/pedals.h"
#include "peripherals/stm_adc.h"

// Global Peripherals ---------------------------------------------------------------------------------------------------------

/// @brief ADC responsible for sampling all on-board analog inputs ( @c pedals & @c glvBattery ).
extern stmAdc_t adc;

/// @brief The VCU's EEPROM. This is responsible for storing all non-volatile variables.
extern mc24lc32_t eeprom;

/// @brief Structure mapping the EEPROM's contents to C datatypes.
extern eepromMap_t* eepromMap;

/// @brief Analog sensor measuring the voltage of the GLV battery.
extern linearSensor_t glvBattery;

/// @brief Analog sensors measuring the requests of the throttle and brake pedals.
extern pedals_t pedals;

/// @brief ADC measuring the steering-angle sensor.
extern am4096_t sasAdc;

/// @brief Sensor measuring the steering angle of the vehicle.
extern sas_t sas;

/// @brief The VCU's virtual memory map. TODO(Barach): Docs.
extern virtualEeprom_t virtualMemory;

// Functions ------------------------------------------------------------------------------------------------------------------

/**
 * @brief Initializes the VCU's peripherals.
 * @return False if a fatal peripheral failed to initialize, true otherwise.
 */
bool peripheralsInit (void);

/**
 * @brief Re-initializes the VCU's peripherals after a change has been made to the on-board EEPROM.
 * @param arg Ignored.
 */
void peripheralsReconfigure (void* arg);

#endif // PERIPHERALS_H