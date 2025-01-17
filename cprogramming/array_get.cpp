#include <iostream>
#include <array>
#include <tuple>

int main ()
{
  std::array<int,3> myarray = {10, 20, 30};
  std::tuple<int,int,int> mytuple (10, 20, 30);

/*  std::tuple_element<0,decltype(myarray)>::type myelement;  // int myelement

  myelement = std::get<2>(myarray);
  std::get<2>(myarray) = std::get<0>(myarray);
  std::get<0>(myarray) = myelement;
*/
    std::tuple_element<0, decltype(myarray)>::type myelement;

    int i = std::get<0>(myarray);
    std::get<1>(myarray) = i;
  std::cout << "first element in myarray: " << std::get<0>(myarray) << "\n";
  std::cout << "second element in myarray: " << std::get<1>(myarray) << "\n";
  std::cout << "first element in mytuple: " << std::get<0>(mytuple) << "\n";

  return 0;
}
