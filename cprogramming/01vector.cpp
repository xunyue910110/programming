#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

int main()
{
  std::vector<char> vc;
  std::cout << vc.size() << std::endl;
  vc.push_back('a');
  vc.push_back('b');
  std::cout << vc.size() << std::endl;
  std::cout << std::string(vc.begin(), vc.end()) << std::endl;
}
