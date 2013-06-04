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
//            |             P1.0|--> RED LED (communication - active high)
//            |             P1.6|--> GREEN LED (output on - active high)
//            |                 |
//            |             P1.3|----> BUTTON
//            |                 |
//
//******************************************************************************

// include section
#include <msp430g2553.h>

#include "uart.h"
#include "rtc.h"
#include "ds18b20.h"
#include "comm.h"

// board (leds)
#define LED_INIT() {P1DIR|=0x41;P1OUT&=~0x41;}
#define LED_RED_ON() {P1OUT|=0x01;}
#define LED_RED_OFF() {P1OUT&=~0x01;}
#define LED_RED_SWAP() {P1OUT^=0x01;}
#define LED_GREEN_ON() {P1OUT|=0x40;}
#define LED_GREEN_OFF() {P1OUT&=~0x40;}
#define LED_GREEN_SWAP() {P1OUT^=0x40;}


// leds and dco init
void board_init(void)
{
	// oscillator
	BCSCTL1 = CALBC1_1MHZ;		// Set DCO
	DCOCTL = CALDCO_1MHZ;
	/*BCSCTL1 = CALBC1_8MHZ;	// Set DCO
	DCOCTL = CALDCO_8MHZ;
	BCSCTL1 = CALBC1_16MHZ;		// Set DCO
	DCOCTL = CALDCO_16MHZ;*/

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
	ds18b20_init(&s[0],&P1OUT,&P1IN,&P1REN,&P1DIR,7); // sensor 0: PORT1 pin 7
	ds18b20_init(&s[1],&P2OUT,&P2IN,&P2REN,&P2DIR,1); // sensor 1: PORT2 pin 1
	ds18b20_init(&s[2],&P2OUT,&P2IN,&P2REN,&P2DIR,2); // sensor 2: PORT2 pin 2

	while(1)
	{
	    int i;
        __bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on rtc second event)
        for (i=0;i<3;i++) ds18d20_start_conversion(&s[i]);
        __bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on rtc second event)
        for (i=0;i<3;i++) ds18b20_read_conversion(&s[i]);
        if (s[0].valid) temp=s[0].data.temp;
	}

	return -1;
}
