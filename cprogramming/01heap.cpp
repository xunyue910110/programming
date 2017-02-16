#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

int main() {
  vector<int> vec = {9, 1, 10, 2 , 45};

  make_heap(vec.begin(), vec.end());
  for ( auto i : vec )
    cout << i << endl;
  sort_heap(vec.begin(), vec.end());

  for ( auto i : vec )
    cout << i << endl;
}


