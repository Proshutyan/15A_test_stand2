/*
 * main.h
 *
 * Created: 28.02.2017 21:45:19
 *  Author: anton.proshutya
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define ProgrammVer  "2.00"

#define _true    1
#define _false   0


#define LEDPORT  PORTD
#define LEDDIR   DDRD
#define LED_RED    PIND2
#define LED_GREEN  PIND3

#define LED_INIT      SET(LEDDIR,LED_RED); SET(LEDDIR,LED_GREEN)

#define LED_RED_OFF     CLR(LEDPORT,LED_RED)
#define LED_RED_ON      SET(LEDPORT,LED_RED)
#define LED_GREEN_OFF   CLR(LEDPORT,LED_GREEN)
#define LED_GREEN_ON    SET(LEDPORT,LED_GREEN)

#define BUTTON_PORT   PORTD
#define BUTTON_DIR    DDRD
#define BUTTON_INPUT  PIND
#define START_BUTTON  PIND4

#define BUTTON_INIT       SET(BUTTON_PORT,START_BUTTON); CLR(BUTTON_DIR,START_BUTTON) //pull-up button
#define BUTTON_IS_PRESSED   _bitL(BUTTON_INPUT,START_BUTTON)
#define BUTTON_IS_UNPRESSED _bitH(BUTTON_INPUT,START_BUTTON)

#define RELAYPORT      PORTB
#define RELAYDIR       DDRB
#define VOLT_RELAY     PINB0
#define _12_RELAY      PINB1
#define _34_RELAY      PINB2


#define _12RELAY_ON     SET(RELAYPORT,_12_RELAY)
#define _12RELAY_OFF    CLR(RELAYPORT,_12_RELAY)
#define _34RELAY_ON     SET(RELAYPORT,_34_RELAY)
#define _34RELAY_OFF    CLR(RELAYPORT,_34_RELAY)

#define USE_36V          CLR(RELAYPORT,VOLT_RELAY)
#define USE_15V          SET(RELAYPORT,VOLT_RELAY)
#define USE_1to4_PATH    _12RELAY_OFF; _34RELAY_ON
#define USE_2to3_PATH    _12RELAY_ON; _34RELAY_OFF
#define RELAY_INIT       SET(RELAYDIR,VOLT_RELAY); SET(RELAYDIR,_12_RELAY); SET(RELAYDIR,_34_RELAY); USE_1to4_PATH; USE_36V

#define PE_DETECTION         PINC5
#define MODULE_INSERTED      _bitH(PINC,PE_DETECTION)
#define MODULE_NOT_INSERTED  _bitL(PINC,PE_DETECTION)


#define div_round(a, b) (((a) + ((b)/2)) / (b))

#endif /* MAIN_H_ */