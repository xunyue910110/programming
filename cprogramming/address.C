#include <iostream>
using namespace std;
const int add(int a, int b)
{
  return a+b;
}
int main()
{
  char c = '9';
  cout << &c << endl;

  int i = 2;
  cout << &i << endl;

  int *p = &i;
  cout << p << endl;

  cout << reinterpret_cast<int*>(&c) << endl;


  const int& rci = 2;
  cout << rci << endl;

  int x = 20;
 int & y = x;
  
 const int & rab  = 10 + 20;
 const int &ret = add(10,20);
  cout << ret;

}

