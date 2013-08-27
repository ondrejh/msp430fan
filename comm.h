#ifndef __COMM_H__
#define __COMM_H__

#include <inttypes.h>

int16_t  t_val;
uint16_t t_err;
uint16_t p_val;

int uint2str(char *s, uint16_t d, int l);
int use_command(char *cmdbuf);

#endif
