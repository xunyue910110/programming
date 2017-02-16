#include <stdio.h>
#include <stdlib.h>
#define swap(a,b) {int t = *a; *a=*b;*b=t;}
int delspace(char *s)
{
    char *p = s;
    char *q = p;
    int cnt = 0;

    while (*q != '\0' )
    {
        while(*p != ' ' )
            p++;
        if (*p == '\0')
            return 0;
        q = p;
        while (*q == ' ' )
            q++;
        if ( *q == '\0')
            break;
        swap(p, q);
        p++;
    }
    *p='\0';
    return q-p;
}

int main()
{
    char s[] = " a  b  ";
    int cnt = delspace(s);
    printf("%s|\n", s);

    int a = 1;
    int b = 2;
    swap(&a, &b);
    printf("%d\n", cnt);
    return 0;
}
