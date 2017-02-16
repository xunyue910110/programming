#include <stdio.h>

int bitsmap[10] = {0};

#define set(nth, v) bitsmap[nth>>4] |= ( v << (nth%16*2))
#define get(nth) (bitsmap[nth>>4] >> (nth&0x0F*2)) & 0x3

int main()
{
    set(159, 3);
    printf("%x\n", bitsmap[9]);
    printf("%d\n", get(159));
    return 0;
}

