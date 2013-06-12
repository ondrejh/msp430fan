#include <msp430g2553.h>
#include <stdbool.h>

#include "globvar.h"
#include "rtc.h"
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

#define SET_AUTO_ON() {AUTO_LED_POUT|=AUTO_LED_MASK;pauto=true;}
#define SET_AUTO_OFF() {AUTO_LED_POUT&=~AUTO_LED_MASK;pauto=false;}
#define SET_OUTPUT_ON() {PUMP_POUT|=PUMP_MASK;PUMP_LED_POUT|=PUMP_LED_MASK;pout=true;}
#define SET_OUTPUT_OFF() {PUMP_POUT&=~PUMP_MASK;PUMP_LED_POUT&=~PUMP_LED_MASK;pout=false;}

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
            SET_OUTPUT_ON(); // power output on
            SET_AUTO_OFF(); // set auto signal off
            break;
        case OFF:
            SET_OUTPUT_OFF(); // power output off
            SET_AUTO_OFF(); // auto signal led off
            break;
        case AUTO:
            SET_AUTO_ON(); // auto signal led on
            {
                tstruct t;
                rtc_get_time(&t);
                if (prog_test(t)) SET_OUTPUT_ON() // power output on
                else SET_OUTPUT_OFF(); // power output off
            }
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

// test program
bool prog_test(tstruct t)
{
    uint8_t daymask = (1<<t.dayow);
    uint16_t time = (t.hour<<8) | t.minute;

    int i;

    for (i=0;i<PROG_LENGTH;i++)
    {
        if ((prog[i].status!=0) &&
            ((daymask&prog[i].daymask)!=0) &&
            (time>=prog[i].starttime) &&
            (time<=prog[i].stoptime)) return true;
    }

    return false;
}
