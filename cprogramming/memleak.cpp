#include <stdio.h>
#include <stdlib.h>

void getMem(void **p){
  *p = new int[16] ;
}

int main(){
  void *p = NULL;
  getMem(&p);
  //free(p);
  //delete (int*)p;
  //delete [] (int*)p;
  delete [] reinterpret_cast<int*>(p);
  p = NULL;
  return 0;
}
