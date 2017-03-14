/*
 * GccApplication1.c
 *
 * Created: 28.02.2017 21:39:34
 * Author : anton.proshutya
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "macros.h"
#include "main.h"
#include "UART_STR.h"
#include "stdio_io.h"
#include "macros.h"
#include "ADC.h"

unsigned int Counter = 0;        //Count of tested modules.

void TwoLedsBlinks (unsigned char Number)
{
	LED_RED_OFF;
	LED_GREEN_OFF;
	for (unsigned char i=0; i<Number; i++)
	{
		LED_RED_ON;
		LED_GREEN_ON;
		_delay_ms(50);
		LED_RED_OFF;
		LED_GREEN_OFF;
		_delay_ms(50);
	}
}

unsigned int AdjustedVoltage37;   //This voltage must be adjusted in form 3715 = 37.15V
#define AdjustedVoltageAddr  0x0000

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

void SaveAdjustedVoltage37 (unsigned int ucData)
{
	EEPROM_write (AdjustedVoltageAddr,ucData);
	EEPROM_write (AdjustedVoltageAddr+1,(ucData >> 8));
}

unsigned int ReadAdjustedVoltage37 (void)
{
	unsigned int ucData = 0x0000;
	ucData = charEEPROM_read(AdjustedVoltageAddr+1);
	ucData = (ucData << 8);
	ucData|= charEEPROM_read(AdjustedVoltageAddr);
	
	return ucData;
}

void ShowVoltage (unsigned int ucData)
{
	unsigned char High, low;
	High = (ucData/100);
	low = ucData - High*100;
	if (low < 10) printf_P(PSTR("%d.0%dV\n\r"), High, low);
	else printf_P(PSTR("%d.%dV\n\r"), High, low);
}

void ShowLeakage (unsigned int ucData)
{
	unsigned char High, low;
	High = (ucData/10);
	low = ucData - High*10;
	printf_P(PSTR("%d.%duA\n\r"), High, low);
}

unsigned char ModulePresenceCheck (void)
{
	if (MODULE_NOT_INSERTED) {
		printf_P(PSTR("Module isn't inserted or PE line is defective.\n\r"));
		return _false;
	}
	return _true;
}

void SendVersionLine (void)
{
	printf_P(PSTR("\n\r\n\r<<< AS-P-1-15-A tester. Program version: %s>>>\n\r"),ProgrammVer);
}


unsigned char  WaitCommand(void)
{
	if (SymbolRecived == _true){
		SymbolRecived = _false;
		printf_P(PSTR("\n\r>"));          //print '>' simbol
		putchar (InputSymbol);

		if ((InputSymbol == 'S')||(InputSymbol == 's')){  //Start
			return _true;  //make start
		}
		
		if ((InputSymbol == 'H')||(InputSymbol == 'h')){  //Help
			SendVersionLine();
			printf_P(PSTR("Help list:\n\r"));
			printf_P(PSTR("   'H' This help;\n\r"));
			printf_P(PSTR("   'S' Start testing;\n\r"));
			printf_P(PSTR("   'C' Show counter of tested modules;\n\r"));
			printf_P(PSTR("   'R' Reset counter of tested modules;\n\r"));
			printf_P(PSTR("   '+' Increment counter by +1;\n\r"));
			printf_P(PSTR("   '-' Decrement counter by -1;\n\r"));
			printf_P(PSTR("   '_' Show voltage for breakdown testing saved in EEPROM;\n\r"));
			printf_P(PSTR("   '<' Decrement voltage by 0.01V;\n\r"));
			printf_P(PSTR("   '>' Increment voltage by 0.01V.\n\r"));
			
			return _false;
		}
		
		if ((InputSymbol == 'R')||(InputSymbol == 'r')){  //Reset
			Counter = 0;
			SendVersionLine();
			printf_P(PSTR("Counter of tested modules was reseted.\n\r"));
			return _false;
		}
		
		if (InputSymbol == '+'){  //Increment
			if (Counter < 0xFFFF) Counter++;
			SendVersionLine();
			printf_P(PSTR("Counter was incremented by 1. New value: %d\n\r"), Counter);
			return _false;
		}
		
		if (InputSymbol == '-'){  //Decrement
			if (Counter > 0) Counter--;
			SendVersionLine();
			printf_P(PSTR("Counter was decremented by 1. New value: %d\n\r"), Counter);
			return _false;
		}
		
		if ((InputSymbol == 'C')||(InputSymbol == 'c')){  //Show counter
			SendVersionLine();
			printf_P(PSTR("Counter value: %d\n\r"), Counter);
			return _false;
		}
		
		if (InputSymbol == '_'){  //Show adjusted voltage
			SendVersionLine();
			if ((ReadAdjustedVoltage37() < 3500L)||(ReadAdjustedVoltage37() > 4000L)){   // If saved voltage less than 35.00V
				SaveAdjustedVoltage37(3700L); // Set defoult voltage 37.0V for the first programm start
			}
			AdjustedVoltage37 = ReadAdjustedVoltage37();
			printf_P(PSTR("Voltage for breakdown testing:"));
			ShowVoltage(AdjustedVoltage37);
			ModulePresenceCheck();
			return _false;
		}
		
		if (InputSymbol == '>') {  //Increment voltage
			SendVersionLine();
			AdjustedVoltage37 = ReadAdjustedVoltage37();
			if (AdjustedVoltage37 < 4000L) {
				AdjustedVoltage37++;
				SaveAdjustedVoltage37(AdjustedVoltage37);
			}
			printf_P(PSTR("Voltage for breakdown testing incremented by 0.01V.\n\rNew value:"));
			ShowVoltage(ReadAdjustedVoltage37());
			ModulePresenceCheck();
			return _false;
		}
		
		if (InputSymbol == '<') {  //Decrement voltage
			SendVersionLine();
			AdjustedVoltage37 = ReadAdjustedVoltage37();
			if (AdjustedVoltage37 > 3500) {
				AdjustedVoltage37--;
				SaveAdjustedVoltage37(AdjustedVoltage37);
			}
			printf_P(PSTR("Voltage for breakdown testing decremented by 0.01V.\n\rNew value:"));
			ShowVoltage(ReadAdjustedVoltage37());
			ModulePresenceCheck();
			return _false;
		}
	}
	return _false;
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

#define CalculateBreakdown(x) (unsigned int)(AdjustedVoltage37 - div_round(x * 133L, 33L*10L))   //10L - to adjust Breakdown_mV to AdjustedVoltage37
#define CalculateLeakage(x)   (unsigned int)(div_round(x * 10L, 33L))                            //10L - to improve resolution, result is uA * 10
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
	
	if (Breakdown_1to4 < 2850L) ERRCode |= 0x01;  //Shortcut or low breakdown voltage on path 1to4
	if (Breakdown_2to3 < 2850L) ERRCode |= 0x02;  //Shortcut or low breakdown voltage on path 2to3
	if (Breakdown_1to4 > 3150L) ERRCode |= 0x04;  //Broken wire or high breakdown voltage on path 1to4
	if (Breakdown_2to3 > 3150L) ERRCode |= 0x08;  //Broken wire or high breakdown voltage on path 2to3
	
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

void HWInit(void)
{
	LED_INIT;
	BUTTON_INIT;
	RELAY_INIT;
	USART_Init();
	stdio_io_init();
	ADC_INIT();
	AdjustedVoltage37 = ReadAdjustedVoltage37();   //Read saved voltage that used for Breakdown checking
}

int main(void)
{
    HWInit();
	SendVersionLine();
	TwoLedsBlinks(10);
	sei();
	
    while (1) 
    {
		while (BUTTON_IS_UNPRESSED) {if (WaitCommand() == _true) break;}  //wait while button Start will pressed or Start command will be recived
		TwoLedsBlinks(3);
		SendVersionLine();
    
		if (TestModule() == _true){
			Counter++;
			printf_P(PSTR("COUNTER: %d\n\r"), Counter);
			printf_P(PSTR("-------------------------\n\r"));
			printf_P(PSTR("TEST: Passed Successfully.\n\r"));
			LED_GREEN_ON;
		}
		else
		{
			printf_P(PSTR("-------------------------\n\r"));
			printf_P(PSTR("TEST: Failed!!!\n\r"));
			LED_RED_ON;
		}
    
		RELAY_INIT;   //set default voltage and path.
    }
}

