#include <stdio.h>

int main()
{
    int a = 0b1001;
    int b = 01001;
    int c = 0x1001;
    //float d = 0x1001.1;
    float e = 1.0011e-1;
    printf ("%d\n", a);
    printf ("%d\n", b);
    printf ("%d\n", c);
   // printf ("%f\n", d);
    printf ("%f\n", e);
    char f = 0x80;
    printf("%d\n", f);
    short g = -32768;
    printf("%hd\n", g);
    g = -g;
    printf("%hd\n", -g);
    char h = -128;
    printf("%hhd\n", h);
    printf("%hhd\n", -h);

    short x = 5;
    char y = -6;
    printf("%hhd\n", x + y);
    short a1 = 128+3;
    printf("%hd\n", a1);
    char a2 = a1;
    printf("%hhd\n", a2); 

    unsigned char c1 = 0b11001101;
    unsigned char c2 = c1 >> 1;
    printf("%hhx\n", c2); 
    return 0;
}
