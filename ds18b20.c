#include <msp430g2553.h>
#include <stdbool.h>
#include "ds18b20.h"
#include "crc8.h"

#define wait(x) __delay_cycles(x)


// sensor context struct


// local function prototypes

void ds18b20_bus_reset(ds18b20_sensor_t *s);

void ds18b20_write_zero(ds18b20_sensor_t *s);
void ds18b20_write_one(ds18b20_sensor_t *s);
void ds18b20_write_byte(ds18b20_sensor_t *s, uint8_t b);

int ds18b20_read_bit(ds18b20_sensor_t *s);
uint8_t ds18b20_read_byte(ds18b20_sensor_t *s);


// local functions implementation

void ds18b20_bus_reset(ds18b20_sensor_t *s)
{
    // pull bus low
    *(s->port_out) &= ~(s->port_mask); // P1OUT &= ~0x80;
    *(s->port_dir) |=  (s->port_mask); // P1DIR |= 0x80;
    // wait 480us
    wait(480);
    // release the bus
    *(s->port_dir) &= ~(s->port_mask); // P1DIR &= ~0x80;
    // wait 480us
    wait(480);
}

void ds18b20_write_zero(ds18b20_sensor_t *s)
{
    // pull bus low
    *(s->port_out) &= ~(s->port_mask); // P1OUT &= ~0x80;
    *(s->port_dir) |=  (s->port_mask); // P1DIR |= 0x80;
    // wait 60us
    wait(60);
    // release the bus
    *(s->port_dir) &= ~(s->port_mask); // P1DIR &= ~0x80;
}

void ds18b20_write_one(ds18b20_sensor_t *s)
{
    // pull bus low
    *(s->port_out) &= ~(s->port_mask); // P1OUT &= ~0x80;
    *(s->port_dir) |=  (s->port_mask); // P1DIR |= 0x80;
    // release the bus
    *(s->port_dir) &= ~(s->port_mask); // P1DIR &= ~0x80;
    // wait 60us
    wait(60);
}

void ds18b20_write_byte(ds18b20_sensor_t *s, uint8_t b)
{
    uint8_t mask = 0x01;
    while (mask!=0)
    {
        if ((b&mask)!=0) ds18b20_write_one(s);
        else ds18b20_write_zero(s);
        mask<<=1;
    }
}

int ds18b20_read_bit(ds18b20_sensor_t *s)
{
    int retval = 0;
    // pull bus low
    //P1OUT &= ~0x80;
    //P1DIR |= 0x80;
    *(s->port_out) &= ~(s->port_mask);
    *(s->port_dir) |=  (s->port_mask);
    // release the bus
    //P1DIR &= ~0x80;
    *(s->port_dir) &= ~(s->port_mask);
    // test input
    //if ((P1IN&0x80)!=0) retval = 1;
    if ((*(s->port_in)&0x80)!=0) retval = 1;
    // wait 60 us
    wait(60);
    // return
    return retval;
}

uint8_t ds18b20_read_byte(ds18b20_sensor_t *s)
{
    uint8_t mask = 0x01;
    uint8_t retval = 0;
    while (mask!=0)
    {
        if (ds18b20_read_bit(s)) retval|=mask;
        mask<<=1;
    }
    return retval;
}


// interface functions implementation

void ds18b20_init(ds18b20_sensor_t *s,
                  volatile uint8_t *p_out,
                  const volatile uint8_t *p_in,
                  volatile uint8_t *p_ren,
                  volatile uint8_t *p_dir,
                  int pin)
{
    (s->port_out) = p_out;
    (s->port_dir) = p_dir;
    (s->port_ren) = p_ren;
    (s->port_in)  = p_in;
    s->port_mask = (1<<pin);

    *(s->port_dir) &= ~(s->port_mask);
    *(s->port_out) &= ~(s->port_mask);
    *(s->port_ren) &= ~(s->port_mask);

    /*P1DIR &= ~0x80;
    P1OUT &= ~0x80;*/
}

void ds18d20_start_conversion(ds18b20_sensor_t *s)
{
    ds18b20_bus_reset(s);
    ds18b20_write_byte(s,0xCC);
    ds18b20_write_byte(s,0x44);
}

void ds18b20_read_conversion(ds18b20_sensor_t *s)
{
    ds18b20_bus_reset(s);
    ds18b20_write_byte(s,0xCC);
    ds18b20_write_byte(s,0xBE);
    int i; uint8_t data[9];
    for (i=0;i<10;i++) data[i]=ds18b20_read_byte(s);
    s->temp = ((uint16_t)data[1]<<8) | (uint16_t)data[0];
    if (crc8(data,8)==data[8]) s->valid = true;
    else s->valid = false;
}
