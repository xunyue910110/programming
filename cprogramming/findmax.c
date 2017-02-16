#include <stdio.h>
#define max2(x,y) ((x)>(y)?(x):(y))
#define max3(x,y,z) ( (x)>(y)?(((x)>(z))? (x) : (z)): (((y)>(z))?(y):(z)))

int maxsum3(int *x, int l, int u)
{
    if (l > u)  
        return 0;
    if (l == u)
        return max2(0 , x[l]);
    int m = (l+u)/2;
    int sum;
    int lmax = sum = 0;
    int i ;
    for (i = m; i >=l; i--)
    {
        sum += x[i];
        lmax = max2(lmax, sum);
    }

    int rmax = sum = 0;
    for (i =m+1; i<=u; i++)
    {
        sum += x[i];
        rmax = max2(rmax, sum);
    }

    return max3(lmax+rmax, maxsum3(x, l,m), maxsum3(x, m+1,u));
}
int findmax(int *a, int n)
{
   return maxsum3(a, 0,n-1); 
}
int main()
{
    int a[] = { 31, -41, 59, 26, -53, 58, 97, -93, -23, 84};

    int max = findmax(a, 10);
    printf("%d\n", max); 
    return 0;
}
