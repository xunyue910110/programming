#include <stdio.h>

int islend()
{
    unsigned int a = 0x12345678;
    unsigned char *p = (unsigned char*)&a;
    return *p == 0x78;
}
int main()
{
    printf("%d \n", islend());
    return 0;
}
