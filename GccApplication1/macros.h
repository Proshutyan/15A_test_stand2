/*
 * macros.h
 *
 * Modified: 01.03.2017 23:20:00
 *  Author: anton.proshutya
 */ 

#ifndef MACROS_H_
#define MACROS_H_

// -------------------- macros.h ----------------------
#define _setL(port,bit) port&=~(1<<bit)
#define _setH(port,bit) port|=(1<<bit)
#define _set(port,bit,val) _set##val(port,bit)
#define on(x) _set (x)
#define SET _setH

#define _clrL(port,bit) port|=(1<<bit)
#define _clrH(port,bit) port&=~(1<<bit)
#define _clr(port,bit,val) _clr##val(port,bit)
#define off(x) _clr (x)
#define CLR _clrH

#define _bitL(port,bit) (!(port&(1<<bit)))
#define _bitH(port,bit) (port&(1<<bit))
#define _bit(port,bit,val) _bit##val(port,bit)
#define signal(x) _bit (x)
#define BIT _bitH

#define _cpl(port,bit,val) port^=(1<<bit)
#define cpl(x) _cpl (x)
#define CPL _cpl
#define outp(val,port) port=val&0x7f

#define SETBIT(x,y) (x |= (y)) /* Set bit y in byte x*/
#define CLEARBIT(x,y) (x &= (~y)) /* Clear bit y in byte x*/
#define CHECKBIT(x,y) (x & (y)) /* Check bit y in byte x*/ 

//------------------------------------------------
#define outw( ADDRESS, VAL )\
{\
_CLI();\
ADDRESS = VAL;\
_SEI();\
}
#define inw( ADDRESS, VAL )\
{\
_CLI(); \
VAL = ADDRESS;\
_SEI();\
}
#define _HBYTE(W) W>>8
#define _LBYTE(W) W&0xFF      
#define _DOWORD(W,LB,HB) W=(HB<<8)|(LB&0xFF) 
//------------------------------------------------

#endif /* MACROS_H_ */
