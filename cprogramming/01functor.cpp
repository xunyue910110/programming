#include <iostream>
using namespace std;

class X {
  public:
    X(int i) {}
    void operator () ( string str) { cout << "oeprator ()";}
};

int main() {
  X(8)("HI");
}
