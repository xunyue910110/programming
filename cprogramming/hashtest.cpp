#include <iostream>
#include <algorithm>
#include <string>
using namespace std;
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
using namespace boost;
template<typename T>
void print(T first, T last){
  for (; first != last; ++first)
    cout << first->first << " "
      << first->second << endl;
}
int main()
{
  unordered_set<int> us;
  us.insert(1);
  us.insert(7);
  us.insert(14);
  us.insert(9);
  us.insert(33);
  copy(us.begin(), us.end(), ostream_iterator<int>(cout, " "));
  unordered_map<string, int> um;
  um.insert(make_pair("xuanyuan1", 12));
  um.insert(make_pair("xuanyuan1", 12));
  um.insert(make_pair("xuanyuan3", 12));
  um.insert(make_pair("xuanyuan4", 12));
  um["x1"] = 120;
  um["x2"] = 999;
  um["x1"] = 333;

  print(um.begin(), um.end());
  cout << um.size() << endl;
  cout << um["x3"] << endl;
  cout << um.size() << endl;
  print(um.begin(), um.end());
  //cout << um.capacity() << endl;
  typedef unordered_map<string, int>:: local_iterator Iter;
  int n = um.bucket_count();
  for ( int i = 0; i < n ; i++)
  {
    cout << "Bucket# " << i << " : " <<
      um.bucket_size(i) << endl;
    for ( Iter it = um.begin(i); it != um.end(i) ; ++ it)
      cout << it->first << ", " << it->second << endl;      
  }
}
