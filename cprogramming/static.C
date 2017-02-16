#include <iostream>
using namespace std;
int& counter()
{
  static int cnt;
  ++cnt;
  return cnt;
}

class CStatic{
  static int i ;
public:
  void show(){cout << i<< endl;}
  static void timefly(int year){ i = year;}
};

int getint(){return 200;}
int CStatic::i = getint();

int main()
{
  cout << counter() <<endl;
  cout << counter() <<endl;
  cout << counter() <<endl;
  counter() = 0;
  cout << counter() <<endl;
  cout << counter() <<endl;
  cout << counter() <<endl;
  CStatic c;
  c.show();
  c.timefly(2000);
  c.show();
  CStatic::timefly(30000);
  c.show();
}
