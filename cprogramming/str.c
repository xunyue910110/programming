#include <stdio.h>
extern int x ;
int main()
{
    char *s1 = "abc";
    char *s2 = "abc";
    char s3[] = "abc";
    char s4[] = "abc";
    printf("%p, %p\n", s1, s2);
    printf("%p, %p\n", s3, s4);
    printf("%p, %p\n", &s3, &s4);

    printf("\n");
    printf("%p, %p\n", s3+1, s4+1);
    printf("%p, %p\n", &s3+1, &s4+1);

    printf("address of main: %p\n", main);
    printf("address of main+1: %p\n", main+1);

    x = 11;
    printf("x: %d\n", x);
}
