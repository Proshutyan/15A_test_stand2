/*
 * ADC.c
 *
 * Created: 03.03.2017 21:18:47
 *  Author: anton.proshutya
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "main.h"
#include "adc.h"
#include "macros.h"

#define USE_P_ADC3_N_ADC2_G_10_V_256     ADMUX = (1<<REFS1) | (1<<REFS0) | (0<<MUX4) | (1<<MUX3) | (1<<MUX2) | (0<<MUX1) | (1<<MUX0)
//  //Select Internal 2.56V Voltage Reference EXternal bypass and enable RIGHT-Adjust
//  //and select 01101 -   posipive: ADC3    negative: ADC2   Gain: 10x
#define USE_P_ADC2_N_ADC2_G_10_V_256     ADMUX = (1<<REFS1) | (1<<REFS0) | (0<<MUX4) | (1<<MUX3) | (1<<MUX2) | (0<<MUX1) | (0<<MUX0)
//  //Select Internal 2.56V Voltage Reference EXternal bypass and enable RIGHT-Adjust
//  //and select 01100 -   posipive: ADC2    negative: ADC2   Gain: 10x

#define USE_P_ADC3_N_ADC2_G_1_V_256     ADMUX = (1<<REFS1) | (1<<REFS0) | (1<<MUX4) | (1<<MUX3) | (0<<MUX2) | (1<<MUX1) | (1<<MUX0)
//  //Select Internal 2.56V Voltage Reference EXternal bypass and enable RIGHT-Adjust
//  //and select 11011 -   posipive: ADC3    negative: ADC2   Gain: 1x
#define USE_P_ADC2_N_ADC2_G_1_V_256     ADMUX = (1<<REFS1) | (1<<REFS0) | (1<<MUX4) | (1<<MUX3) | (0<<MUX2) | (1<<MUX1) | (0<<MUX0)
//  //Select Internal 2.56V Voltage Reference EXternal bypass and enable RIGHT-Adjust
//  //and select 11010 -   posipive: ADC3    negative: ADC2   Gain: 1x

void ADC_INIT (void)
{
	ADCSRA = 0x00;  //Swt off ADC
	_delay_ms(10);
	USE_P_ADC2_N_ADC2_G_1_V_256;
	ADCSRA = (1<<ADEN) | (1<<ADPS2)| (1<<ADPS0);    //Enable ADC in Single Conversion mode with prescale by 32 (115.2 kHz from F_CPU=3686400)  with interrupt
	SET(ADCSRA,ADIF);
	SET(ADCSRA,ADSC);  // Start Conversion
	loop_until_bit_is_set(ADCSRA,ADIF);          //Perform first conversion
	ADCSRA |= (1<<ADIE);
	_delay_ms(10);
}

ISR(ADC_vect)
{
	asm("nop");
};

void ADCStartConversion (void)
{
	set_sleep_mode(SLEEP_MODE_ADC);     // ADC Noise Reduction Mode
	sleep_mode();                       //If the ADC is enabled, a conversion starts automatically when this mode is entered
}

/*
Correlation between Input Voltage and Output Codes in the Differential Measurement

VADCm + VREF/GAIN            0x1FF 511
VADCm + 0.999 VREF/GAIN      0x1FF 511
VADCm + 0.998 VREF/GAIN      0x1FE 510
... ... ...
VADCm + 0.001 VREF/GAIN      0x001 1
VADCm 0x000 0
VADCm - 0.001 VREF/GAIN      0x3FF -1
... ... ...
VADCm - 0.999 VREF/GAIN      0x201 -511
VADCm - VREF/GAIN            0x200 -512


0x3FF + 0xFC00 = 0xFFFF - it is -1 for int16_t

*/

unsigned int ADC_get_Breakdown_mV (void)
{
	int16_t  Offset;
	int16_t  TempADC;
	
	USE_P_ADC2_N_ADC2_G_1_V_256;  //Analog inputs for offset measuring
	_delay_ms(10);
	ADCStartConversion();
	if (ADC > 0x1ff) Offset = ADC + 0xFC00;  //If offset is negative make Offset negative by adding 0xFC00
	else Offset = ADC;                       //Get Offset value
	  //printf_P(PSTR("\n\r***************\n\rOffset: DEC=%d, HEX=%x\n\r"), (int16_t)Offset, Offset);
	
	USE_P_ADC3_N_ADC2_G_1_V_256;  //Analog inputs for data measuring
	_delay_ms(10);
	ADCStartConversion();
	if (ADC > 0x1ff) TempADC = ADC + 0xFC00;  //If offset is negative make TempADC negative by adding 0xFC00
	else TempADC = ADC;                       //Get Offset value
	  //printf_P(PSTR("TempADC: DEC=%d, HEX=%x\n\r"), TempADC,TempADC);
	
	TempADC = TempADC - Offset;
	if (TempADC < 0) TempADC = 0;  //If Offset bigger than TempADC to avoid negative results when zero-voltage
	  //printf_P(PSTR("TempADC - Offset: DEC=%d, HEX=%x\n\r"), TempADC, TempADC);

	TempADC = div_round(TempADC * _2_56V, 512L);
	  //printf_P(PSTR("Voltage TempADC: %d\n\r***************\n\r\n\r"), TempADC);
	
	return TempADC;
}



unsigned int ADC_get_Leakage_mV (void)
{
	int16_t  Offset;
	int16_t  TempADC;
	
	USE_P_ADC2_N_ADC2_G_10_V_256;  //Analog inputs for offset measuring gain = 10
	_delay_ms(10);
	ADCStartConversion();
	if (ADC > 0x1ff) Offset = ADC + 0xFC00;  //If offset is negative make Offset negative by adding 0xFC00
	else Offset = ADC;                       //Get Offset value
	  //printf_P(PSTR("\n\r***************\n\rOffset: DEC=%d, HEX=%x\n\r"), (int16_t)Offset, Offset);
	
	USE_P_ADC3_N_ADC2_G_10_V_256; //Analog inputs for data measuring gain = 10
	_delay_ms(10);
	ADCStartConversion();
	if (ADC > 0x1ff) TempADC = ADC + 0xFC00;  //If offset is negative make TempADC negative by adding 0xFC00
	else TempADC = ADC;                       //Get Offset value	
	  //printf_P(PSTR("TempADC: DEC=%d, HEX=%x\n\r"), TempADC,TempADC);
	
	TempADC = TempADC - Offset;
	if (TempADC < 0) TempADC = 0;  //If Offset bigger than TempADC to avoid negative results when zero-leakage
	  //printf_P(PSTR("TempADC - Offset: DEC=%d, HEX=%x\n\r"), TempADC, TempADC);

	TempADC = div_round(TempADC * _2_56V, 512L);
	  //printf_P(PSTR("Voltage TempADC: %d\n\r***************\n\r\n\r"), TempADC);
	
	return TempADC;
}