#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXN 10000000

typedef int DType;

DType realx[MAXN];
int *x = realx;
int n = 100000;

void swap(int i, int j)
{
    DType t = x[i];
    x[i] = x[j];
    x[j] = t;
}
/*
int randint(int l, int u)
{
    return l + (RAND_MAX * rand() * rand())%(u - l +1);
}
*/
int randint(int l, int u)
{   
    int i = l + abs(RAND_MAX*rand() + rand()) % (u-l+1);

    //printf("%d %d %d\n", l, u, i);
    return i;
}
int intcomp(int *x, int *y)
{
    return *x - *y;
}

/* INSERTION SORT */

/* Simplest insertion sort */
void isort1 ()
{
    int i , j;
    for ( i = 1; i < n; i++)
        for (j = i; j > 0 && x[j-1] > x[j]; j--)
            swap(j-1, j);
}

void isort3 ()
{
    int i , j;
    int t;
    for ( i = 1; i < n; i++)
    {
        t = x[i]; 
        for ( j = i ; j > 0 && x[j-1] > t; j--)
        {
            x[j] = x[j-1];
        }
        x[j] = t;
    }
}
int partition(int l, int u )
{
    int m = l ;
    int i ;
    int t = x[l];
    for ( i = l+1; i <= u; i++)
    {
        if (x[i] < t)
            swap(++m, i);
    }
    swap(l, m);
    return m;
}
void qsort1(int l, int u)
{
    if ( l >= u)
        return;
    /* split the array */
    int p = partition (l, u);
    qsort1( l, p-1);
    qsort1 ( p+1 , u); 
}

void qsort3(int l, int u)
{
    int i;
    int j;
    if (l >=u )
        return;

    int t = x[l]; i = l; j = u+1;
    while (1)
    {
        do i++; while ( i <=u && x[i] < t);
        do j--; while ( x[j] > t);
        if ( i > j)
            break;
        swap(i, j);
    }
    swap(l, j);
    qsort3(l, j-1);
    qsort3(j+1, u);
}

void qsort4(int l, int u)
{
    int i;
    int j;
    if (l >=u )
        return;
    swap(l, randint(l, u));
    //swap(l, (l+u)/2);
    int t = x[l]; i = l; j = u+1;
    while (1)
    {
        do i++; while ( i <=u && x[i] < t);
        do j--; while ( x[j] > t);
        if ( i > j)
            break;
        swap(i, j);
    }
    swap(l, j);
    qsort4(l, j-1);
    qsort4(j+1, u);
}

int nthmin(int l, int u, int k)

{
    int i;
    int j;
    if (l >=u )
        return;
    swap(l, randint(l, u));
    //swap(l, (l+u)/2);
    int t = x[l]; i = l; j = u+1;
    while (1)
    {
        do i++; while ( i <=u && x[i] < t);
        do j--; while ( x[j] > t);
        if ( i > j)
            break;
        swap(i, j);
    }
    swap(l, j);
    if ( j == k)
        return x[j];
    if ( j < k)
        return nthmin(j+1, u, k);
    else
        return nthmin(l, j-1, k);
}
int main()
{
    int i ;
    srand(time(NULL));
    for (i = 0;  i < MAXN; i++)
        x[i] = MAXN - i -1 ;
    //qsort4(0, n-1);
    //min value from [0,MAXN)
    //
    printf("%d\n", nthmin(0, n-1,2));
    //for ( i= 0; i < MAXN; i++)
     //   printf("%d\n", x[i]);
}
