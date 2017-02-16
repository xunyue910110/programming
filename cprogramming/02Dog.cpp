#include <iostream>
using namespace std;

class dog {
public:
  string m_name;
  dog(string name) { m_name = name; cout << name << " is born." << endl;}
  ~dog () { cout << m_name << " is distroied." << endl; throw 20;}
  void PrepareToDestory() { cout << "Prepare to destory" << endl; throw 20;}
  void bark() { }
};


int main() {

 try {
   dog dog1 ( "Henry") ;
   dog dog2 ( "Bod ");
   dog1.bark();
   dog2.bark();
//   dog1.PrepareToDestory();
 //  dog2.PrepareToDestory();
 }catch ( int e )
 {
   cout << e  << " is caught"  << endl;
 }
}
