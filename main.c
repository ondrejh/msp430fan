//******************************************************************************
// Swimming pool filtration switching circuit
//
// It should measure temperature in three points - solar panel lowest side,
// solar panel highest side and swimming pool output temperatute. It switches
// the filtration output acorditg to temperature difference of the solar panel
// sensors and mode of operation.
//
// author:          Ondrej Hejda
// date (started):  26.5.2013
//
// hardware: MSP430G2553 (launchpad)
//
//                MSP4302553
//             -----------------
//         /|\|                 |
//          | |           P1.1,2|--> UART (debug output 9.6kBaud)
//          --|RST              |
//            |             XTAL|<---> 32.768kHz quartz (realtime clock)
//            |                 |
//            |             P1.0|--> COMMUNICATION LED
//            |             P1.6|--> POWER OUTPUT LED
//            |             P1.7|--> AUTO MODE LED
//            |                 |
//            |             P1.3|<---- BUTTON
//            |                 |
//            |             P1.4|<---- DCF77
//            |                 |
//            |             P2.0|<---> Temp. sensor DS18B20 (panel low.s.)
//            |             P2.1|<---> Temp. sensor DS18B20 (panel high.s.)
//            |             P2.2|<---> Temp. sensor DS18B20 (pool)
//            |                 |
//            |             P2.3|--
//            |             P2.4|--|-> Tyristor output (tripled for more power)
//            |             P2.5|--
//            |                 |
//
//******************************************************************************

// include section
#include <msp430g2553.h>

#include "uart.h"
#include "rtc.h"
#include "ds18b20.h"
//#include "comm.h"
#include "globvar.h"
#include "pout.h"

// board (leds)
#define LED_INIT() {P1DIR|=0x41;P1OUT&=~0x41;}
#define LED_RED_ON() {P1OUT|=0x01;}
#define LED_RED_OFF() {P1OUT&=~0x01;}
#define LED_RED_SWAP() {P1OUT^=0x01;}
#define LED_GREEN_ON() {P1OUT|=0x40;}
#define LED_GREEN_OFF() {P1OUT&=~0x40;}
#define LED_GREEN_SWAP() {P1OUT^=0x40;}

#define BUTTON BIT3
#define DCF77 BIT4


// leds and dco init
void board_init(void)
{
	// oscillator
	BCSCTL1 = CALBC1_1MHZ;		// Set DCO
	DCOCTL = CALDCO_1MHZ;

    // button P1.3 & dcf77 P1.4
    P1DIR&=~(BUTTON|DCF77); P1IE|=(BUTTON|DCF77); P1IES|=(BUTTON|DCF77); P1IFG&=~(BUTTON|DCF77); P1REN|=(BUTTON|DCF77);

	LED_INIT(); // leds
}

// main program body
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;	// Stop WDT

	board_init(); // init dco and leds
	uart_init(); // init uart
	rtc_timer_init(); // init rtc timer

	ds18b20_sensor_t s[3]; // init ds18b20 sensors
	ds18b20_init(&s[0],&P2OUT,&P2IN,&P2REN,&P2DIR,0); // sensor 0: PORT2 pin 0
	ds18b20_init(&s[1],&P2OUT,&P2IN,&P2REN,&P2DIR,1); // sensor 1: PORT2 pin 1
	ds18b20_init(&s[2],&P2OUT,&P2IN,&P2REN,&P2DIR,2); // sensor 2: PORT2 pin 2

    prog_init();
    pout_init(); // init power output (pump switch)

	while(1)
	{
	    int i;
        for (i=0;i<3;i++) ds18d20_start_conversion(&s[i]); // start conversion
        __bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on rtc second event)
        for (i=0;i<3;i++)
        {
            ds18b20_read_conversion(&s[i]); // read data from sensor
            if (s[i].valid==true)
            {
                t_val[i]=s[i].data.temp; // save temperature value
                t_err[i]=0; // clear error counter
            }
            else if (t_err[i]!=0xFFFF) t_err[i]++; // increase error counter
        }
        __bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on rtc second event)
        if (minute_event)
        {
            minute_event=false;
            if (pauto) pout_set(AUTO);
            __bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on rtc second event)
        }
	}

	return -1;
}

typedef enum {LOW, HIGH, CLEAR} t_dcf77bit;

