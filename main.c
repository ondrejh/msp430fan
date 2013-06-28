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
#include "dcf77.h"

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
        }
        else
        {
            P1IES &= ~DCF77;
            P1IFG &= ~DCF77; // P1.4 IFG cleared

            if (diff>100) // minute mark (probably
            {
                dcf77_synchro(MINUTE);
            }
        }
    }
}
