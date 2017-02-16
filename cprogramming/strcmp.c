//#include <string.h>
#include <stdio.h>

int strcmp(char *p1, char *p2)
{
    while (*p1 == *p2){
        if (*p1 == 0 || *p2 == 0) break;
        p1++; 
        p2++;
    }
    //if (*(p1-1) == 0) return 0;
        return *(p1) - *(p2);
}
int main()
{
    char *p1 = "";
    char *p2 = "1asfjlslfj";
    //printf("%d \n", strncmp(p1, p2, strlen(p2)));
    printf("%d \n", strcmp(p1, p2));
    return 0;
}
