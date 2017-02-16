#include <iostream>
using namespace std;

class P{
string name;
public:
  P(const char*name = "\0"): name(name){
    cout << "P(" << name << ")\n";
  }
  ~P(){cout << "~P()" << endl;}
  void show(){
  cout << "大家好，我是" << name << endl;
  }
  const string & getname(){return name;} 
};

class T :public P{
string course;
public:
  T(const char *n, const char* c):P(n),course(c){
    cout << "Teacher(" << n << ","<<c<< ")\n";
  }
  ~T() {
    cout << "~T()" << endl;
  }
  void show(int){
    cout << "同伙们好，我是" << getname()   << ",教" << course << "的" << endl;
  }
};

int main()
{
  P("芙蓉").show();
  T ("权哥", "CPP").P::show();
  //cout << sizeof t;
  
}
