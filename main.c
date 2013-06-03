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
	timer_init();
	ds18b20_init();

	while(1)
	{
        __bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on rtc second event)
        ds18b20_bus_reset();
        ds18b20_write_byte(0xCC);
        ds18b20_write_byte(0x44);
        __bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on rtc second event)
        ds18b20_bus_reset();
        ds18b20_write_byte(0xCC);
        ds18b20_write_byte(0xBE);
        uint8_t b1 = ds18b20_read_byte();
        uint8_t b2 = ds18b20_read_byte();
        temp = ((uint16_t)b2<<8) | (uint16_t)b1;
	}

	return -1;
}
