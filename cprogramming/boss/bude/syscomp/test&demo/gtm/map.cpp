#include <iostream>
#include <map>

using namespace std;

class A {
public:
A() { i = 5; }
A(int p):i(p) {}
bool operator<(const A a) {
	return i < a.i;
}
friend ostream& operator<< (ostream& os, A a)
{
	os << a.i ;
	return os;
}
private:
int i;
};


int main()
{

	A aa(100);	
	pair<A,string> a;
	a = make_pair(aa, string("heng"));
	cout << a.first << "=" << a.second << endl;
	

	map<string,long>::iterator iter;
	map<string,int>::iterator it;

	map<string,long> mp;

	mp["heng"] = 100;
	mp["zheng"] = 200;
	mp["guo"] = 50;

	cout << "size=" << mp.size() << endl;

	for (iter = mp.begin(); iter != mp.end(); ++iter)
		cout << iter->first << "=" << iter->second << endl;

	pair<string, long> aa0("tst", 150);
	cout << aa0.first << "=" << aa0.second << endl;
	
	map<string, int> bb;
	cout << "b.size=" << bb.size() << endl;
	int c = bb["heng"];
	cout << "c=" << c << " bb.size=" << bb.size() << endl;
	bb["heng"] = 34;
	it = bb.find("heng");
	cout << it->second << endl;
	
	return 0;
}