void dcf77_synchro(t_dcf77bit dcf77bit)
{
    /* // bits debug output section
    switch (dcf77bit)
    {
        case LOW:
            uart_putc('0');
            break;
        case HIGH:
            uart_putc('1');
            break;
        case CLEAR:
            uart_putc('\n');
            uart_putc('\r');
            break;
    }*/

    static int cnt = 0;

    static int dcf77_minute = 0;
    static int dcf77_hour = 0;
    static int dcf77_dayow = 0;
    static bool dcf77_parity = false;
    static bool dcf77_error = true;

    if (dcf77bit==CLEAR)
    {
        cnt=0;
    }
    else
    {
        switch (cnt)
        {
            case 0:  //break; // 0 allways
                dcf77_error=false;
                if (dcf77bit==HIGH)
                {
                    dcf77_error=true;
                    //uart_putc('X'); // error debug output line
                }
                break;
            case 1:  break; // 1 civil warning bits
            case 2:  break; // 1
            case 3:  break; // 1
            case 4:  break; // 1
            case 5:  break; // 1
            case 6:  break; // 1
            case 7:  break; // 1
            case 8:  break; // 1
            case 9:  break; // 1
            case 10: break; // 0
            case 11: break; // 0
            case 12: break; // 0
            case 13: break; // 0
            case 14: break; // 0
            case 15: break; // reserve antena
            case 16: break; // summer time
            case 17: break; // CEST
            case 18: break; // CET
            case 19: break; // leap second
            case 20: //break; // 1 allways
                if (dcf77bit==LOW)
                {
                    dcf77_error=true;
                    //uart_putc('X'); // error debug output line
                }
                break;
            case 21: //break; // 1 minutes
                dcf77_minute = 0;
                dcf77_parity = false;
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 1;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 22: //break; // 2
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 2;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 23: //break; // 4
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 4;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 24: //break; // 8
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 8;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 25: //break; // 10
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 10;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 26: //break; // 20
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 20;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 27: //break; // 40
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 40;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 28: //break; // even parity over minute bits 21 - 28
                if (((dcf77bit==HIGH) && (!dcf77_parity)) ||
                    ((dcf77bit==LOW) && (dcf77_parity)))
                {
                    dcf77_error=true;
                    //uart_putc('X'); // error debug output line
                }
                break;
            case 29: //break; // 1 hours
                dcf77_hour = 0;
                dcf77_parity = false;
                if (dcf77bit==HIGH)
                {
                    dcf77_hour += 1;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 30: //break; // 2
                if (dcf77bit==HIGH)
                {
                    dcf77_hour += 2;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 31: //break; // 4
                if (dcf77bit==HIGH)
                {
                    dcf77_hour += 4;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 32: //break; // 8
                if (dcf77bit==HIGH)
                {
                    dcf77_hour += 8;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 33: //break; // 10
                if (dcf77bit==HIGH)
                {
                    dcf77_hour += 10;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 34: //break; // 20
                if (dcf77bit==HIGH)
                {
                    dcf77_hour += 20;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 35: //break; // even parity over hour bits 29 - 35
                if (((dcf77bit==HIGH) && (!dcf77_parity)) ||
                    ((dcf77bit==LOW) && (dcf77_parity)))
                {
                    dcf77_error=true;
                    //uart_putc('X'); // error debug output line
                }
                break;
            case 36: //break; // 1 day of month
                dcf77_parity = false;
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 37: //break; // 2
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 38: //break; // 4
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 39: //break; // 8
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 40: //break; // 10
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 41: //break; // 20
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 42: //break; // 1 day of week (monday 1, sunday 7)
                dcf77_dayow = 0;
                if (dcf77bit==HIGH)
                {
                    dcf77_dayow += 1;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 43: //break; // 2
                if (dcf77bit==HIGH)
                {
                    dcf77_dayow += 2;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 44: //break; // 4
                if (dcf77bit==HIGH)
                {
                    dcf77_dayow += 4;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 45: //break; // 1 month
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 46: //break; // 2
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 47: //break; // 4
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 48: //break; // 8
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 49: //break; // 10
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 50: //break; // 1 year within century
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 51: //break; // 2
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 52: //break; // 4
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 53: //break; // 8
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 54: //break; // 10
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 55: //break; // 20
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 56: //break; // 40
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 57: //break; // 80
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 58: //break; // even parity over bits 36 - 58
                if (((dcf77bit==HIGH) && (!dcf77_parity)) ||
                    ((dcf77bit==LOW) && (dcf77_parity)))
                {
                    dcf77_error=true;
                    //uart_putc('X'); // error debug output line
                }
                if (!dcf77_error)
                {
                    tstruct t;
                    t.second = 0;
                    t.minute = dcf77_minute;
                    t.hour = dcf77_hour;
                    t.dayow = dcf77_dayow-1;
                    rtc_set_time(&t);
                }
                break;
            default: break;
        }
        cnt++;
    }
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    // button input
    if (P1IFG&BUTTON)
    {
        static int status = 0;
        P1IFG &= ~BUTTON; // P1.3 IFG cleared
        switch (status)
        {
            case 0: pout_set(OFF); break;
            case 1: pout_set(AUTO); break;
            case 2: pout_set(ON); break;
            case 3: pout_set(AUTO); break;
        }
        status = (status+1)&0x03;
    }

    // dcf77 input
    if (P1IFG&DCF77)
    {
        static unsigned int rtc_last_ticks=0;
        unsigned int now = rtc_ticks;
        unsigned int diff = now-rtc_last_ticks;
        rtc_last_ticks = now;

        /* // timing debug output section
        char s[6];
        s[uint2str(s,diff,0)]='\0';
        uart_puts(s);
        uart_putc('\n'); uart_putc('\r');*/

        if ((P1IN&DCF77)!=0)
        {
            P1IES |= DCF77;
            P1IFG &= ~DCF77; // P1.4 IFG cleared

            if ((diff>3)&&(diff<10)) // log "0" detected
            {
                dcf77_synchro(LOW);
            }
            else if ((diff>10)&&(diff<16)) // log "1" detected
            {
                dcf77_synchro(HIGH);
            }
            else // signal error (less than 4, more than 15 or 10 exactly)
            {
                //dcf77_synchro(CLEAR);
            }
        }
        else
        {
            P1IES &= ~DCF77;
            P1IFG &= ~DCF77; // P1.4 IFG cleared

            if (diff<40) // signal error (too short pause)
            {
                //dcf77_synchro(CLEAR);
            }
            else if (diff>100) // minute mark (probably
            {
                dcf77_synchro(CLEAR);
            }
        }
    }
    /*P1IES ^= BUTTON; // toggle the interrupt edge,
    if (P1IES&BUTTON) pout_set(ON);
    else pout_set(OFF);*/
    // the interrupt vector will be called
    // when P1.3 goes from HitoLow as well as
    // LowtoHigh
}
