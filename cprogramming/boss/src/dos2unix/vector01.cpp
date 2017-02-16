#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

int main(int argc, char *argv[])
{
	vector<int>::iterator iter;
	
	vector<int> a;
	a.push_back(10);
	a.push_back(15);
	a.push_back(20);
	a.push_back(21);
	a.push_back(21);
	a.push_back(21);
	a.push_back(21);
	a.push_back(21);
	a.push_back(21);
	a.push_back(81);

	iter = lower_bound(a.begin(), a.end(), 100);
	if (iter != a.end()) {
		cout << *iter << endl;
	} else {
		cout << "not found" << endl;
	}


	a.clear();
	a.push_back(100);
	a.push_back(20);
	a.push_back(120);
	a.push_back(30);
	a.push_back(40);
	a.push_back(80);
	a.push_back(5);
	a.push_back(3);
	a.push_back(6);
	a.push_back(134);

	partial_sort(a.begin(), a.end(), a.end());
	//partial_sort(a.begin(), a.begin()+5, a.end());
	//sort(a.begin(), a.begin()+5);


	for (iter = a.begin(); iter != a.end(); iter++)
		cout << *iter << endl;
    a.resize(10);

 cout << "a.size()=" << a.size() << endl;

  for (iter = a.begin(); iter != a.end(); iter++)
              cout << *iter << endl;
    a.resize(5);
      cout << "a.size()=" << a.size() << endl;
        for (iter = a.begin(); iter != a.end(); iter++)
              cout << *iter << endl;
    vector<int>::size_type i = a.capacity();
      cout << "capacity="  << i << endl;
        cout << "max_size=" << a.max_size() << endl;


      char s[4];
        char p[4];

          s[0]='h';
            s[1]='z';
              s[2]='s';
                s[3]='G';

                  p[0] = 'Q';
                    p[1] = 0;

                      cout << "s=" << s << endl;
                        char hh[3];
                          cout << "hh.size=" << sizeof(hh) << endl;
                            string pu;
                              pu = "hengsg";

                                cout << "size=" << pu.size();
                                  cout << "len=" << pu.length();
}

