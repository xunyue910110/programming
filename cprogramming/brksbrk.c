#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
    int *pi = sbrk(sizeof(int));
    *pi = 100;
    double *pd = sbrk(sizeof(double));
    *pd = 12.00010;
    char *pc = sbrk(10);
    strcpy(pc, "zhangpi");
    printf("%d\n", *pi);
    printf("%lf\n", *pd);
    printf("%s\n", pc); 
    brk(pi);
}
