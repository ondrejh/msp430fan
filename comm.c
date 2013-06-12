/**
 * communication interface module (hooked to uart)
 *
 * description: module parses incomming messages and answers or responds to it
 *
 **/

#include <string.h>

#include "comm.h"
#include "uart.h"
#include "rtc.h"
#include "globvar.h"
#include "pout.h"

#define MAX_NUM_LEN 7

// local function prototypes
int decode_day(char *day);
int encode_day(int day, char *daystr);
int16_t str2uint(char *s);
int uint2str(char *s, uint16_t d, int l);
int float2str(char *s, float f, int d);
int getdec(uint16_t val);


// local functions implementation

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

int getdec(uint16_t val)
{
    switch(val&0x000F)
    {
        case 0x0: return 0;
        case 0x1: return 1;
        case 0x2: return 1;
        case 0x3: return 2;
        case 0x4: return 3;
        case 0x5: return 3;
        case 0x6: return 4;
        case 0x7: return 4;
        case 0x8: return 5;
        case 0x9: return 6;
        case 0xA: return 6;
        case 0xB: return 7;
        case 0xC: return 8;
        case 0xD: return 8;
        case 0xE: return 9;
        case 0xF: return 9;
    }
    return -1;
}


// interface implementation

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

    // print temp command: "T?"
    if ((cmdlen==2) && (strncmp(cmdbuf,"T?\0",3)==0))
    {
        char retstr[UART_TX_BUFLEN];
        int i;
        for (i=0;i<3;i++)
        {
            int16_t t = t_val[i];
            uint16_t e = t_err[i];
            char *s = retstr;
            if (i==0) *s++='\n';
            *s++='T';
            s+=(uint2str(s,i+1,1)); *s++=' ';
            if (e==0)
            {
                if (t<0) {*s++='-'; t=-t;}
                s+=uint2str(s,t>>4,0);
                *s++='.';
                s+=uint2str(s,getdec(t),1);
                *s++='C';
            }
            else
            {
                strncpy(s,"---",3); s+=3;
            }
            strncpy(s,"\n\r\0",3); //s+=3;
            uart_puts(retstr);
        }
        return 0;
    }

    // print output status: "P?"
    if ((cmdlen==2) && (strncmp(cmdbuf,"P?\0",3)==0))
    {
        uart_puts("\nP ");
        if (pauto) uart_puts("AUTO ");
        if (pout) uart_puts("ON\n\r");
        else uart_puts("OFF\n\r");
        return 0;
    }

    // set output status: "P ON","P OFF" or "P AUTO"
    if (((cmdlen>=4) || (cmdlen<=6)) && (strncmp(cmdbuf,"P ",2)==0))
    {
        if (strncmp(&cmdbuf[2],"ON\0",3)==0)
            pout_set(ON);
        else if (strncmp(&cmdbuf[2],"OFF\0",4)==0)
            pout_set(OFF);
        else if (strncmp(&cmdbuf[2],"AUTO\0",5)==0)
            pout_set(AUTO);
        else return -1;
        uart_puts("\nOK\n\r");
        return 0;
    }

    // get program line: "pX?"
    if ((cmdlen==3) && (cmdbuf[0]=='p') && (cmdbuf[2]=='?'))
    {
        int i = cmdbuf[1]-'0';
        if ((i<0)||(1>7)) return -1;
        char retstr[UART_TX_BUFLEN];
        char *s = retstr;
        *s++='\n'; *s++='p'; *s++='0'+i; *s++=' ';
        if (prog[i].status==0)
        {
            strncpy(s,"EMPTY\n\r\0",8);
            s+=8;
        }
        else
        {
            int b;
            for (b=0x01;b<0x80;b<<=1) *s++=(((prog[i].daymask&b)==0)?'0':'1');
            *s++=' ';
            s+=uint2str(s,prog[i].starttime>>8,2); *s++=':'; s+=uint2str(s,prog[i].starttime&0xFF,2);
            *s++=' ';
            s+=uint2str(s,prog[i].stoptime>>8,2); *s++=':'; s+=uint2str(s,prog[i].stoptime&0xFF,2);
            strncpy(s,"\n\r\0",3); //s+=3;
        }
        uart_puts(retstr);
    }

    // clear programm line: "pX CLEAR"
    if ((cmdlen==8) && (cmdbuf[0]=='p') && (strncmp(&cmdbuf[3],"CLEAR\0",6)==0))
    {
        int i = cmdbuf[1]-'0';
        if ((i<0)||(i>7)) return -1;
        prog[i].status=0;
        uart_puts("\nOK\n\r");
    }

    // set program line: "pX YYYYYYY HH:MM HH:MM"
    if ((cmdlen==22) && (cmdbuf[0]=='p'))
    {
        int i = cmdbuf[1]-'0';
        if ((i<0)||(i>7)) return -1;
        uint8_t daymask = 0;
        int b;
        char *s = &cmdbuf[3];
        for (b=0x01;b<0x80;b<<=1) if (*s++=='1') daymask|=b;
        uint16_t bh,bm,eh,em; // begin hour/minute, end hour/minute
        bh=str2uint(&cmdbuf[11]);
        if ((bh<0)||(bh>23)) return -2;
        bm=str2uint(&cmdbuf[14]);
        if ((bm<0)||(bm>59)) return -2;
        eh=str2uint(&cmdbuf[17]);
        if ((eh<0)||(eh>23)) return -2;
        em=str2uint(&cmdbuf[20]);
        if ((em<0)||(em>59)) return -2;
        prog[i].status=0;
        prog[i].daymask=daymask;
        prog[i].starttime=(bh<<8)|bm;
        prog[i].stoptime=(eh<<8)|em;
        prog[i].status=1;
        uart_puts("\nOK\n\r");
        if (pauto) pout_set(AUTO);
    }

    return -1;
}
