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
#include "main.h"
#include "adc.h"
#include "macros.h"
#include <avr/pgmspace.h>


#define _2_56V 2470L   //Real value for my uC in mV.

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
	ADCSRA = (1<<ADEN) | (1<<ADPS2)| (1<<ADPS0);    //Enable ADC in Single Conversion mode with prescale by 32 (115.2 kHz) with interrupt
	SET(ADCSRA,ADIF);
	SET(ADCSRA,ADSC);  // Start Conversion
	loop_until_bit_is_set(ADCSRA,ADIF);          //Perform first conversion
	ADCSRA |= (1<<ADIE);
	_delay_ms(10);
}

void ADCStartConversion (void)
{
	set_sleep_mode(SLEEP_MODE_ADC);     // ADC Noise Reduction Mode
			//printf_P(PSTR("set_sleep_mode(SLEEP_MODE_ADC)\n\r"));
	sleep_mode();                       //If the ADC is enabled, a conversion starts automatically when this mode is entered
}
/*
//#define UseADCNRMode SMCR = (1<<SM0) | (1<<SE)        // ADC Noise Reduction Mode
#define UseADCNRMode SMCR = (1<<SE)        // ADC Noise Reduction Mode
#define ADCStartConversion   UseADCNRMode; sleep_cpu()  //If the ADC is enabled, a conversion starts automatically when this mode is entered
//#define ADCStartConversion   sleep_cpu()
*/
unsigned int ADC_get_Breakdown_mV (void)
{
	unsigned int Offset;
	unsigned int TempADC;
	
	printf_P(PSTR("ADC_get_Breakdown_mV\n\r"));
	
	USE_P_ADC2_N_ADC2_G_1_V_256;  //Analog inputs for offset measuring
	
		printf_P(PSTR("USE_P_ADC2_N_ADC2_G_1_V_256\n\r"));
	_delay_ms(100);
			printf_P(PSTR("_delay_ms(100)\n\r"));
	
	ADC_INIT();
	printf_P(PSTR("ADC_INIT();\n\r"));		
	//ADCStartConversion;		
	//ADCStartConversion();
	
		USE_P_ADC2_N_ADC2_G_1_V_256;
		ADCSRA = (1<<ADEN) | (1<<ADPS2)| (1<<ADPS0);    //Enable ADC in Single Conversion mode with prescale by 32 (115.2 kHz) with interrupt
		SET(ADCSRA,ADIF);
		SET(ADCSRA,ADSC);  // Start Conversion
		loop_until_bit_is_set(ADCSRA,ADIF);          //Perform first conversion
		ADCSRA |= (1<<ADIE);
		_delay_ms(10);
	
			printf_P(PSTR("ADCStartConversion\n\r"));
	Offset = ADC; //Get Offset value
	  printf("Offset: %d\n\r", Offset);
	
	USE_P_ADC3_N_ADC2_G_1_V_256;  //Analog inputs for data measuring
	_delay_ms(10);
	//ADCStartConversion();
	TempADC = ADC; //Get value
	TempADC = TempADC & 0x1FF;
	//  printf_P("TempADC: %d\n\r", TempADC);
	
	if (TempADC > Offset) TempADC = TempADC - Offset;  //Offset compensation
	else TempADC = 0;
	//  printf_P("TempADC - Offset: %d\n\r", TempADC);

	TempADC = div_round(TempADC * _2_56V, 512L);
	//  printf_P("Voltage TempADC: %d\n\r", TempADC);
	
	return TempADC;
}



unsigned int ADC_get_Leakage_mV (void)
{
	unsigned int Offset;
	unsigned int TempADC;
	
	USE_P_ADC2_N_ADC2_G_10_V_256;  //Analog inputs for offset measuring gain = 10
	_delay_ms(10);
	//ADCStartConversion();
	Offset = ADC; //Get Offset value
	//  printf_P("Offset: %d\n\r", Offset);
	
	USE_P_ADC3_N_ADC2_G_10_V_256; //Analog inputs for data measuring gain = 10
	_delay_ms(10);
	//ADCStartConversion();
	TempADC = ADC; //Get value
	TempADC = TempADC & 0x1FF;
	//  printf_P("TempADC: %d\n\r", TempADC);
	
	if (TempADC > Offset) TempADC = TempADC - Offset;  //Offset compensation
	else TempADC = 0;
	//  printf_P("TempADC - Offset: %d\n\r", TempADC);

	TempADC = div_round(TempADC * _2_56V, 512L);
	//  printf_P("Voltage TempADC: %d\n\r", TempADC);
	
	return TempADC;
}