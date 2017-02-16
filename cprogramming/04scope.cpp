#include <iostream>
using namespace std;
namespace A {
 struct X {};
 void g(X) { cout << "CALL from A;";}
}

class C {
  public:
// void g(A::X) { cout << "CALL from C;";}
 void j() {
   A::X x;
   g(x);
 }
};

int main() {
  C c; c.j();
}
