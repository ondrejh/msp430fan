#include <string.h>

#include "comm.h"
#include "uart.h"
#include "rtc.h"

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

int encode_day(int day,char *daystr)
{
    switch (day)
    {
        case 0: strncpy(daystr,"MON",3); break;
        case 1: strncpy(daystr,"TUE",3); break;
        case 2: strncpy(daystr,"WED",3); break;
        case 3: strncpy(daystr,"THU",3); break;
        case 4: strncpy(daystr,"FRI",3); break;
        case 5: strncpy(daystr,"SAT",3); break;
        case 6: strncpy(daystr,"SUN",3); break;
        default: strncpy(daystr,"---",3); break;
    }
    return 3;
}

int16_t str2uint(char *s)
{
    uint16_t res = 0,i;
    for (i=0;;i++)
    {
        char c = *s++;
        if ((c<'0')||(c>'9')) break;
        res *= 10;
        res += c-'0';
    }
    if (i==0) return -1;
    return res;
}

#define MAX_NUM_LEN 7
int uint2str(char *s, uint16_t d, int l)
{
    char numbuf[MAX_NUM_LEN+1];
    numbuf[MAX_NUM_LEN]='\0';
    int ptr = MAX_NUM_LEN;
    int cnt = 0;
    uint16_t cpd = d;
    do
    {
        numbuf[--ptr]='0'+(cpd%10);
        cpd/=10;
        cnt++;
    }
    while (cpd!=0);
    while (cnt<l) {numbuf[--ptr]='0'; cnt++;}
    while (numbuf[ptr]!='\0') *s++ = numbuf[ptr++];
    return cnt;
}

int float2str(char *s, float f, int d)
{
    uint16_t hw,hd;
    char *sptr=s;
    uint16_t decn = 1, i;
    for (i=0;i<d;i++) decn*=10;
    if (f<0)
    {
        *s++='-';
        hw=(uint16_t)(-f);
        hd=(uint16_t)((-f-(float)hw)*(float)decn);
    }
    else
    {
        hw=(uint16_t)f;
        hd=(uint16_t)((f-(float)hw)*(float)decn);
    }
    s+=uint2str(s,hw,0);
    *s++= '.';
    s+=uint2str(s,hd,0);
    return s-sptr;
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
        hour = str2uint(&cmdbuf[6]);
        if ((hour<0) || (hour>23)) return -2;
        minute = str2uint(&cmdbuf[9]);
        if ((minute<0) || (minute>59)) return -2;
        if (cmdlen>11)
        {
            second = str2uint(&cmdbuf[12]);
            if ((second<0) || (second>59)) second=0;
        }
        tstruct t;
        t.second = second;
        t.minute = minute;
        t.hour = hour;
        t.dayow = day;
        rtc_set_time(&t);
        uart_puts("\nOK\n\r");
        return 0;
    }

    // print time command: "t?"
    if ((cmdlen==2) && (strncmp(cmdbuf,"t?\0",3)==0))
    {
        tstruct t;
        rtc_get_time(&t);
        if (timeset==false) t.dayow=-1;
        char str[UART_TX_BUFLEN];
        char *s = str;
        *s++='\n';
        encode_day(t.dayow,s); s+=3; *s++=' ';
        s+=uint2str(s,t.hour,2); *s++=':';
        s+=uint2str(s,t.minute,2); *s++=':';
        s+=uint2str(s,t.second,2);
        strncpy(s,"\n\r\0",3); //s+=3;
        uart_puts(str);
        return 0;
    }

    // print temperature: "T?"
    if ((cmdlen==2) && (strncmp(cmdbuf,"T?\0",3)==0))
    {
        float temp[3] = {25.8, -15.7, 5.4};
        int i;
        uart_putc('\n');
        for (i=0;i<3;i++)
        {
            char str[UART_TX_BUFLEN];
            char *s = str;
            *s++='T'; *s++=('1'+i); *s++=' ';
            s+=float2str(s,temp[i],2);
            strncpy(s,"C\n\r\0",4); //s+=4;
            uart_puts(str);
        }
        return 0;
    }

    return -1;
}
