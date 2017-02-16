#include <numeric>
namespace A {
  class C{};
};

  int operator+( int n, A::C c ) {return n + 1;}
int main() {
   A::C arr[3] ;
   std::accumulate(arr, arr + 3, 0);

}
