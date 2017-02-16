#include <iostream>
using namespace std;

class R
{
private:
  int n;
  int d;
public:
  R(int n , int d): n(n),d(d){format();}
  int getn() const{return n;}
  int getd() const {return d;}
  int getn() {return n;}
  int getd() {return d;}
//  R multi (const R&right){
  R operator *(const R&right){
    int ln = n*right.n;
    int ld = d*right.d;
    /*
    int ret = gcd(ln, ld);
    if ( ret != 1)
      return R(ln/ret,ld/ret);
    else 
      return R(ln,ld);
    */
    return R(ln,ld);
  }
  R operator *(R&right){}
  int func(int){}
  int func(int&){}
/*
R operator+(const R&x)
{
  cout << "use member operator"<<endl;
  return R(n*x.d+d*x.n, d*x.d);
}
*/

friend 
R operator+(const R&y, const R&x)
{
  cout <<"use global"<<endl;
  return R(y.n*x.d+y.d*x.n, y.d*x.d);
}
  
 friend 
ostream& operator<< (ostream& o, const R& x);
friend
istream& operator>>(istream& i, R& x)
{
  char c;
  //i >> x.n >> c >> x.d;
  int n; 
  int d;
  i >> x.n >> c >> x.d;
   x.format();
}
private:
  int gcd(int a, int b ) {  
    if ( b == 0) 
     return a;
    return gcd( b, a%b);
  }
private:
/*
  R format(int a, int b)
  {

    if ( b < 0)
    {
      b = -b;
      a = -a; 
    }
    int ret = gcd(a, b);
    if ( ret != 1 )
      return R(a/ret, b/ret); 
    else
      return R(a, b);
  }
*/
  void format(){
    //if ( d < 0) { n = n*-1; d = d*-1;}
    int ret = gcd(n, d);
    //cout << "ret= " << ret << endl;
    if ( ret != 1 && ret != -1)
    {
      n = n/ret;
      d = d/ret;
    }
    if (  n/d <= 0 && d < 0 )
    {
        n = -n;d = -d;  
    }
  }
};
ostream& operator<< (ostream& o, const R& x)
{
  //o << x.getn() << "/" << x.getd() ;
  o << x.n << "/" << x.d;
  return o;
}

int main()
{
  R r(2,-5);
  cout << r <<" "<< r;
  //cout << gcd(30,24);
  //cout << R::gcd(-12, 102);
  cout << endl;
  cout << r*(R(-7, -11));
  R rx(1,1);
  cin >>  rx;
  cout << rx;
  cout << rx+r;
}
