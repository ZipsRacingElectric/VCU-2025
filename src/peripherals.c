// Header
#include "peripherals.h"

// Includes -------------------------------------------------------------------------------------------------------------------

// Includes
#include "debug.h"

// ChibiOS
#include "hal.h"

// Macros ---------------------------------------------------------------------------------------------------------------------

#if PERIPHERAL_DEBUGGING
	#define PERIPHERAL_PRINTF(format, ...) DEBUG_PRINTF("[Peripherals] " format "\r\n", ##__VA_ARGS__)
#else
	#define PERIPHERAL_PRINTF()
#endif // PERIPHERAL_DEBUGGING

// Global Peripherals ---------------------------------------------------------------------------------------------------------

mc24lc32_t	eeprom;
apps_t		apps1;
apps_t		apps2;
bse_t		bseF;
bse_t		bseR;

// Configuration --------------------------------------------------------------------------------------------------------------

static ADCConversionGroup adcGroup =
{
	.circular	=	false,
	.cr1		=	0,
	.cr2		=	0,
	.smpr1		=	(ADC_SAMPLE_480	<< ADC_SMPR1_SMP10_Pos) |		// Channel 10: 480 cycles per sample.
					(ADC_SAMPLE_480	<< ADC_SMPR1_SMP11_Pos) |		// Channel 11: 480 cycles per sample.
					(ADC_SAMPLE_480	<< ADC_SMPR1_SMP12_Pos) |		// Channel 12: 480 cycles per sample.
					(ADC_SAMPLE_480	<< ADC_SMPR1_SMP13_Pos) |		// Channel 13: 480 cycles per sample.
					(ADC_SAMPLE_480	<< ADC_SMPR1_SMP14_Pos) |		// Channel 14: 480 cycles per sample.
					(ADC_SAMPLE_480	<< ADC_SMPR1_SMP15_Pos),		// Channel 15: 480 cycles per sample.
	.smpr2		= 	(ADC_SAMPLE_480	<< ADC_SMPR2_SMP0_Pos),			// Channel 0: 480 cycles per sample.
	.htr		=	0,
	.ltr		=	0,
	.sqr1		= 	(6 << ADC_SQR1_L_Pos),							// 6 samples in the sequence
	.sqr2		=	0,
	.sqr3		=	(ADC_CHANNEL_IN10	<< ADC_SQR3_SQ1_Pos) |		// Sample 1 is APPS-1
					(ADC_CHANNEL_IN11	<< ADC_SQR3_SQ2_Pos) |		// Sample 2 is APPS-2
					(ADC_CHANNEL_IN12	<< ADC_SQR3_SQ3_Pos) |		// Sample 3 is BSE-F
					(ADC_CHANNEL_IN13	<< ADC_SQR3_SQ4_Pos) |		// Sample 4 is BSE-R
					(ADC_CHANNEL_IN14	<< ADC_SQR3_SQ5_Pos) |		// Sample 5 is the SAS
					(ADC_CHANNEL_IN0	<< ADC_SQR3_SQ6_Pos)		// Sample 6 is the GLV Battery
};

static I2CConfig i2cConfig =
{
	.op_mode		= OPMODE_I2C,
	.clock_speed	= 115200,
	.duty_cycle		= STD_DUTY_CYCLE
};

/// @brief Configuration for the board EEPROM.
static mc24lc32Config_t eepromConfig =
{
	.addr			= 0x50,
	.i2c			= &I2CD1,
	.magicString	= "VCU"
};

// Functions ------------------------------------------------------------------------------------------------------------------

void peripheralsInit ()
{
	// I2C 3 driver initialization
	if (i2cStart (&I2CD1, &i2cConfig) != MSG_OK)
		PERIPHERAL_PRINTF ("Failed to initialize I2C1.");

	// EEPROM initialization
	if (!mc24lc32Init (&eeprom, &eepromConfig))
		PERIPHERAL_PRINTF ("Failed to initialize the EEPROM.");
}