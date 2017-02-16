#include <stdio.h>
#include <stdlib.h>


int intcomp(int *x, int *y)
{
    return *x - *y;
}
int a[1000000];

int  main()
{
    int i= 0;
    int n = 0;

    while ((scanf("%d", &a[n]) != EOF))
        n++;

    qsort(a, n, sizeof(int), intcomp);
    i=0;
    while (i<n)
    {
       printf("%d\n", a[i]); 
        i++;
    }
    return 0;
}
