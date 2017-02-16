#include <iostream>
class R{
private:
  int n ;
   int d;
public:
  explicit R(int n, int d = 1):n(n),d(d){}
  ~R(){std::cout<< "n=" << n << ",d="<<d<<std::endl;}
  R add(const R& other){ 
  return R(n*(other.d) + d*(other.n), 
    d*(other.d));}
};
  

int main()
{
  R r1 (2,5);
  R r2 = r1.add(R(3));

}
