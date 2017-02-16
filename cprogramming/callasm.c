#include <stdio.h>
int add(int a, int b){
  return a+b;
}

int main(){
  int nRet;
  asm{
    push 1
    push 2
    call add
    mov nRet, eax
  }
  //std::cout << "nRet:" << nRet<<std::endl;
  printf("nRet:%\n", nRet);
  return 0;
}
