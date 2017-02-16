#include "bill/Index.h"
#include <iostream>

int main()
{
  int _a[] = { 3, 23, 7, 10, 11, 2 };
  int _b[] = { 1, 51, 8, 12, 13, 2, 3 , 9, 7, 6};
  Index<int> index(_a, _a+6);
  index.append(_b, _b+10);

  for (int i=0; i<index.size(); ++i)
    cout << index[i] << endl;
  return 0;
}
