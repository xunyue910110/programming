#include <iostream>
#include <string>
using namespace std;

int main() {
  string s("Goodbye");
  cout << s << "," <<  s.size() <<", "<< s.length() << s.capacity() << endl;
  s.reserve(100);
  cout << s << "," << s.size() <<", "<< s.length() << s.capacity()<< endl;
  s.reserve(5);
  cout << s << "," << s.size() <<", "<< s.length() << s.capacity() << endl;
  s.shrink_to_fit();
  cout << s << "," << s.size() <<", "<< s.length() << s.capacity() << endl;

  s.resize(9);
  cout << s << "," << s.size() <<", "<< s.length() << s.capacity() << endl;
  s.resize(3);
  cout << s << "," << s.size() <<", "<< s.length() << s.capacity() << endl;
}
