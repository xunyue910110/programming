#include <stdio.h>

#include <unistd.h>
int main()
{
    void *p1 = sbrk(4);
    void *p2 = sbrk(4);
    void *p3 = sbrk(4);
    int *p4 = sbrk(4);
    printf("%p\n", p1);
    printf("%p\n", p2);
    printf("%p\n", p3);
    printf("%p\n", p4);
    *p4 = 100;
    sbrk(-4);
    sbrk(-8);
    void *p5 = sbrk(0);
    printf("%p\n", p5);
    printf("%d\n", getpid());
    printf("%d\n", *p4);
    void *p6  = sbrk(4093);
    printf("%p\n", p6);
    sbrk(-1);
    while(1);
}

