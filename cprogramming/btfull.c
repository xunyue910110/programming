#include <stdio.h>
void fun_a(void)
{
      int a = 0;
          printf("%d\n", a);
}
void fun_b(void)
{
      int b = 1;
          fun_a();
              printf("%d\n", b);
}
void fun_c(void)
{
      int c = 2;
          fun_b();
              printf("%d\n", c);
}

void fun_d(void)
{
      int d = 3;
          fun_c();
              printf("%d\n", d);
}
int main(void)
{
      int var = -1;
          fun_d();
              return 0;
}
