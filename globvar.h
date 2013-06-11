#ifndef __GLOBVAR_H__
#define __GLOBVAR_H__

#include <inttypes.h>
#include <stdbool.h>

#define PROG_LENGTH 8

typedef struct
{
    uint8_t status;
    uint8_t daymask;
    uint16_t starttime;
    uint16_t stoptime;
} tprog_struct;

tprog_struct prog[PROG_LENGTH];

bool pout;
bool pauto;

int16_t  t_val[3];
uint16_t t_err[3];

#endif
