#include <msp430g2553.h>
#include <stdbool.h>

#include "globvar.h"
#include "pout.h"

// initialize power ouput
void pout_init(void)
{
    // power output pins P2 3..5
    P2OUT&=~0x38;
    P2DIR|= 0x38;
    P2REN&=~0x38;
    // green led
    P1OUT&=~0x40;
    P1DIR|= 0x40;
    P1REN&=~0x40;
}

// set power output status (on/off)
void pout_set(bool state)
{
    if (state)
    {
        P2OUT|=0x38; // power outputs on
        P1OUT|=0x40; // green led on
        pout=true; // set power output global variable
    }
    else
    {
        P2OUT&=~0x38; // power output off
        P1OUT&=~0x40; // green led off
        pout=false; // set power output global variable
    }
}

