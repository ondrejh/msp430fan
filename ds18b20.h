#ifndef __DS18B20_H__
#define __DS18B20_H__

#include <inttypes.h>

void timer_init(void);
void ds18b20_init(void);
int ds18b20_bus_reset(void);
void ds18b20_write_byte(uint8_t b);
uint8_t ds18b20_read_byte(void);

#endif
