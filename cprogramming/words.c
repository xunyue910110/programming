#include <iostream>
#include <string>
#include <set>
using namespace std;

int main()
{
    set<string> s;
    set<string>::iterator j;
    set<string>::iterator end;
    string t;
    while (cin >> t)
        s.insert(t);
    j = s.begin();
    end = s.end(); 
    while (j != end)
        cout << *j++ << "\n";
    return 0; 
}
