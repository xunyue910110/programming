#include <stdio.h>

int max(int *a, int l, int r)
{
    if ( l == r )   return a[l];
    int m = (l+r)/2;
    int u = max (a, l, m);
    int v = max (a, m+1, r);
    return u > v ? u : v;
}

int main()
{
    int a[] = {10, 9, 8, 17, 6, 5, 4, 3, 2, 1};

    printf("%d\n", max(a, 0, 9));
    return 0;
}
