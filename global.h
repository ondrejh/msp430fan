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
    int temperature1;
    int temperature2;
    int temperature3;
    int hysteresis;
};

heating_auto_settings hauto;

volatile int t_val[5];
volatile int t_err[5];
volatile heating_status heating;
volatile int heating_power;

#endif
