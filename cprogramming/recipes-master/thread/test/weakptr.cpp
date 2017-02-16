#include <iostream>
using namespace std;
#include <boost/smart_ptr.hpp>
class C{
  public:
    ~C(){cout << "des" << endl;}
};
int main()
{
  boost::shared_ptr<C> spc(new C);
  cout << spc.use_count() << endl; 
  boost::weak_ptr<C> wpc (spc);
  cout << spc.use_count() << endl; 
  cout << wpc.use_count() << endl; 
  boost::shared_ptr<C> spc2 (wpc);
  cout << spc2.use_count() << endl;
  cout << wpc.use_count() << endl;
}
