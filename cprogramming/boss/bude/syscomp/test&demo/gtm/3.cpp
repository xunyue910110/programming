#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
int main(int argc, char *argv[])
{
	vector<int> vecInt;

	vecInt.push_back(1);
	vecInt.push_back(0);
	vecInt.push_back(5);
	vecInt.push_back(4);
	vecInt.push_back(8);
	vecInt.push_back(9);
	vecInt.push_back(4);
	vecInt.push_back(3);
	vecInt.push_back(1);
	vecInt.push_back(100);

	for (int i = 0; i < vecInt.size(); i++)
		cout << vecInt[i] << endl;

	cout<< "------------------" << endl;


	::sort(vecInt.begin(), vecInt.end());

	  for (int i = 0; i < vecInt.size(); i++)
                cout << vecInt[i] << endl;


	cout << "==================" << endl;



	vector<int>::iterator iter = ::unique(vecInt.begin(), vecInt.end());
	vecInt.erase(iter, vecInt.end());

	  for (int i = 0; i < vecInt.size(); i++)
                cout << vecInt[i] << endl;


	cout << "HHHHHHHHHHHHHHHHHHHHHHHHHHH" << endl;

	vecInt.clear();
	cout << "ooooooooooooooooooooo" << endl;
	cout << "vecInt.size()=" << vecInt.size() << endl;
 for (int i = 0; i < vecInt.size(); i++)
                cout << vecInt[i] << endl;

	return 0;
}
