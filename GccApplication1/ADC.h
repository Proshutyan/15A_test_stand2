/*
 * ADC.h
 *
 * Created: 03.03.2017 21:19:08
 *  Author: anton.proshutya
 */ 


#ifndef ADC_H_
#define ADC_H_

void ADC_INIT (void);
unsigned int ADC_get_Breakdown_mV (void);
unsigned int ADC_get_Leakage_mV (void);

#endif /* ADC_H_ */