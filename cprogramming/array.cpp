#include <iostream>
#include <stdio.h>
#include <array>
#include <algorithm>
using namespace std;


int main()
{
    array<int, 4> arrInt {1,2,3,4};
    array<int, 3> arrInt2 {10,20,30};
    int  arrInt3[] {100, 200, 300, 400};
    int *p = arrInt.data();
    for (int i = 0; i < arrInt.size(); i++)
    cout << p[i] << endl;

    //swap(begin(arrInt3), end(arrInt3), arrInt.begin(), arrInt.end());
    arrInt.swap( arrInt2);

    for_each(arrInt.begin(), arrInt.end(), [](const int& n ){ cout << n << " "; });    
    for_each(arrInt2.begin(), arrInt2.end(), [](const int& n ){ cout << n << " "; });    
    
}
