/*
 * test.c
 *
 * Created: 16.03.2017 22:35:48
 *  Author: anton.proshutya
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include "main.h"
#include "macros.h"
#include "adc.h"
#include "test.h"
#include "EEPROM.h"
#include "menu.h"

unsigned char ModulePresenceCheck (void)
{
	if (MODULE_NOT_INSERTED) {
		printf_P(PSTR("Module isn't inserted or PE line is defective.\n\r"));
		return _false;
	}
	return _true;
}

unsigned char BreakdownAnalyze(unsigned char Data)
{ unsigned char Code;
	
	if (Data == 0x00) return _true;
	
	printf_P(PSTR("ERROR: "));
	Code = Data & 0x03;
	if (Code != 0x00){
		printf_P(PSTR("Defective diode (low Vbr),\n\r"));
		printf_P(PSTR("Or shortcut between lines "));
		if (Code == 0x03) printf_P(PSTR("< 1 and 2 , possibly PE >"));
		if (Code == 0x01) printf_P(PSTR("< 2 and PE >"));
		if (Code == 0x02) printf_P(PSTR("< 1 and PE >"));
		printf_P(PSTR("\n\r"));
		return _false;
	}
	Code = Data & 0x0C;
	if (Code != 0x00){
		printf_P(PSTR("Defective diode (high Vbr,)\n\r"));
		printf_P(PSTR("Or break fuse, check lines "));
		if (Code == 0x0C) {printf_P(PSTR("< 1 and 2 >,\n\r"));printf_P(PSTR("Check diode soldering."));}
		if (Code == 0x04) printf_P(PSTR("< 1 >"));
		if (Code == 0x08) printf_P(PSTR("< 2 >"));
		printf_P(PSTR("\n\r"));
	}
	return _false;
}

unsigned char LeakageAnalyze(unsigned char Data)
{ unsigned char Code;
	
	if (Data == 0x00) return _true;
	

	Code = Data & 0x03;
	if (Code != 0x00){
		printf_P(PSTR("ERROR: Leakage is more than 10uA on path "));
		if (Code == 0x03) printf_P(PSTR("< 1to4 and 2to3 >"));
		if (Code == 0x01) printf_P(PSTR("< 1to4 >"));
		if (Code == 0x02) printf_P(PSTR("< 2to3 >"));
		printf_P(PSTR("\n\r"));
		return _false;
	}
	Code = Data & 0x0C;
	if (Code != 0x00){
		printf_P(PSTR("WARNING: Leakage is more than 5uA on path "));
		if (Code == 0x0C) printf_P(PSTR("< 1to4 and 2to3 >"));
		if (Code == 0x04) printf_P(PSTR("< 1to4 >"));
		if (Code == 0x08) printf_P(PSTR("< 2to3 >"));
		printf_P(PSTR("\n\r"));
	}
	return _true;
}

#define CalculateBreakdown(x) (unsigned int)(AdjustedVoltage36 - div_round(x * 133L, 33L*10L))   //10L - to adjust Breakdown_mV to AdjustedVoltage36
#define CalculateLeakage(x)   (unsigned int)(div_round(x * 10L, 33L))							 //10L - to improve resolution, result is uA * 10

unsigned char TestModule(void)
{
	unsigned int Breakdown_1to4;
	unsigned int Breakdown_2to3;
	unsigned int Leakage_1to4;
	unsigned int Leakage_2to3;
	unsigned char ERRCode;
	
	printf_P(PSTR("------------------------------------------------\n\r"));
	
	/*  Presence checking  */

	if (ModulePresenceCheck() == _false) return _false;
	
	/*  Breakdown checking  */
	// default path and voltage used in main cycle
	Breakdown_1to4 = ADC_get_Breakdown_mV();
	
	USE_2to3_PATH;
	_delay_ms(200);
	Breakdown_2to3 = ADC_get_Breakdown_mV();

	Breakdown_1to4 = CalculateBreakdown(Breakdown_1to4);
	Breakdown_2to3 = CalculateBreakdown(Breakdown_2to3);
	printf_P(PSTR(" - 1to4 Breakdown: "));
	ShowVoltage(Breakdown_1to4);
	printf_P(PSTR(" - 2to3 Breakdown: "));
	ShowVoltage(Breakdown_2to3);
	printf_P(PSTR("-------------------------\n\r"));
	
	ERRCode = 0;
	
	if (Breakdown_1to4 < 2835L) ERRCode |= 0x01;  //Shortcut or low breakdown voltage on path 1to4
	if (Breakdown_2to3 < 2835L) ERRCode |= 0x02;  //Shortcut or low breakdown voltage on path 2to3
	if (Breakdown_1to4 > 3165L) ERRCode |= 0x04;  //Broken wire or high breakdown voltage on path 1to4
	if (Breakdown_2to3 > 3165L) ERRCode |= 0x08;  //Broken wire or high breakdown voltage on path 2to3
	
	if (BreakdownAnalyze(ERRCode) == _false) return _false; //Checking Breakdown voltages and Analyze it
	
	/*   Leakage checking  */
	USE_15V;
	USE_1to4_PATH;
	_delay_ms(200);
	Leakage_1to4 = ADC_get_Leakage_mV();
	USE_2to3_PATH;
	_delay_ms(200);
	Leakage_2to3 = ADC_get_Leakage_mV();
	
	Leakage_1to4 = CalculateLeakage(Leakage_1to4);     //Calculate Leakage in uA * 10
	Leakage_2to3 = CalculateLeakage(Leakage_2to3);     //
	printf_P(PSTR(" - 1to4 Leakage: "));
	ShowLeakage(Leakage_1to4);
	printf_P(PSTR(" - 2to3 Leakage: "));
	ShowLeakage(Leakage_2to3);
	printf_P(PSTR("-------------------------\n\r"));
	
	ERRCode = 0;
	
	if (Leakage_1to4 > 100L) ERRCode |= 0x01;  //ERROR , Leakage is more than 10uA on path 1to4
	if (Leakage_2to3 > 100L) ERRCode |= 0x02;  //ERROR , Leakage is more than 10uA on path 2to3
	if (Leakage_1to4 > 50L) ERRCode |= 0x04;  //Warning, Leakage is more than 5uA on path 1to4
	if (Leakage_2to3 > 50L) ERRCode |= 0x08;  //Warning, Leakage is more than 5uA on path 2to3

	
	if (LeakageAnalyze(ERRCode) == _false) return _false; //Checking Leakage current and Analyze it
	
	return _true;
}