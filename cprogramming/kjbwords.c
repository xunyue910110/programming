#include <iostream>
#include <map>
#include <string>
using namespace std;

int main()
{
    map<string, int> m;
    map<string, int>::iterator j;
    string t;

    while ( cin >> t)
    {
        m[t]++;
    }
    
    int i = 0;
    for (j = m.begin(); j != m.end(); j++)
    {

        cout << j->first << " " << j->second << endl;
    }
    return 0;
}
