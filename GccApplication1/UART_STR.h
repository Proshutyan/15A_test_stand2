/*
 * UART_STR.h
 *
 * Created: 28.02.2017 23:09:26
 *  Author: anton.proshutya
 */ 


#ifndef UART_STR_H_
#define UART_STR_H_

#define BAUD 115200

extern unsigned char SymbolRecived;
extern unsigned char InputSymbol;

void USART_Init();

#endif /* UART_STR_H_ */