#include <stdlib.h>
#include <iostream>
#include <new>
void NoMoreMem() {
  std::cerr << "Unable to allocation " << std::endl;
  abort();
}
int main() {
  std::set_new_handler(NoMoreMem);
  int *p = new int [1000000000000L];
  delete [] p;
}
