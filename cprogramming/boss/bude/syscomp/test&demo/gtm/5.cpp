#include <iostream>
using namespace std;

class A {
public:
  void add(int x) {
	cout << x << endl;
  }
 A(int i) {
   cout << i << endl;
 }

protected:
  A(); 
};

class B:public A {
public:
    void sub(int x) {
	cout << "x=" << x << endl;
    }
public:
    B() {}
};


int main()
{
   A a(5);
//   B b;
}
