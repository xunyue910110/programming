#include <iostream>
using namespace std;

int main(int argc ,char *argv[])
{
	enum Status {S_1 = -1, S_2 = 2, S_3 = 3 };

	Status s;
	s= S_1;	
	cout << "s=" << s << endl;

	s=S_2;
	cout << "s=" << s << endl;

	cout << "S_1=" << S_1 << endl;
	cout << "S_2=" << S_2 << endl;
	cout << "S_3=" << S_3 << endl;

	string s1="13243";
	cout << "s1=" << s1.size() << endl;	
	return 0;
}
