#include <stdio.h>
#include <stdlib.h>
struct st{
  int x;
  int y;
  int z;
};

int global_init = 2;
int global_uninit ;
int main()
{
  int x;
  int y;
  int z;

  int a[3] ;
 
  struct st s;
  printf("stack area:\n");
  printf("%p, %p, %p\n", &x, &y, &z);
  printf("%p, %p, %p\n", a, a+1, a+2);
  printf("%p, %p, %p\n", &s.x, &s.y, &s.z);

  int *pint = (int*)malloc(8);
  char *pchar = (char*)malloc(1);

  printf("heap area:\n");
  printf("%p, %p\n", pint, pchar);

  static int static_init =1;
  static int static_unit ;

  printf("uninitialized data seg:\n");
  printf("%p,%p\n", &global_uninit, &static_unit);


  printf("initialized data seg:\n");
  printf("%p,%p\n", &global_init, &static_init);

  printf("text seg:\n");
  printf("%p\n", &main);

  const char* string = "hello, world";
  printf("%p\n", string);

  const int ca = 10;
  const int *pa = &ca;
  printf("%p\n", pa);
  return 0;
}
