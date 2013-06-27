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
#include "comm.h"
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
    static int cnt=0;

    if (dcf77bit==CLEAR)
    {
        cnt=0;
    }
    else
    {
        switch (cnt)
        {
            case 0:  break; // 1 allways
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
            case 16: break; //
            case 17: break; //
            case 18: break; //
            case 19: break; //
            case 20: break; //
            case 21: break; //
            case 22: break; //
            case 23: break; //
            case 24: break; //
            case 25: break; //
            case 26: break; //
            case 27: break; //
            case 28: break; //
            case 29: break; //
            case 30: break; //
            case 31: break; //
            case 32: break; //
            case 33: break; //
            case 34: break; //
            case 35: break; //
            case 36: break; //
            case 37: break; //
            case 38: break; //
            case 39: break; //
            case 40: break; //
            case 41: break; //
            case 42: break; //
            case 43: break; //
            case 44: break; //
            case 45: break; //
            case 46: break; //
            case 47: break; //
            case 48: break; //
            case 49: break; //
            case 50: break; //
            case 51: break; //
            case 52: break; //
            case 53: break; //
            case 54: break; //
            case 55: break; //
            case 56: break; //
            case 57: break; //
            case 58: break; //
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

        if ((P1IES&DCF77)!=0)
        {
            if ((diff>3)&&(diff<10)) // log "0" detected
            {

            }
            else if ((diff>10)&&(diff<16)) // log "1" detected
            {

            }
            else // signal error (less than 4, more than 15 or 10 exactly)
            {

            }
        }
        else
        {
            if (diff<40) // signal error (too short pause)
            {

            }
            else if (diff>100) // minute mark (probably
            {

            }
        }

        P1IES ^= DCF77; // swap
    }
    /*P1IES ^= BUTTON; // toggle the interrupt edge,
    if (P1IES&BUTTON) pout_set(ON);
    else pout_set(OFF);*/
    // the interrupt vector will be called
    // when P1.3 goes from HitoLow as well as
    // LowtoHigh
}
