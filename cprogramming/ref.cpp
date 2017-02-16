#include <iostream>
#include <algorithm>
using namespace std;
int main(){
  auto item = 'A';
  char arr[] = { 'A', 'B', 'C'};
  for(auto&item : arr ){
  item ++;
  }
  cout << item << endl;
  for_each(begin(arr), end(arr), [](char& i){ cout << i << endl;});
}
