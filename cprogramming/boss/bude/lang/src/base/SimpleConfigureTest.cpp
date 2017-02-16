#include "SimpleConfigure.h"
#include <iostream>
#include <cstring>
using namespace std;
int main()
{
    SimpleConfigure sc;
    string s("myconfig.ini");
    sc.initialize(s.c_str());
    sc.setremark('#');
    cout << sc.getval("DB", "USER");
    char buf[20];
    sc.getval("GENERAL1", "LIMITNUM",  buf, 20);
    cout << buf;
    return 0;
}
