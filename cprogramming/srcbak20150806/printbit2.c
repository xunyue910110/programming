#include <stdio.h>
void showvar(int var)
{
    int base = 0b10000000;
    int i;
    int j;
    char byte;
    char *p = (char *)&var;

    for (i = 0; i < 4 ; i++)
    {
        byte = *p;
        for (j = 0; j<8; j++)
        {
            putchar((byte & base) ? ('1') : ('0'));
            byte <<= 1;
        }
        p++;
        putchar(' ');
    }
    putchar('\n');
}

int main()
{
    int x = 2048;
    showvar(x);
    return 0;
}
