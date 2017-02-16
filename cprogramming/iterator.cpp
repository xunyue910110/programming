#include <iostream>
#include <vector>
using namespace std;

#include <list>
int main()
{
    int ai[] = { 10, 20, 110, 200, -500, 999, 333};
    const int size = sizeof(ai) / sizeof(ai[0]);

    for (int i = 0; i < size; ++i)
        cout << ai[i] << " ";
    cout << endl;

    list<int> li(ai, ai+size);
    for (list<int>::iterator it =
        li.begin(); it != li.end(); ++it)
        *it += 100000;
    for (list<int>::const_iterator it = 
        li.begin(); it!= li.end(); ++it)
        cout <<  *it << " ";
    cout << endl;
}
