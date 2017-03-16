/*
 * EEPROM.h
 *
 * Created: 16.03.2017 22:25:58
 *  Author: anton.proshutya
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

extern unsigned int AdjustedVoltage36;   //This voltage must be adjusted in form 3715 = 37.15V
#define AdjustedVoltageAddr  0x0000

void SaveAdjustedVoltage36 (unsigned int ucData);
unsigned int ReadAdjustedVoltage36 (void);

#endif /* EEPROM_H_ */