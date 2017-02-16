#include <string>
#include <iostream>
using namespace std;
  class MyExc{
string str;
public:
  MyExc(const string&name):str(name){}
  string what(){ 
    return str;
  }
};
void beforedie(){cout << "共产党万岁" << endl;}
void g() throw( MyExc)
{
/*
  throw 1;
  throw 34.4;
  throw 'c';
*/
  throw MyExc("userdefinedexception");
}


int main()
{
  set_terminate(beforedie);
  try{
  g();
  }catch(MyExc& e){
    cout <<e.what();
  }
}
