#include <iostream>
#include <vector>
using namespace std;

#include <list>
int main()
{
    int ai[] = { 10, 20, 110, 200, -500, 999, 333};
//    const int size = sizeof(ai) / sizeof(ai[0]);

    for (auto &elt : ai)
        elt += 100;

    for (auto i : ai)
        cout << i << " ";
    cout << endl;

    list<int> li(begin(ai), end(ai));
    for (auto &elt : li)
        elt += 100000;
    for (const auto &elt : li )
        cout <<  elt << " ";
    cout << endl;

    for ( auto i : { 100, 200, 300, 400})
        cout << i << " ";
}
