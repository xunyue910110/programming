#include <iostream>
using namespace std;

int main()
{
  cout.precision(2);
  cout.setf(ios::fixed);
  cout.setf(ios::showpoint);
  double d = 123;
  cout << d << endl;
}
