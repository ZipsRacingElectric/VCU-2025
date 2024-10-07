#ifndef EEPROM_H
#define EEPROM_H

// C Standard Library
#include <stdint.h>

#define APPS_1_MIN_ADDR		0x0010
#define APPS_1_MAX_ADDR		0x0012
#define APPS_2_MIN_ADDR		0x0014
#define APPS_2_MAX_ADDR		0x0016
#define BSE_F_MIN_ADDR		0x0018
#define BSE_F_MAX_ADDR		0x001A
#define BSE_R_MIN_ADDR		0x001C
#define BSE_R_MAX_ADDR		0x001E

extern uint16_t* apps1Min;
extern uint16_t* apps1Max;
extern uint16_t* apps2Min;
extern uint16_t* apps2Max;
extern uint16_t* bseFMin;
extern uint16_t* bseFMax;
extern uint16_t* bseRMin;
extern uint16_t* bseRMax;

#endif // EEPROM_H