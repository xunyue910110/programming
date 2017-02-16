#include <iostream>
#include <typeinfo>
using namespace std;

class P{
public:
  virtual void show(){}
};

class Teacher : public P{};
class Student : public P{};
class CppTeacher : public Teacher{};

class Computer{
public :
  virtual void show(){}
};

class  Notebook : public Computer{};
int main(){
 Teacher t ;
  CppTeacher cp;
  Student s;
  Computer * c = new Notebook;
 P *p = &t;
  //错误，没有虚函数
 // Notebook* n = dynamic_cast<Notebook*>(c);
  Teacher *pt = dynamic_cast<Teacher*>( p);
  if (pt != NULL) cout << " cast successfull " << endl;
  cout <<typeid(new Teacher).name()<<endl;
  if (typeid(*pt) == typeid(cp))
    cout << " equal" <<endl;
  cout <<  typeid(Notebook*).name() << endl;
  cout << boolalpha<<(typeid(*c)  == typeid(Notebook)) << endl;
}
