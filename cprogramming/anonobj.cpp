#include <iostream>
#include <string>
using namespace std;

class A {
    public:
    A(){ cout << "A ctor" << endl;};
    A(const A& hr) { cout << "A copy-ctor" << endl;}
    ~A() { cout << "A desctor"<< endl;}
};


void g()
{
    A a ;
   // return a;
}
void f()
{
   // A a ; a = g();
   g();
   
}

int main()
{
    f();
    return 0;
}
