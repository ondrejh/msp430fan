/**
 * communication interface module (hooked to uart)
 *
 * description: module parses incomming messages and answers or responds to it
 *
 **/

//#include <string.h>

#include "comm.h"
#include "pwm.h"
#include "uart.h"
#include "global.h"

#define MAX_NUM_LEN 7

// local function prototypes
int16_t str2uint(char *s);
//int uint2str(char *s, uint16_t d, int l);
int float2str(char *s, float f, int d);
int getdec(uint16_t val);

// local functions implementation

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
    //int cmdlen = strlen(cmdbuf);
    int cmdlen;
    for (cmdlen=0;;cmdlen++) if (cmdbuf[cmdlen]=='\0') break;

    /*// "?" command
    if ((cmdlen==1) && (cmdbuf[0]=='?'))
    {
        uart_puts("Hello World!\n\r");
        return 0;
    }*/

    if ((cmdlen>4) && (cmdbuf[0]=='@') && (cmdbuf[1]=='A') && (cmdbuf[2]=='1') && (cmdbuf[3]==':'))
    {
        // print temp command: "@A1:T?"
        if ((cmdlen==6) && (cmdbuf[4]=='T') && (cmdbuf[5]=='?'))
        {
            char retstr[UART_TX_BUFLEN];
            int16_t t = t_val;
            uint16_t e = t_err;
            char *s = retstr;
            // /* *s++='\n';*/ *s++='T'; *s++=' ';
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
                //strncpy(s,"---",3); s+=3;
                *s++='-'; *s++='-'; *s++='-';
            }
            //strncpy(s,"\n\r\0",3); //s+=3;
            *s++='\n'; *s++='\r'; *s++='\0';
            uart_puts(retstr);
            return 0;
        }

        // set power command: "@A1:P xxx"
        if ((cmdlen>5) && (cmdbuf[4]=='P'))
        {
            if (cmdbuf[5]==' ')
            {
                if (cmdbuf[6]=='O')
                {
                    if ((cmdbuf[7]=='N') && (cmdbuf[8]=='\0'))
                        pwm_set(100);
                    else if ((cmdbuf[7]=='F') && (cmdbuf[8]=='F') && (cmdbuf[9]=='\0'))
                        pwm_set(0);
                }
                else
                {
                    int p = str2uint(&cmdbuf[6]);
                    if (p!=-1)
                        pwm_set(p);
                }
            }
            if ((cmdbuf[5]==' ') || (cmdbuf[5]=='?'))
            {
                char retstr[UART_TX_BUFLEN];
                //uint16_t p = p_val;
                char *s = retstr;
                // /* *s++='\n';*/ *s++='P'; *s++=' ';
                if (p_val==0)
                {
                    *s++='O'; *s++='F'; *s++='F';
                }
                else if (p_val==100)
                {
                    *s++='O'; *s++='N';
                }
                else
                {
                    s+=uint2str(s,p_val,1);
                    *s++='%';
                }
                //strncpy(s,"\n\r\0",3); //s+=3;
                *s++='\n'; *s++='\r'; *s++='\0';
                uart_puts(retstr);
                return 0;
            }
        }
    }

    return -1;
}
