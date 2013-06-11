#include <msp430g2553.h>
#include <stdbool.h>

#include "globvar.h"
#include "pout.h"

#define PUMP_POUT P2OUT
#define PUMP_PDIR P2DIR
#define PUMP_MASK ((1<<3)|(1<<4)|(1<<5)) // P2.3,4,5

#define PUMP_LED_POUT P1OUT
#define PUMP_LED_PDIR P1DIR
#define PUMP_LED_MASK (1<<6) // P1.6
#define AUTO_LED_POUT P1OUT
#define AUTO_LED_PDIR P1DIR
#define AUTO_LED_MASK (1<<7) // P1.7

// initialize power ouput
void pout_init(void)
{
    // power output pins P2 3..5
    PUMP_POUT&=~PUMP_MASK;
    PUMP_PDIR|= PUMP_MASK;
    // pump signal led
    PUMP_LED_POUT&=~PUMP_LED_MASK;
    PUMP_LED_PDIR|= PUMP_LED_MASK;
    // auto signal led
    AUTO_LED_POUT|= AUTO_LED_MASK;
    AUTO_LED_PDIR|= AUTO_LED_MASK;
    // set variables
    pout=false;
    pauto=true;
}

// set power output status (on/off)
void pout_set(t_setstatus state)
{
    switch (state)
    {
        case ON:
            PUMP_POUT     |= PUMP_MASK; // power outputs on
            PUMP_LED_POUT |= PUMP_LED_MASK; // power signal led on
            pout=true; // set power output global variable
            AUTO_LED_POUT &=~AUTO_LED_MASK; // auto signal led off
            pauto=false; // set auto signal off
            break;
        case OFF:
            PUMP_POUT     &= ~PUMP_MASK; // power output off
            PUMP_LED_POUT &= ~PUMP_LED_MASK; // power signal led off
            pout=false; // set power output global variable
            AUTO_LED_POUT &= ~AUTO_LED_MASK; // auto signal led off
            pauto=false;
            break;
        case AUTO:
            PUMP_POUT     &= ~PUMP_MASK; // power outputs off
            PUMP_LED_POUT &= ~PUMP_LED_MASK; // power signal led off
            pout=false; // set power output global variable
            AUTO_LED_POUT |=  AUTO_LED_MASK; // auto signal led om
            pauto=true;
            break;
    }
}

// initialize programm
void prog_init(void)
{
    int i;
    for (i=0;i<PROG_LENGTH;i++) prog[i].status=0; // reset all prog lines

    prog[0].daymask=(1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4); // mon - fri
    prog[0].starttime=(16/*h*/<<8)|(35/*min*/);
    prog[0].stoptime=(18/*h*/<<8)|(35/*min*/);
    prog[0].status=1;

    prog[1].daymask=(1<<5)|(1<<6); // sat - sun
    prog[1].starttime=(16/*h*/<<8)|(00/*min*/);
    prog[1].stoptime=(18/*h*/<<8)|(00/*min*/);
    prog[1].status=1;
}
