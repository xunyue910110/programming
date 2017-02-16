#include <iostream>
#include <list>

using namespace std;

int main(int argc, char *argv[])
{
	list<int *> aa;
	
	int i = 6;
	int j = 8;
	int z = 9;
	int y = 4;


	aa.push_back(&i);
	aa.push_back(&j);
	aa.push_back(&z);
	aa.push_back(&y);

	for (list<int *>::iterator iter = aa.begin(); iter != aa.end(); iter ++)
		cout << **iter << '\n';

	cout << aa.size()  << endl;

	return 0;
}
