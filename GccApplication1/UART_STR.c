/*
 * UART_STR.c
 *
 * Created: 28.02.2017 23:09:03
 *  Author: anton.proshutya
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "stdio_io.h"
#include "UART_STR.h"
#include "main.h"
#include <util/setbaud.h>
#include "macros.h"
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

unsigned char SymbolRecived = _false;
unsigned char InputSymbol;

void USART_Init(void)
{
	//Set baud rate (defined in UART_STR.h)
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	#if USE_2X
		UCSR0A |= (1 << U2X0);
	#else
		UCSR0A &= ~(1 << U2X0);
	#endif
	//Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Set frame format: 8data, 1stop bit, non-parity
	UCSR0C = (3<<UCSZ00);
	_setH(UCSR0B,RXCIE0); //Enable interrupt when word are received 
}


ISR(USART0_RX_vect)
{
	InputSymbol = UDR0;
	SymbolRecived = _true;
};


int stdio_io_putchar(char data)
{
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = data;
	_delay_ms(1);
	return 0;
}
