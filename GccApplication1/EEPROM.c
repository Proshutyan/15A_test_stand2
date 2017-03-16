/*
 * EEPROM.c
 *
 * Created: 16.03.2017 22:25:42
 *  Author: anton.proshutya
 */ 

#include <util/atomic.h>
#include "macros.h"
#include "main.h"
#include "EEPROM.h"

unsigned int AdjustedVoltage36;

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		loop_until_bit_is_clear(EECR,EEPE);  /* Wait for completion of previous write*/
		EEAR = uiAddress;         /* Set up address and Data Registers */
		EEDR = ucData;            /* Write logical one to EEMPE */
		EECR |= (1<<EEMPE);       /* Start eeprom write by setting EEPE */
		EECR |= (1<<EEPE);
	}
}

unsigned charEEPROM_read(unsigned int uiAddress)
{
	loop_until_bit_is_clear(EECR,EEPE); /* Wait for completion of previous write */
	EEAR = uiAddress; /* Set up address register */
	EECR |= (1<<EERE);/* Start eeprom read by writing EERE */
	return EEDR; /* Return data from Data Register */
}

void SaveAdjustedVoltage36 (unsigned int ucData)
{
	EEPROM_write (AdjustedVoltageAddr,ucData);
	EEPROM_write (AdjustedVoltageAddr+1,(ucData >> 8));
}

unsigned int ReadAdjustedVoltage36 (void)
{
	unsigned int ucData = 0x0000;
	ucData = charEEPROM_read(AdjustedVoltageAddr+1);
	ucData = (ucData << 8);
	ucData|= charEEPROM_read(AdjustedVoltageAddr);
	
	return ucData;
}