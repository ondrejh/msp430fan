// include section
#include <msp430g2553.h>
#include "comm.h"
#include "pwm.h"
#include "global.h"

void heating_on(int pwr)
{
    int lpwr = pwr;

    // beware endless cycle
    if (lpwr>3) return;
    if (lpwr<0) return;

    static int mask = BIT3;

    if (!fuse_switch) lpwr=0; // test fuse

    // increase power if needed
    while (HEATING<lpwr)
    {
        P2OUT|=mask;
        mask<<=1;
        if (mask>BIT5) mask=BIT3;
    }

    // decrease power if needed
    while (HEATING>lpwr)
    {
        P2OUT&=~mask;
        mask<<=1;
        if (mask>BIT5) mask=BIT3;
    }

    heating_power = lpwr;
}

// set heating output and internall value
void heating_set(heating_status status)
{
    if (status == ON)
    {
        heating = ON;
        heating_on(heating_power);
        //HEATING_ON();
        return;
    }
    if (status == OFF)
    {
        heating = OFF;
        HEATING_OFF();
        heating_power = 0;
        return;
    }
    heating = AUTO;
    return;
}

// set heating output and power
void heating_set_pwr(heating_status status, int power)
{
    if ((power>=0) && (power<=3)) heating_power=power;
    heating_set(status);
}
