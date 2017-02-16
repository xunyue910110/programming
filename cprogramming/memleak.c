#include <stdio.h>
#include <stdlib.h>

void getMem(void **p){
  *p = malloc(16);
}

int main(){
  void *p = NULL;
  getMem(&p);
  free(p);
  //delete (int*)p;
  p = NULL;
  return 0;
}
