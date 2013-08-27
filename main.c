//******************************************************************************
// DC fan PWM driver with temperature sensor, based on MSP430 launchpad
//
// author:          Ondrej Hejda
// date (started):  27.8.2013
//
// hardware: MSP430G2553 (launchpad)
//
//                MSP4302553
//             -----------------
//         /|\|                 |
//          | |           P1.1,2|--> UART (debug output 9.6kBaud)
//          --|RST              |
//            |             XTAL|<---> 32.768kHz quartz
//            |                 |
//            |             P1.0|--> COMMUNICATION LED
//            |                 |
//            |             P1.6|--> FAN OUTPUT (PWM)
//            |                 |
//            |             P2.0|<---> Temp. sensor DS18B20
//            |                 |
//
//******************************************************************************

// include section
#include <msp430g2553.h>

#include "uart.h"
#include "comm.h"
#include "ds18b20.h"
#include "pwm.h"

// board (leds)
#define LED_INIT() {P1DIR|=0x41;P1OUT&=~0x41;}
#define LED_RED_ON() {P1OUT|=0x01;}
#define LED_RED_OFF() {P1OUT&=~0x01;}
#define LED_RED_SWAP() {P1OUT^=0x01;}
#define LED_GREEN_ON() {P1OUT|=0x40;}
#define LED_GREEN_OFF() {P1OUT&=~0x40;}
#define LED_GREEN_SWAP() {P1OUT^=0x40;}

bool wdt_timer_flag = false;

// leds and dco init
void board_init(void)
{
	// oscillator
	BCSCTL1 = CALBC1_1MHZ;		// Set DCO
	DCOCTL = CALDCO_1MHZ;

	LED_INIT(); // leds
}

// init timer (wdt used)
void wdt_timer_init(void)
{
    WDTCTL = WDT_MDLY_8;   // Set Watchdog Timer interval to ~8ms
    IE1 |= WDTIE;           // Enable WDT interrupt
}

// main program body
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;	// Stop WDT

	board_init(); // init dco and leds
	uart_init(); // init uart
	wdt_timer_init();
	pwm_init();

	ds18b20_sensor_t s; // init ds18b20 sensors
	ds18b20_init(&s,&P2OUT,&P2IN,&P2REN,&P2DIR,0); // sensor 0: PORT2 pin 0

	while(1)
	{
        ds18d20_start_conversion(&s); // start conversion
        __bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on wdt second event)
        ds18b20_read_conversion(&s); // read data from sensor
        if (s.valid==true)
        {
            t_val=s.data.temp; // save temperature value
            t_err=0; // clear error counter
        }
        else if (t_err!=0xFFFF) t_err++; // increase error counter
        __bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on wdt second event)
	}

	return -1;
}

// Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
    static int cnt = 0;

    cnt++;
    if (cnt==125)
    {
        cnt = 0;
        wdt_timer_flag = true;
        __bic_SR_register_on_exit(CPUOFF);  // Clear CPUOFF bit from 0(SR)
    }
}
