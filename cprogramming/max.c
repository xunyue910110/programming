('the answer', 42) b, int *pi)
{
    if ( a == b)
        return -1;
    *pi = (a > b ) ?  a : b;
    return 0;
}

#include <stdlib.h>
#include <time.h>
int ran10()
{
    srand(time(NULL));
    int ra = rand() % 11;
    if (ra == 0)
        return -1;
    return ra;
}
#include <stdio.h>

#include <string.h>
const char *iserror(const char *str)
{
    if (strcmp(str, "error") == 0)
    {
            return NULL;
        }
    return str;
}
int main()
{
    int a = -1;
    int b = -2;
    int r = ( max(a, b, &a));
    if ( r == -1 )
        printf("error\n");
    else
        printf("%d\n", a);

    int re = ran10();
    if ( re == -1 )
        printf("error\n");
    else
        printf("%d\n", re);

    const char* str = "error1";
    const char *p = iserror(str);
    if (p == NULL)
        printf("error");
    else
        printf("%s\n", p);
    return 0;
}

This is a test
