#include <iostream>
#include <list>
using namespace std;


class A {
public:
A() { i = 0; j = 0; }
void add() {};
private:
int i;
int j;
};

int main()
{
	list<A *> aa;

	A a, b, c, d;

	aa.push_back(&a);
	aa.push_back(&b);
	aa.push_back(&c);
	aa.push_back(&b);
	aa.push_back(&c);
	aa.push_back(&d);

	cout << "size()=" << aa.size() << endl;
	aa.remove(&c);
	cout << "size()=" << aa.size() << endl;

	exit(0);
}   
