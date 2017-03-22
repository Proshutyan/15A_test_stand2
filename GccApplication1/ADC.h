/*
 * ADC.h
 *
 * Created: 03.03.2017 21:19:08
 *  Author: anton.proshutya
 */ 


#ifndef ADC_H_
#define ADC_H_

#ifdef DEVICE1
#define _2_56V 2470L  //Real value for the first device
#elif DEVICE2
#define _2_56V 2448L   //Real value for the second device in mV.
#else 
#  error "Serial number is not defined"
#endif

void ADC_INIT (void);
unsigned int ADC_get_Breakdown_mV (void);
unsigned int ADC_get_Leakage_mV (void);

#endif /* ADC_H_ */