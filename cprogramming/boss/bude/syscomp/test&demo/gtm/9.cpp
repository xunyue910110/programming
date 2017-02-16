#include <iostream>
using namespace std;

class A {
public:
	typedef int (A::*PtrFunc)(int);
	/*
	int f() {
		int i = 100;
		cout << "in A::f(), i = 100 " << endl;
		return i;
	}
	*/
	int g(int i) {
		cout << "in A::g(int), i = " << i << endl;
		return i;
	}
};

class B {
public:
	//typedef int (A::*PtrFunc)(int);
	B(A::PtrFunc p):ptrfunc(p){}
	void run() {
		(*ptrfunc)(100);
	}
private:
	//PtrFunc ptrfunc;
	A::PtrFunc ptrfunc;
};

int main(int argc, char *argv[])
{
	B b(A::g);
	b.run();
}
