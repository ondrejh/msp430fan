// include section
#include <msp430g2553.h>
#include "comm.h"
#include "pwm.h"
#include "global.h"

// set heating output and internall value
void heating_set(heating_status status)
{
    if (status == ON)
    {
        heating = ON;
        HEATING_ON();
        return;
    }
    if (status == OFF)
    {
        heating = OFF;
        HEATING_OFF();
        return;
    }
    heating = AUTO;
    return;
}
