#ifndef __DCF77_H__
#define __DCF77_H__

typedef enum {LOW, HIGH, MINUTE} t_dcf77bit;

void dcf77_synchro(t_dcf77bit dcf77bit);

#endif
