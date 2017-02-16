#include <stdio.h>
#include <string.h>
#include <unistd.h>
int main()
{
    void *p = sbrk(0);
    brk(p+4);   
    brk(p+8);
    brk(p+4);

    double *pd = sbrk(0);
    brk(pd+1);
    *pd = 12.0001;

    char *pc = sbrk(0);
    brk(pc+10);
    strcpy(pc, "zhangfei");
    printf("*pd=%lf\n", *pd);
    printf("pc=%s\n", pc);
    brk(pd);
}
