#include <iostream>
using namespace std;
class A{
  int ma;
};
class B: public A{
  int mb;
};

class C: public A{
  int mc;
};

class D: public B , public C{
  int d;
};

int main()
{
  cout << sizeof(D) << endl;
  return 0;
}
