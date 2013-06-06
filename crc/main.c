#include <stdio.h>
#include <inttypes.h>

#include "crc8.h"

uint8_t data[9] = {1,0,0,0,0,0,0,0,0};

int main(void)
{
    printf("%d\n",crc8(&data[0],8));
    return 0;
}
