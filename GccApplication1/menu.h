/*
 * menu.h
 *
 * Created: 16.03.2017 22:50:51
 *  Author: anton.proshutya
 */ 


#ifndef MENU_H_
#define MENU_H_

unsigned char  WaitCommand(void);
void SendVersionLine (void);
void SendDeviceVersionLine (void);
void ShowVoltage (unsigned int ucData);
void ShowLeakage (unsigned int ucData);
void SendModuleTestPassed (void);
void SendTestNotPased (void);

#endif /* MENU_H_ */