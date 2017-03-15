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

//#define _2_56V 2470L //  //Real value for the first device
#define _2_56V 2463L   //Real value for my uC in mV.

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

unsigned int ADC_get_Breakdown_mV (void)
{
	unsigned int Offset;
	unsigned int TempADC;
	
	USE_P_ADC2_N_ADC2_G_1_V_256;  //Analog inputs for offset measuring
	_delay_ms(10);
	ADCStartConversion();
	Offset = ADC; //Get Offset value
	  //printf_P(PSTR("\n\r***************\n\rOffset: DEC=%d, HEX=%x\n\r"), Offset, Offset);
	
	USE_P_ADC3_N_ADC2_G_1_V_256;  //Analog inputs for data measuring
	_delay_ms(10);
	ADCStartConversion();
	TempADC = ADC; //Get value
	TempADC = TempADC & 0x1FF;
	 //printf_P(PSTR("TempADC: DEC=%d, HEX=%x\n\r"), TempADC,TempADC);
	
	if (TempADC > Offset) TempADC = TempADC - Offset;  //Offset compensation
	else TempADC = 0;
	  //printf_P(PSTR("TempADC - Offset: DEC=%d, HEX=%x\n\r"), TempADC, TempADC);

	TempADC = div_round(TempADC * _2_56V, 512L);
	  //printf_P(PSTR("Voltage TempADC: %d\n\r***************\n\r\n\r"), TempADC);
	
	return TempADC;
}



unsigned int ADC_get_Leakage_mV (void)
{
	unsigned int Offset;
	unsigned int TempADC;
	
	USE_P_ADC2_N_ADC2_G_10_V_256;  //Analog inputs for offset measuring gain = 10
	_delay_ms(10);
	ADCStartConversion();
	Offset = ADC; //Get Offset value
	//  printf_P(PSTR("\n\r***************\n\rOffset: DEC=%d, HEX=%x\n\r"), Offset, Offset);
	
	USE_P_ADC3_N_ADC2_G_10_V_256; //Analog inputs for data measuring gain = 10
	_delay_ms(10);
	ADCStartConversion();
	TempADC = ADC; //Get value
	TempADC = TempADC & 0x1FF;
	//  printf_P(PSTR("TempADC: DEC=%d, HEX=%x\n\r"), TempADC,TempADC);
	
	if (TempADC > Offset) TempADC = TempADC - Offset;  //Offset compensation
	else TempADC = 0;
	//  printf_P(PSTR("TempADC - Offset: DEC=%d, HEX=%x\n\r"), TempADC, TempADC);

	TempADC = div_round(TempADC * _2_56V, 512L);
	//  printf_P(PSTR("Voltage TempADC: %d\n\r***************\n\r\n\r"), TempADC);
	
	return TempADC;
}