#include <iostream>
#include <deque>

int main ()
{
  std::deque<int> mydeque (100);
  std::cout << "1. size of mydeque: " << mydeque.size() << '\n';

  mydeque.resize(10);
  std::cout << "2. size of mydeque: " << mydeque.size() << '\n';

    mydeque.push_back(1);
    mydeque.push_back(2);
    mydeque.resize(10);
    mydeque.push_back(2);

  //mydeque.shrink_to_fit();
  std::cout << "2. size of mydeque: " << mydeque.size() << '\n';

  return 0;
}
