// include section
#include <msp430g2553.h>
#include "comm.h"
#include "pwm.h"
#include "global.h"

// init pwm
void pwm_init(void)
{
    P1DIR |= BIT6; // P1.6 to output
    P1SEL |= BIT6; // P1.6 to TA0.1

    CCR0 = 10000-1; // PWM Period (10ms ~ 100Hz)
    CCTL1 = OUTMOD_7; // CCR1 reset/set
    CCR1 = 0; // CCR1 PWM duty cycle
    TACTL = TASSEL_2 + MC_1; // SMCLK, up mode
}

// set pwm value (0..100)
void pwm_set(int val)
{
    if (val>0)
    {
        if (val<100)
        {
            CCR1 = val*100;
            p_val = val;
        }
        else
        {
            CCR1 = 10000;
            p_val = 100;
        }
    }
    else
    {
        CCR1 = 0;
        p_val = 0;
    }
}
