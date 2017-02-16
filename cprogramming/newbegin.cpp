#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;
#include <cstring>

bool strLenGT4(const char*s){

    return strlen(s) > 4; }

int main()
{
    int arri[] = { 2, 3, 1, 5, 4 };
    auto first3 = find (begin(arri), end(arri), 3);
    if ( first3 != end(arri))
        cout << "First 3 in v = " << *first3 << endl;

    const char *names[] {"Huey", "Dewey", "Louie"};
    auto firstGT4 = find_if(begin(names), end(names), strLenGT4);
    if ( firstGT4 != end(names))
        cout << "First long names : " << *firstGT4 << endl;
}
