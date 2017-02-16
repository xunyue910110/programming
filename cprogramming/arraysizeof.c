#include <stdio.h>

int main()
{
  char myArray[10][30];
  printf("%p\n", &myArray[0]);
  printf("%p\n", (&myArray[0]+1));
  printf("%p\n", myArray);
  printf("%p\n", myArray+1);

  printf("%p\n", myArray[0]);
  printf("%p\n", myArray[0]+1);
}
