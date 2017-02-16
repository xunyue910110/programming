#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#define RAND_MAX 10000000

int main()
{
    int i = 0;
    srand(time(NULL));
    for (i=10000000; i > 0; i--)
    {
       printf("%d\n", i);
    }
    return 0;
}
