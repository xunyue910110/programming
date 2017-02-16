#include <iostream>
#include <thread>

using namespace std ;

class X {
  public:
    void do_lengthy_work(){
            cout << "do_lengthy_work called.\n";
        }
};

int main() {
    X my_x;
       std::thread t(&X::do_lengthy_work(), &my_x);
          t.join();
}
