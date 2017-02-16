#include <iostream>
#include <thread>
#include <string>
using namespace std;

class Fctor{
 public:
   void operator()(string msg) {
     cout << "t1 says: " <<msg << endl;
    msg = "Trust is the mother of deceit." ;
   }
};

int main() {
  string s = "Where there is no trust , there is no love.";

  Fctor f;
  std::thread t1(f, std::move(s) );

  std::thread t2 =std::move( t1 );
  cout << "child thread id : " << t2.get_id() << endl;
  t2.join();
  cout << "from main: " << s << endl;
  cout << "main thread id : " << std::this_thread::get_id() << endl;
  cout << std::thread::hardware_concurrency();
  return 0;
}
