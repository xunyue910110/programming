#include <iostream>
using namespace std;

class NA
{
  int n;
  double d;
public:
  NA*operator &(){return 0;}
};

int main()
{
  NA na;
  cout << &na << endl;
}
