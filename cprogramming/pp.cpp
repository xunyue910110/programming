#include <iostream>
using namespace std;

class R{
int n;
int d;
public:
  R(int n = 1, int d = 1): n(n), d(d){}
  friend ostream& operator<<(ostream&o, const R&x){
   return o << x.n << "/" << x.d ;
  }
friend R& operator++(R& r){ 
  r.n+= r.d;
  return r;}
 /* R& operator++(){
    n += d;
    return *this;
  }
*/
  R operator++(int){
    R r = *this;
    n+=d; 
    return r;
  }
};

int main()
{
  R r (3, 4);
  cout << ++r << endl;
  cout << r++ << endl;
  cout << r << endl;
  cout << ++++++r << endl;
}
