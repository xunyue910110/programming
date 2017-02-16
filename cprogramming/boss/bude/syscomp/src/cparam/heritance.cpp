#include <stdio.h>

class A{
  int a;
  public:
    void Print();
    void SetValue(int a_){ a = a_;}
};
class B : public A{
  int b;
  public:
  void Print();
  void SetValue(int b_){b = b_;}
};

void A::Print(){
  printf("a=%d\n", a);
}

void B::Print(){
  printf("b=%d\n",b);
}

int main(){
  B b;
  b.SetValue ( 11);
  b.Print();
  
  b.A::SetValue ( 22);
  b.A::Print();
}
