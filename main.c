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
//            |             P1.0|--> COMMUNICATION LED
//            |                 |
//            |             P2.3|-->
//            |             P2.4|--> 3stage heating output
//            |             P2.5|-->
//            |                 |
//            | internal    P1.5|<---> Temp. 1. (sensor DS18B20)
//            | sensor      P2.0|<---> Temp. 2.
//            | Temp. 5     P2.1|<---> Temp. 3.
//            |             P2.2|<---> Temp. 4.
//            |                 |
//
//******************************************************************************

// include section
#include <msp430g2553.h>

#include "uart.h"
#include "comm.h"
#include "ds18b20.h"
#include "pwm.h"
#include "global.h"

// board (leds)
/*#define LED_INIT() {P1DIR|=0x41;P1OUT&=~0x41;}
#define LED_RED_ON() {P1OUT|=0x01;}
#define LED_RED_OFF() {P1OUT&=~0x01;}
#define LED_RED_SWAP() {P1OUT^=0x01;}
#define LED_GREEN_ON() {P1OUT|=0x40;}
#define LED_GREEN_OFF() {P1OUT&=~0x40;}
#define LED_GREEN_SWAP() {P1OUT^=0x40;}*/

// switch input P1.4
#define SWITCH_INIT() do{P1DIR&=~0x10;P1OUT|=0x10;P1REN|=0x10;P1IE|=0x10;P1IES&=~0x10;P1IFG&=~0x10;}while(0)
#define SWITCH() ((P1IN&0x10)==0)

// leds and dco init
void board_init(void)
{
	// oscillator
	BCSCTL1 = CALBC1_1MHZ;		// Set DCO
	DCOCTL = CALDCO_1MHZ;

	SWITCH_INIT();

	HEATING_INIT();
}

// init timer (wdt used)
void wdt_timer_init(void)
{
    //WDTCTL = WDT_MDLY_0_5; // Set Watchdog Timer interval to ~0.5ms
    WDTCTL = WDT_MDLY_8; // Set Watchdog Timer interval to ~8ms
    IE1 |= WDTIE;        // Enable WDT interrupt
}

// init global variables
void global_init(void)
{
    int i;
    for (i=0;i<5;i++)
    {
        t_val[i] = 0;
        t_err[i] = 1;
    }
    heating = AUTO;
    heating_power = 0;

    hauto.channel=1; // T2
    hauto.temperature=79*16;
    hauto.hysteresis=8; // 0.5C

    fuse_switch = SWITCH();
}

// init internal temperature sensor
void temp_init(void)
{
    ADC10CTL1=INCH_10+ ADC10DIV_3; //temp sensor is at 10 and clock/4
    ADC10CTL0=SREF_1 + REFON + ADC10ON + ADC10SHT_3 ; //1.5V ref,Ref on,64 clocks for sample
}

#define temp_start_conversion() do{ADC10CTL0|=ENC+ADC10SC;}while(0)

int temp_read_conversion(void)
{
    while(ADC10CTL1 & BUSY);    //wait..i am converting..pum..pum..
    int t=ADC10MEM&0x3FF;             //store val in t
    ADC10CTL0&=~ENC;            //disable adc conv
    return(int) (long)((((long)t - 673) * 423) / 64);
    //return(int) ((t * 27069L - 18169625L)>>12);   //convert and pass
}

/**
 * temperature regulation function
 *
 * input .. temperature
 * output .. power 0..3
 **/
int auto_heating(int temperature, bool valid)
{
    static int retval = 0;

    if (valid)
    {
        if (temperature>(hauto.temperature+(2-retval)*hauto.hysteresis)) retval--;
        else if (temperature<(hauto.temperature+(1-retval)*hauto.hysteresis)) retval++;

        if (retval<0) retval=0;
        if (retval>3) retval=3;
    }
    else retval = 0;

    return retval;
}

// main program body
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;	// Stop WDT

    global_init(); // init global variables

	board_init(); // init dco and leds
	uart_init(); // init uart
	wdt_timer_init(); // used for main timing
	//pwm_init();
	temp_init();

	ds18b20_sensor_t s[4]; // init ds18b20 sensors
	ds18b20_init(&s[0],&P2OUT,&P2IN,&P2REN,&P2DIR,2); // sensor 0: PORT2 pin 2
	ds18b20_init(&s[1],&P2OUT,&P2IN,&P2REN,&P2DIR,1); // sensor 1: PORT2 pin 1
	ds18b20_init(&s[2],&P2OUT,&P2IN,&P2REN,&P2DIR,0); // sensor 2: PORT2 pin 0
	ds18b20_init(&s[3],&P1OUT,&P1IN,&P1REN,&P1DIR,5); // sensor 3: PORT1 pin 5

	while(1)
	{
	    static int n = 0;

        ds18d20_start_conversion(&s[n]); // start conversion

        if (heating==AUTO)
        {
            int chnl = (hauto.channel-1);
            if (fuse_switch)
            {
                heating_power = auto_heating(t_val[chnl],t_err[chnl]==0);
                heating_on(heating_power);
            }
            else
            {
                HEATING_OFF(); // temp. not measured or fuse off
            }
        }

        __bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on wdt second event)
        ds18b20_read_conversion(&s[n]); // read data from sensor
        if (s[n].valid==true)
        {
            t_val[n]=s[n].data.temp; // save temperature value
            t_err[n]=0; // clear error counter
        }
        else if (t_err[n]!=0xFFFF) t_err[n]++; // increase error counter
        n++;
        if (n==0x04) // get internal temperature
        {
            temp_start_conversion();
            t_val[4] = temp_read_conversion(); // internal temperature
            t_err[4] = 0;
        }
        n&=0x03;

        fuse_switch = SWITCH();

        __bis_SR_register(CPUOFF + GIE); // enter sleep mode (leave on wdt second event)
	}

	return -1;
}

// Heating fuse interrurp
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
  HEATING_OFF();
  fuse_switch = false;
  P1IFG &= ~0x10;                           // P1.4 IFG cleared
}

// Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
    static int cnt = 0;

    cnt++;
    //if (cnt==2000)
    if (cnt==125)
    {
        cnt = 0;
        __bic_SR_register_on_exit(CPUOFF);  // Clear CPUOFF bit from 0(SR)
    }

    if ((UCA0STAT&UCBUSY)==0) UART_TX_ENABLE_OFF();
}
