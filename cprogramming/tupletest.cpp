using namespace std;
#include <boost/tr1/tuple.hpp>
#include <iostream>
using namespace std::tr1;
using namespace boost;

void f(int (&a)[3])
{
  cout << a[1];
}

int main()
{
  int a[3] = {1,2,3};
  f(a);
  std::tr1::tuple<int,double> t1 ;
  t1 = std::tr1::make_tuple(1, 1.1);
  
  cout << get<0>(t1);
  //cout << t1.get(0);
  //cout << boost::get<0>(t1) << endl;
  //cout << boost::get<1>(t1) << endl;
  int i;
  char c;
  double d;
  tie(i,c,d) = tuple<int, char, double>(1, 'A', 0.618);
  cout << d  << endl;
  char c2;
  tie(ignore, c) = make_pair(1, 'a');
  cout << c << endl;
  cout << t1;
  }
