#include <iostream>
#include <stdio.h>
using namespace std;

class A {
public:
    long a;
    A(){cout << "A constructor" << endl; this->func1();}
    virtual void func1() { cout << "A::func1() " << endl; func4();}
    void func2() { cout << "A::func2() " << endl;}
    virtual void func3() { cout << "A::func3() " << endl;}
    void func4() { cout << "A::func4() " << endl;cout << sizeof(*this) << endl;printf("%x\n", this);this->func5();}
    virtual void func5() { cout << "A::func5() " << endl;}
};

class B : public A{
public:
    long b;
    long c;
    B(){cout << "B constructor" << endl; A::func1(); }
    void func1() { cout << "B::func1() " << endl; A::func4();}
    void func2() { cout << "B::func2() " << endl;}
    virtual void func3() { cout << "B::func3() " << endl;}
    void func5() { cout << "B::func5() " << endl;}
    void func4(int a) { cout << "B::func4() " << endl;cout << sizeof(*this) << endl;printf("%x\n", this);this->func5();}

};

int main()
{
    //A a;
    //cout << "sizeof(a)" << sizeof(a) << endl;
    B b;
    //A a (b);
    //A *pa = &a;
    //pa ->func5();
    //cout << "sizeof(b)" << sizeof(b) << endl;
    //printf("%x\n", &b);
    //A *p = &b;
    //p->func1();
    //b.A::func4();
    return 0;
}
