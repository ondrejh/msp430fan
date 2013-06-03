#include <msp430g2553.h>
#include <stdbool.h>
#include "ds18b20.h"

typedef struct
{
    uint16_t tstart;
} timer_t;

void timer_init(void)
{
    TACTL = TASSEL_2 + MC_2 + ID_0;	// SMCLK, contmode, fosc/8
}

void timer_wait(uint16_t timeout)
{
    uint16_t tstart = TAR;
    while ((TAR-tstart)<timeout) {};
}

void timer_start(timer_t *t)
{
    t->tstart = TAR;
}

bool timer_timeout(timer_t *t, uint16_t timeout)
{
    if ((TAR-t->tstart)<timeout) return false;
    return true;
}

uint16_t timer_runtime(timer_t *t)
{
    return (TAR-t->tstart);
}

void ds18b20_init(void)
{
    P1DIR &= ~0x80;
    P1OUT &= ~0x80;
}

int ds18b20_bus_reset(void)
{
    // pull bus low
    P1OUT &= ~0x80;
    P1DIR |= 0x80;
    // wait 480us
    timer_wait(480);
    // release the bus
    P1DIR &= ~0x80;
    // wait 100us if presence pulse comes
    timer_t t;
    timer_start(&t);
    while ((P1IN&0x80)==0) if (timer_timeout(&t,100)) return -1;
    // wait up to 480us
    while (!timer_timeout(&t,500)) {};
    // test if bus already high
    if ((P1IN & 0x80)==0) return -2; // presence pulse too long (bus is probably shorted)

    return 0;
}

void ds18b20_write_zero(void)
{
    // pull bus low
    //P1OUT &= ~0x80;
    P1DIR |= 0x80;
    // wait 60us
    timer_wait(60);
    // release the bus
    P1DIR &= ~0x80;
}

void ds18b20_write_one(void)
{
    // pull bus low
    //P1OUT &= ~0x80;
    P1DIR |= 0x80;
    // release the bus
    P1DIR &= ~0x80;
    // wait 60us
    timer_wait(60);
}

void ds18b20_write_byte(uint8_t b)
{
    uint8_t mask = 0x01;
    while (mask!=0)
    {
        if ((b&mask)!=0) ds18b20_write_one();
        else ds18b20_write_zero();
        mask<<=1;
    }
}

bool ds18b20_read_bit(void)
{
    //timer_t t;
    bool retval = false;
    // pull bus low
    P1OUT &= ~0x80;
    P1DIR |= 0x80;
    // release the bus
    P1DIR &= ~0x80;
    // test input
    if ((P1IN&0x80)!=0) retval = true;
    // wait 60 us
    timer_wait(60);
    // return
    return retval;
}

uint8_t ds18b20_read_byte(void)
{
    uint8_t mask = 0x01;
    uint8_t retval = 0;
    while (mask!=0)
    {
        if (ds18b20_read_bit()) retval|=mask;
        mask<<=1;
    }
    return retval;
}
