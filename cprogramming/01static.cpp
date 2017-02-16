#include <iostream>
using namespace std;
class dog{
  public:
  //void bar() { cout << "hello " << a << endl;}
  virtual ~dog() {}
  virtual void bark(){}
};

class yellowdog : public dog {
  int age;
  public:
  void bark( ) { cout << "woof. " << age  << endl;}
};
int main() {

  dog * pd = new dog();
//  dog * py = new yellowdog;
  yellowdog * py = static_cast<yellowdog*> (pd);
  if ( py )
    py ->bark();
  cout << " py = " << py << endl;
  cout << " pd = " << pd << endl;
}
