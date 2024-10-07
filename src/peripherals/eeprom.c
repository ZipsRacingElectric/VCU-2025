// Header
#include "eeprom.h"

// Includes
#include "peripherals.h"

uint16_t* apps1Min	= (uint16_t*) (eeprom.cache + APPS_1_MIN_ADDR);
uint16_t* apps1Max	= (uint16_t*) (eeprom.cache + APPS_1_MAX_ADDR);
uint16_t* apps2Min	= (uint16_t*) (eeprom.cache + APPS_2_MIN_ADDR);
uint16_t* apps2Max	= (uint16_t*) (eeprom.cache + APPS_2_MAX_ADDR);
uint16_t* bseFMin	= (uint16_t*) (eeprom.cache + BSE_F_MIN_ADDR);
uint16_t* bseFMax	= (uint16_t*) (eeprom.cache + BSE_F_MAX_ADDR);
uint16_t* bseRMin	= (uint16_t*) (eeprom.cache + BSE_R_MIN_ADDR);
uint16_t* bseRMax	= (uint16_t*) (eeprom.cache + BSE_R_MAX_ADDR);