#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

template<class Cont>
auto findNull(const Cont& c) ->  decltype(begin(c))
{
    auto it = begin(c);
    for (; it != end(c); ++it)
        if ( *it == nullptr)
            break;
    return it;
}
int main1()
{
    int a = 1000, b = 2000, c = 3000;
    vector<int *> vpi;
    vpi.push_back(&a);
    vpi.push_back(&b);
    vpi.push_back(&c);
    vpi.push_back(0);
    vector<int *> ::const_iterator cit = findNull(vpi);
    if (cit == vpi.end())
        cout << "no null pointers in vpi" << endl;
    else
    {
        vector<int*>::difference_type pos =
            cit - vpi.begin();
        cout << "null pointer found at pos." << pos << endl;
    }
}

int main()
{
    int a = 1000, b = 2000, c = 3000;
    vector<int*> vpi {&a, &b, &c, 0};
    auto cit = findNull(vpi);
    if (cit == vpi.end())
        cout << "no null pointers in vpi" << endl;
    else
    {
        auto pos =
            cit - vpi.begin();
        cout << "null pointer found at pos." << pos << endl;
    }
    
}
