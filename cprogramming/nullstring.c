#include <stdio.h>
#include <string.h>

int main()
{
    char buff[] = "\0";
    printf ("%d\n", sizeof(buff)); 
    printf ("%d\n", strlen(buff));

    char * p22 = "\0";
    if ( p22)
    printf("%p\n", p22);
    printf("null size: %d\n", sizeof(NULL));
    return 0;
}
