/// list of global variables

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdbool.h>

typedef enum heating_status {
    OFF,
    ON,
    AUTO,
} heating_status;

typedef struct heating_auto_settings heating_auto_settings;

struct heating_auto_settings{
    int channel;
    int temperature;
    int hysteresis;
};

heating_auto_settings hauto;

volatile int t_val[4];
volatile int t_err[4];
volatile heating_status heating;

#endif
