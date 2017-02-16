#include <stdio.h>
int myglobal;

int& foo() {
 return myglobal;
}
int main() {
  foo() = 50;
  printf("%d", myglobal);
}

