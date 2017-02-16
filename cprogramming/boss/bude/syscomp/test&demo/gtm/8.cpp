#include <iostream>
using namespace std;

class A {
public:
	A(int x):a(x) 
	{
		cout << "A::A a=[" << a << "]" << endl;
	}

	virtual void f() {
		cout << "A::f" << endl;
	}
	
	virtual void g() {
		cout << "A::g" << endl;
	}

	void z() {
		cout << "A::z" << endl;
	}

	/*
	void h() {
		cout << "A::h" << endl;
	}
	*/

	//~A() {
	virtual ~A() {
		cout << "A::~A" << endl;
	}

protected:
	int a;
};

class B:public A {
public:
	B(int x, int y):A(x), b(y)
	{
		cout << "B:B a=[" << a << "] b=[" << b << "]" << endl;
	}
	void f() {
		cout << "in b...[" << endl;
		A::f();
		cout <<  "]" << endl;
		cout << "B::f" << endl;
	}

	void g() {
		cout << "B:g" << endl;
	}

	void z() {
		cout << "B::z" << endl;
	}

	void h() {
		cout << "B::h" << endl;
	}

	~B() {
		cout << "B::~B" << endl;
	}

private:
	int b;
};


int main()
{
	A *p = new B(8,6);
	p->f();
	p->g();
	p->z();
	delete p;
	
	cout << "===================================" << endl;
	B *b = new B(10, 11);
	b->f();
	b->g();
	b->z();
	delete b;

	cout << "==================================" << endl;
	A *d = new B(20, 10);
	//d->h();
	delete d;

	cout << "===================================" << endl;
	B *d0 = new B(21, 11);
	d0->h();
	delete d0;

	return 0;
}
