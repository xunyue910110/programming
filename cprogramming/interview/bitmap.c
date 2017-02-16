#include <stdio.h>

#define BSIZE  1024*1024
#define bit_isset(bitmap, bit) \
    bitmap[(bit) / (sizeof(unsigned long)*8)] & ((long)0x1 << ((bit) % (sizeof(unsigned long)*8)))
#define bit_set(bitmap, bit) \
    bitmap[(bit) / (sizeof(unsigned long)* 8)] |= ((long)0x1 << ((bit) % (sizeof(unsigned long)* 8)))
#define bit_unset(bitmap, bit)  \
    bitmap[(bit) / (sizeof(unsigned long)* 8)] &= ~((long)0x1 << ((bit) % (sizeof(unsigned long)* 8)))

unsigned long bitmap[BSIZE];

int main()
{
    //unsigned long a = 0x1;
    bit_set(bitmap, 1024*1024*64-1);
    if (bit_isset(bitmap, 1024*1024*64-1))
        printf("seted\n");
    bit_unset(bitmap, 319);
    //printf("%d\n", sizeof(unsigned long));
    return 0;
    }
