/**
 * dcf77 time synchronization module
 *
 * author: ondrejh.ck@email.cz
 * date: 6.2013
 *
 * dcf77_synchro function should be feed with LOW/HIGH/MINUTE marks
 *
 * it calls rtc_set_time function when MINUTE mark is present
 * and minute data was received correctly
 *
 */

#include "dcf77.h"
#include "rtc.h"
//#include "uart.h" // debug

void dcf77_synchro(t_dcf77bit dcf77bit)
{
    /* // bits debug output section
    switch (dcf77bit)
    {
        case LOW:
            uart_putc('0');
            break;
        case HIGH:
            uart_putc('1');
            break;
        case CLEAR:
            uart_putc('\n');
            uart_putc('\r');
            break;
    }*/

    static int cnt = 0;

    static int dcf77_minute = 0;
    static int dcf77_hour = 0;
    static int dcf77_dayow = 0;
    static bool dcf77_parity = false;
    static bool dcf77_error = true;

    if (dcf77bit==MINUTE)
    {
        if ((cnt==59) && (!dcf77_error))
        {
            tstruct t;
            t.second = 0;
            t.minute = dcf77_minute;
            t.hour = dcf77_hour;
            t.dayow = dcf77_dayow-1;
            rtc_set_time(&t);
        }
        cnt=0;
    }
    else
    {
        switch (cnt)
        {
            case 0:  //break; // 0 allways
                dcf77_error=false;
                if (dcf77bit==HIGH)
                {
                    dcf77_error=true;
                    //uart_putc('X'); // error debug output line
                }
                break;
            case 1:  break; // 1 civil warning bits
            case 2:  break; // 1
            case 3:  break; // 1
            case 4:  break; // 1
            case 5:  break; // 1
            case 6:  break; // 1
            case 7:  break; // 1
            case 8:  break; // 1
            case 9:  break; // 1
            case 10: break; // 0
            case 11: break; // 0
            case 12: break; // 0
            case 13: break; // 0
            case 14: break; // 0
            case 15: break; // reserve antena
            case 16: break; // summer time
            case 17: break; // CEST
            case 18: break; // CET
            case 19: break; // leap second
            case 20: //break; // 1 allways
                if (dcf77bit==LOW)
                {
                    dcf77_error=true;
                    //uart_putc('X'); // error debug output line
                }
                break;
            case 21: //break; // 1 minutes
                dcf77_minute = 0;
                dcf77_parity = false;
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 1;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 22: //break; // 2
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 2;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 23: //break; // 4
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 4;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 24: //break; // 8
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 8;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 25: //break; // 10
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 10;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 26: //break; // 20
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 20;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 27: //break; // 40
                if (dcf77bit==HIGH)
                {
                    dcf77_minute += 40;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 28: //break; // even parity over minute bits 21 - 28
                if (((dcf77bit==HIGH) && (!dcf77_parity)) ||
                    ((dcf77bit==LOW) && (dcf77_parity)))
                {
                    dcf77_error=true;
                    //uart_putc('X'); // error debug output line
                }
                break;
            case 29: //break; // 1 hours
                dcf77_hour = 0;
                dcf77_parity = false;
                if (dcf77bit==HIGH)
                {
                    dcf77_hour += 1;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 30: //break; // 2
                if (dcf77bit==HIGH)
                {
                    dcf77_hour += 2;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 31: //break; // 4
                if (dcf77bit==HIGH)
                {
                    dcf77_hour += 4;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 32: //break; // 8
                if (dcf77bit==HIGH)
                {
                    dcf77_hour += 8;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 33: //break; // 10
                if (dcf77bit==HIGH)
                {
                    dcf77_hour += 10;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 34: //break; // 20
                if (dcf77bit==HIGH)
                {
                    dcf77_hour += 20;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 35: //break; // even parity over hour bits 29 - 35
                if (((dcf77bit==HIGH) && (!dcf77_parity)) ||
                    ((dcf77bit==LOW) && (dcf77_parity)))
                {
                    dcf77_error=true;
                    //uart_putc('X'); // error debug output line
                }
                break;
            case 36: //break; // 1 day of month
                dcf77_parity = false;
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 37: //break; // 2
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 38: //break; // 4
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 39: //break; // 8
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 40: //break; // 10
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 41: //break; // 20
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 42: //break; // 1 day of week (monday 1, sunday 7)
                dcf77_dayow = 0;
                if (dcf77bit==HIGH)
                {
                    dcf77_dayow += 1;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 43: //break; // 2
                if (dcf77bit==HIGH)
                {
                    dcf77_dayow += 2;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 44: //break; // 4
                if (dcf77bit==HIGH)
                {
                    dcf77_dayow += 4;
                    dcf77_parity = !dcf77_parity;
                }
                break;
            case 45: //break; // 1 month
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 46: //break; // 2
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 47: //break; // 4
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 48: //break; // 8
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 49: //break; // 10
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 50: //break; // 1 year within century
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 51: //break; // 2
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 52: //break; // 4
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 53: //break; // 8
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 54: //break; // 10
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 55: //break; // 20
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 56: //break; // 40
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 57: //break; // 80
                if (dcf77bit==HIGH) dcf77_parity = !dcf77_parity;
                break;
            case 58: //break; // even parity over bits 36 - 58
                if (((dcf77bit==HIGH) && (!dcf77_parity)) ||
                    ((dcf77bit==LOW) && (dcf77_parity)))
                {
                    dcf77_error=true;
                    //uart_putc('X'); // error debug output line
                }
                break;
            default: break;
        }
        cnt++;
    }
}
