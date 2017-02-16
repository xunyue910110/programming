int main()
{
#include <stdlib.h>
//    int *p = malloc(4);
 //   *(p+1024+1024) = 10;
 //
 /*   char *p = malloc(0);
    if ( p == NULL)
        perror("malloc");
    else
        printf("address %p\n", p);
if (p)
        free(p);
*/

#include <stdio.h>
    int a, b, c;
    printf("&a = %p, &b = %p, &c = %p\n", &a, &b, &c);
    
    int *p1 = malloc(4);
    int *p2 = malloc(4);
    int *p3 = malloc(4);
    printf("p1 = %p, p2 = %p, p3 = %p\n", p1, p2, p3);
    printf("&p1 = %p, &p2 = %p, &p3 = %p\n", &p1, &p2, &p3);
    printf("pid = %d\n", getpid());

    *(p1+1024*33-3) = 11;
    char *p4 = malloc(4096*33);
    *p4='d';
    char *p5 = malloc(4096*330);
    *p5 = 'c';
    //*(p1+1) = 0;
    //*(p1+3) = 0;
    free(p1);
    free(p2);
    free(p3);
    while(1) sleep(1);
}
