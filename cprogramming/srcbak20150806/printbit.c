#include <stdio.h>
void showvar(int var)
{
    int i = sizeof(var);
    int j = i*8;
    //char *p = &var;
    while (j>0)
{
    if ( (var & 0b10000000000000000000000000000000) == 0b0)
    printf("0");
    else 
    printf ("1");
    var <<= 1;
    j--;
}
}

int main()
{
    int x = 2048;
    showvar(x);
    return 0;
}
