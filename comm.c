//#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include "comm.h"
#include "uart.h"
#include "rtc.h"

int decode_day(char *day)
{
    if (strncmp(day,"PO",2)==0) return 0;
    if (strncmp(day,"UT",2)==0) return 1;
    if (strncmp(day,"ST",2)==0) return 2;
    if (strncmp(day,"CT",2)==0) return 3;
    if (strncmp(day,"PA",2)==0) return 4;
    if (strncmp(day,"SO",2)==0) return 5;
    if (strncmp(day,"NE",2)==0) return 6;

    return -1;
}

void use_command(char *cmdbuf)
{
    // ? command
    if (strncmp(cmdbuf,"?\0",2)==0)
    {
        uart_puts("Hello World!\n\r");
        return;
    }

    /*// set time: T DD HH:MM
    char dd[3];
    uint16_t hh,mm;
    if (sscanf(cmdbuf,"T %2s %2d:%2d\0",&dd,&hh,&mm)==3)
    {
        int day = decode_day(dd);
        if (day<0) return;
        if ((hh<0)||(hh>23)) return;
        if ((mm<0)||(mm>59)) return;
        tstruct tset;
        tset.second = 0;
        tset.minute = mm;
        tset.hour = hh;
        tset.dayow = day;
        rtc_set_time(&tset);
    }*/
}
