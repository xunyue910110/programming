#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("%d\n", sizeof(unsigned long));
    printf("%d\n", sizeof(unsigned long*));
    printf("%d\n", sizeof(1));
    int*p = malloc(sizeof(int));
    return 0;
}
