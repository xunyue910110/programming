#include <stdio.h>

extern int etext, edata, end;

void main(void)
{
    printf("etext: %6X \t edata: %6X \t end: %6X\n",
        &etext, &edata, &end);
}
