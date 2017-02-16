#include <stdio.h>
int lg(int );

int main()
{
    //printf("%d\n", lg(8));
    int i ;
    int n ;
    for (i = 1, n = 10; i <= 6; i++, n *= 10)
    {
        printf("%7d %2d %9d\n",n, lg(n), n*lg(n));
    }
    return 0;
}

int lg (int n )
{
    int i = 0;
    while ( n >= 2)
    {
        i++;
        n /= 2;
    }
    return i;
}
