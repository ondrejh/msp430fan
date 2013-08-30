/*
 * uart.h
 *
 *  Created on: 20.6.2012
 *      Author: ohejda
 *
 *  Description: uart module using rx/tx interrupts.
 *  Functions:
 *  	uart_init .. initialization
 *  	uart_putc .. put char function
 *  	uart_puts .. put string function
 */

#ifndef UART_H_
#define UART_H_

#include <inttypes.h>
#include <stdbool.h>

// uart TX enable
#define UART_TX_ENABLE 1
#if UART_TX_ENABLE==1
	// TX enable port 1 pin 0, active high
	#define UART_TX_ENABLE_INIT() {P1DIR|=0x01;UART_TX_LED_OFF();}
	#define UART_TX_ENABLE_OFF() {P1OUT&=~0x01;}
	#define UART_TX_ENABLE_ON() {P1OUT|=0x01;}
#else
	// no TX enable used
	#define UART_TX_ENABLE_INIT()
	#define UART_TX_ENABLE_OFF()
	#define UART_TX_ENABLE_ON()
#endif
#undef UART_TX_ENABLE

// uart buffer length (mask preferred)
#define UART_TX_BUFLEN 64
#define UART_TX_BUFMASK 0x3F
#define UART_RX_BUFLEN 32
#define UART_RX_BUFMASK 0x1F

char h2c(unsigned int h); // hex to char
int8_t c2h(char c); // char to hex


void uart_init(void); // initialization
int uart_putc(char c); // put char function
int uart_puts(char *s); // put string function

#endif /* UART_H_ */
