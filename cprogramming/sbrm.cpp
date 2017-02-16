#include <iostream>
#include <memory>
using namespace std;

class C{
public:
    C(int i): x(i){ }
    ~C() { cout <<"C destructor" <<endl;}
    
private:
    int x;
};

class SBRM
{
public:
    SBRM(C* c) : pc(c){}
    ~SBRM(){delete pc;}
    operator C*(){return pc;}
    private:
    C* pc;
};

class Base
{
public:
    virtual void Print(){ cout << "Print from base\n"; }
};

class Derive : public Base
{
public:
    virtual void Print(){ cout << "Print from Derive\n"; }

};

int main()
{
/*
    for (int i = 0; i  < 100; ++i)
    {
        C* c = new C(i);
        delete c;
    }
*/
/*
    for (int i = 0; i < 100; ++i)
    {
        SBRM cc(new C(i));
        C* c = cc;
    }
*/
    //C c{1};
    shared_ptr<Base> pb ( new Base());
    pb->Print();
    shared_ptr<Derive> pd ( new Derive());
    pd->Print();
    shared_ptr<Base> pb2 ( new Derive());
    pb2->Print();
    shared_ptr<Base> pb3(pb2);
    pb3->Print();
}
