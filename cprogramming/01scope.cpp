#include <iostream>
using namespace std;
using std::cout ;

class B{
  public:
    void f(int a){}
};

class D : private B {
public:
  void g() {
    using namespace std;
   cout << "From D: \n " ;
  }

  void h() {
   using std::cout ;
  cout << "From D:\n";
  }

  using B::f;
};


int main(){
D d;
d.f(8);
}
