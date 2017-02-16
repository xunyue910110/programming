#include <iostream>
using namespace std;

class A {
public:
	int f() {
		int i = 100;
		cout << "in A::f(), i = 100 " << endl;
		return i;
	}

virtual	int g(int i) {
		cout << "in A::g(int), i = " << i << endl;
		return i;
	}
};

class B:public A {
public:
	int g(int i) {
		cout << "in B::g(int), i = " << i << endl;
		return i;
	}
};		

class C {
public:
	typedef int (A::*PtrFunc)(int);
	C(PtrFunc p, A *c):ptrfunc(p), a(c){}
	void run() {
		(a->*ptrfunc)(50);
	}
private:
	PtrFunc ptrfunc;
	A *a;
};

int main(int argc, char *argv[])
{
	A a;
	B b;
	C c(A::g, &a);
	c.run();

	C d(A::g, &b);
	d.run();
}
