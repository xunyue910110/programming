#include "Person.h"

#include <iostream>
using namespace std;
Person::Person(const string& name, int year)
: name(name), year(year)
{
}
void Person::show()
{
  cout << "我是" << name << ", 今年" <<
    2013-year << endl;
}
