// pwm module header

#ifndef __PWM_H__
#define __PWM_H__

#include "global.h"

#define HEATING_INIT() do{P2OUT&=~(BIT3|BIT4|BIT5);P2DIR|=(BIT3|BIT4|BIT5);}while(0)
//#define HEATING_ON() {P1OUT|=BIT6;}
//#define HEATING_OFF() do{P2OUT&=~(BIT3|BIT4|BIT5);}while(0)
//#define HEATING ((P2OUT&(BIT3|BIT4|BIT5))!=0)
#define HEATING (((P2OUT&BIT3)>>3)+((P2OUT&BIT4)>>4)+((P2OUT&BIT5)>>5))

void heating_on(int pwr);
void heating_set(heating_status status);
void heating_set_pwr(heating_status status, int power);
#define HEATING_OFF() do{heating_on(0);}while(0)

#endif
