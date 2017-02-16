#include <iostream>
#include <array>

int main ()
{
  const std::array<int,5> myarray = { 2, 16, 77, 34 };

  std::cout << "myarray contains:";
  for ( auto it = myarray.begin(); it != myarray.end(); ++it )
    std::cout << ' ' << *it;
  std::cout << '\n';
  for ( auto it = cbegin(myarray); it != cend(myarray); ++it )
    std::cout << ' ' << *it;
  std::cout << '\n';

  return 0;
}
