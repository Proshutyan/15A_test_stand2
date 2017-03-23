/*
 * menu.h
 *
 * Created: 16.03.2017 22:50:51
 *  Author: anton.proshutya
 */ 


#ifndef MENU_H_
#define MENU_H_

#define _start_and_count   'S'
#define _restart_wo_count  'R'

char WaitCommand(void);
void SendVersionLine (void);
void SendDeviceVersionLine (void);
void ShowVoltage (unsigned int ucData);
void ShowLeakage (unsigned int ucData);
void SendModuleTestPassed (void);
void SendTestNotPased (void);

#endif /* MENU_H_ */