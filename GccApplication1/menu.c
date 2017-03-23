/*
 * menu.c
 *
 * Created: 16.03.2017 22:50:40
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
#include "UART_STR.h"
#include "menu.h"

unsigned int Counter = 0;        //Count of tested modules.

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

void SendVersionLine (void)
{
	printf_P(PSTR("\n\r\n\r<<< AS-P-1-15-A tester. Program version: %s>>>\n\r"),ProgrammVer);
}

void SendDeviceVersionLine (void)
{
	printf_P(PSTR("Device version: %s\n\r"),DeviceVer);
}

void SendModuleTestPassed (void)
{
	Counter++;
	printf_P(PSTR("COUNTER: %d\n\r"), Counter);
	printf_P(PSTR("-------------------------\n\r"));
	printf_P(PSTR("TEST: Passed Successfully.\n\r"));
}

void SendTestNotPased (void)
{
	printf_P(PSTR("-------------------------\n\r"));
	printf_P(PSTR("TEST: Failed!!!\n\r"));
}

unsigned char CalibrationModulePresenceCheck (void)
{
	if (MODULE_NOT_INSERTED) {
		printf_P(PSTR("To get reliable result insert workable module!\n\r"));
		return _false;
	}
	return _true;
}

unsigned char  WaitCommand (void)
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
			printf_P(PSTR("   'V' Device and program version;\n\r"));
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
		
		if ((InputSymbol == 'V')||(InputSymbol == 'v')){  //Device and program version
			SendVersionLine();
			SendDeviceVersionLine();
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
			if ((ReadAdjustedVoltage36() < 3500L)||(ReadAdjustedVoltage36() > 4000L)){   // If saved voltage less than 35.00V
				SaveAdjustedVoltage36(3700L); // Set defoult voltage 37.0V for the first programm start
			}
			AdjustedVoltage36 = ReadAdjustedVoltage36();
			printf_P(PSTR("Voltage for breakdown testing:"));
			ShowVoltage(AdjustedVoltage36);
			CalibrationModulePresenceCheck();
			return _false;
		}
		
		if (InputSymbol == '>') {  //Increment voltage
			SendVersionLine();
			AdjustedVoltage36 = ReadAdjustedVoltage36();
			if (AdjustedVoltage36 < 4000L) {
				AdjustedVoltage36++;
				SaveAdjustedVoltage36(AdjustedVoltage36);
			}
			printf_P(PSTR("Voltage for breakdown testing incremented by 0.01V.\n\rNew value:"));
			ShowVoltage(ReadAdjustedVoltage36());
			CalibrationModulePresenceCheck();
			return _false;
		}
		
		if (InputSymbol == '<') {  //Decrement voltage
			SendVersionLine();
			AdjustedVoltage36 = ReadAdjustedVoltage36();
			if (AdjustedVoltage36 > 3350) {
				AdjustedVoltage36--;
				SaveAdjustedVoltage36(AdjustedVoltage36);
			}
			printf_P(PSTR("Voltage for breakdown testing decremented by 0.01V.\n\rNew value:"));
			ShowVoltage(ReadAdjustedVoltage36());
			CalibrationModulePresenceCheck();
			return _false;
		}
	}
	return _false;
}