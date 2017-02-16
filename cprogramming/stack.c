#include <stdio.h>

int main()
{
    int a = 1;
    *(&a-10)  = 2;
    printf("%d",*(&a-1));
}
