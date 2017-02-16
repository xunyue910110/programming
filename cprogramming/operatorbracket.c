#include <iostream>
using namespace std;

class A{
int *p;
  int len;
public:
  A():p(0), len(0){}
  A(int n, int v = 0): p(new int[n]), len(n){
    for(int i = 0; i < n;i++) p[i] = v;
  }
  A& operator()(int n, int v ){
    for (int i = 0; i < n; i++)
      p[i] = v;
    return *this;
  }
  friend ostream& operator<<(ostream& o, const A& a)
  {
    for ( int i = 0; i < a.len; i++)
      o << a.p[i] << " ";
    return o;
  }
  int operator()(){
    int sum = 0;
    for (int i = 0; i < len; i++)
      sum += p[i];
    return sum;
  }
};

int main()
{
A a(5);
  a(5,1);
  cout << a;
  cout << a();
}
