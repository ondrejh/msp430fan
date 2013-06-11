#ifndef __POUT_H__
#define __POUT_H__


// initialize power ouput
void pout_init(void);

// set power output status (on/off)
typedef enum {ON, OFF, AUTO,} t_setstatus;
void pout_set(t_setstatus state);

void prog_init(void);

#endif
