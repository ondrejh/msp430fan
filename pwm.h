// pwm module header

#ifndef __PWM_H__
#define __PWM_H__

#include "global.h"

#define HEATING_INIT() {P1OUT&=~BIT6;P1DIR|=BIT6;}
#define HEATING_ON() {P1OUT|=BIT6;}
#define HEATING_OFF() {P1OUT&=~BIT6;}
#define HEATING ((P1OUT&BIT6)!=0)

void heating_set(heating_status status);

#endif
