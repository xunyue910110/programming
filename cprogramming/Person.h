#ifndef __PERSON_H
#define __PERSON_H

#include <string>
using namespace std;
class Person
{
private:
  string name;
  int year;
public:
  Person(const string& name, int year);
  void show();
};

#endif
