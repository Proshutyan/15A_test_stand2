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
#include "ADC.h"
#include "EEPROM.h"
#include "test.h"
#include "menu.h"


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

void HWInit(void)
{
	LED_INIT;
	BUTTON_INIT;
	RELAY_INIT;
	USART_Init();
	stdio_io_init();
	ADC_INIT();
	AdjustedVoltage36 = ReadAdjustedVoltage36();   //Read saved voltage that used for Breakdown checking
}

int main(void)
{
    HWInit();
	SendVersionLine();
	SendDeviceVersionLine();
	TwoLedsBlinks(10);
	sei();
	
    while (1) 
    {
		while (BUTTON_IS_UNPRESSED) {if (WaitCommand() == _true) break;}  //wait while button Start will pressed or Start command will be recived
		TwoLedsBlinks(3);
		SendVersionLine();
    
		if (TestModule() == _true){
			SendModuleTestPassed();
			LED_GREEN_ON;
		}
		else
		{
			SendTestNotPased();
			LED_RED_ON;
		}
    
		RELAY_INIT;   //set default voltage and path.
    }
}

