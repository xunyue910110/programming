#include <stdio.h>
#include <stdlib.h>
#define swap(a,b) {int t = *a; *a=*b;*b=t;}
int delspace(char *s)
{
    char *p = s;
    char *q = p;
    int cnt = 0;

    while (*q != '\0' && *p != '\0')
    {
        while(*p != ' ' && *p != '\0')
            p++;
        if (*p == '\0')
            return 0;
        q = p;
        while (*q == ' ' && *q != '\0')
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
    char s[] = "ABCDEFG";
    int cnt = delspace(s);
    printf("%s|\n", s);
    printf("%d\n", cnt);

    int a = 1;
    int b = 2;
    swap(&a, &b);
    printf("%d, %d\n", a, b);
    return 0;
}
