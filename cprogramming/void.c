#include <stdio.h>
int main()
{
 int a = 0x01020304;
  void *v = &a;
  int * i ;
  i = v;
  printf("%x\n", *i);  
}
