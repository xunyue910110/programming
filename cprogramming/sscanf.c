#include <stdio.h>

int main()
{
    char *s6 = "a";
    int num2;
    char c;
    sscanf (s6, "%c", &c);
    num2 = c;
    printf("%d\n", num2);
    
    printf("\n");
    char *s = "1234567890";
    int num3;
    
    sscanf(s, "%s", &num3);
    sprintf((char*)&num3, "%s", s);  
    int x = 4444;
    printf("%s\n", &num3);
}
