#include <stdio.h>
char *a = "* + 7 * * 4 6 + 8 9 5"; 
//char *a = "+76 11"; 
int i = 0;
int eval()
  { int x = 0;
    while (a[i] == ' ') i++;
    if (a[i] == '+')
      { i++; return eval() + eval(); }
    if (a[i] == '*')
      { i++; return eval() * eval(); }
    while ((a[i] >= '0') && (a[i] <= '9')) 
      x = 10*x + (a[i++]-'0'); 
    return x;
  }

int main()
{
    printf("%d\n", eval());
return 0;
}
