#include <string.h>
#include <stdio.h>

#include "comm.h"
#include "uart.h"
#include "rtc.h"

uint16_t temp = 0;

int decode_day(char *day)
{
    if (strncmp(day,"MON",3)==0) return 0; // monday
    if (strncmp(day,"TUE",3)==0) return 1; // tuesday
    if (strncmp(day,"WED",3)==0) return 2; // wednesday
    if (strncmp(day,"THU",3)==0) return 3; // thursday
    if (strncmp(day,"FRI",3)==0) return 4; // friday
    if (strncmp(day,"SAT",3)==0) return 5; // saturday
    if (strncmp(day,"SUN",3)==0) return 6; // sunday

    return -1; // no day abbr. found
}

void encode_day(int day,char *daystr)
{
    switch (day)
    {
        case 0: sprintf(daystr,"MON"); break;
        case 1: sprintf(daystr,"TUE"); break;
        case 2: sprintf(daystr,"WED"); break;
        case 3: sprintf(daystr,"THU"); break;
        case 4: sprintf(daystr,"FRI"); break;
        case 5: sprintf(daystr,"SAT"); break;
        case 6: sprintf(daystr,"SUN"); break;
        default: sprintf(daystr,"---"); break;
    }
}

int str2int(char *s,int l)
{
    int res = 0,i;
    for (i=0;i<l;i++)
    {
        char c = *s++;
        if ((c<'0')||(c>'9')) return -1;
        res *= 10;
        res += c-'0';
    }
    return res;
}

int use_command(char *cmdbuf)
{
    int cmdlen = strlen(cmdbuf);

    // "?" command
    if ((cmdlen==1) && (strncmp(cmdbuf,"?\0",2)==0))
    {
        uart_puts("Hello World!\n\r");
        return 0;
    }

    // set time command: "t DDD HH:MM[:SS]"
    if (((cmdlen==11)||(cmdlen==14)) && (strncmp(cmdbuf,"t ",2)==0))
    {
        int day = decode_day(&cmdbuf[2]);
        if (day==-1) return -2;
        int hour,minute,second=0;
        hour = str2int(&cmdbuf[6],2);
        if ((hour<0) || (hour>23)) return -2;
        minute = str2int(&cmdbuf[9],2);
        if ((minute<0) || (minute>59)) return -2;
        if (cmdlen>11)
        {
            second = str2int(&cmdbuf[12],2);
            if ((second<0) || (second>59)) second=0;
        }
        tstruct t;
        t.second = second;
        t.minute = minute;
        t.hour = hour;
        t.dayow = day;
        rtc_set_time(&t);
        uart_puts("Time set OK\n\r");
        return 0;
    }

    // print time command: "t?"
    if ((cmdlen==2) && (strncmp(cmdbuf,"t?\0",3)==0))
    {
        tstruct t;
        rtc_get_time(&t);
        if (timeset==false) t.dayow=-1;
        char daystr[4];
        encode_day(t.dayow,daystr);
        char retstr[UART_TX_BUFLEN];
        sprintf(retstr,"%s %02d:%02d:%02d\n\r",daystr,t.hour,t.minute,t.second);
        uart_puts(retstr);
        return 0;
    }

    // print temp command: "T?"
    if ((cmdlen==2) && (strncmp(cmdbuf,"T?\0",3)==0))
    {
        char retstr[UART_TX_BUFLEN];
        sprintf(retstr,"T %04Xh\n\r",temp);
        uart_puts(retstr);
        return 0;
    }

    return -1;
}
