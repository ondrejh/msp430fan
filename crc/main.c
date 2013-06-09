#include <stdio.h>
#include <inttypes.h>

#include "../crc8.h"

uint8_t data[9] = {0,0,0,0,0,0,0,0,0};

int main(int argc, char *argv[])
{
    int i;

    if (argc>1)
    {
        int cnt = argc;
        if (cnt>9) cnt=9;
        for (i=1;i<argc;i++)
        {
            int h;
            if ((sscanf(argv[i],"%d",&h)==1) && (h>=0) && (h<256)) data[i-1]=h;
        }
    }

    printf("Input data:");
    for (i=0;i<8;i++) printf(" %d",data[i]);
    printf(" Crc: %d\n",data[8]);
    uint8_t crc = crc8(&data[0],8);
    printf("Calculated Crc: %d .. ",crc);
    if (crc==data[8])
    {
        printf("fits\n");
        return 0;
    }
    else
    {
        printf("doesn't fit\n");
        return -1;
    }
}
