#include <algorithm>
#include <vector>
using namespace std;

#include <iostream>

template <class T>
void print_elements(T elem) { cout << elem << " "; }

void (*pfi) (int )  = print_elements;
void (*pfs) ( string) = print_elements;
int main()
{
    int arrInt[] = { 3 , 2, 2, 1, 2, 3, 4, 1 };
    /*sort( arrInt, arrInt + 8);
    for_each( arrInt, arrInt+8, pfi);
    cout << endl;
    vector<int> arr2 (8);
    vector<int> :: iterator it;
    //pi = unique(arrInt, arrInt+8);
    it = unique_copy(arrInt, arrInt + 8, arr2.begin());

    for_each( arr2.begin(), it, pfi);
    */
    /*int *pj = arrInt;
    for ( ; pj<pi; pj++)
        cout << *pj << " ";
    cout << endl;

    for ( ; pi<arrInt+8; pi++)
        cout << *pi << " ";
    cout << endl; 
    */
    int a = 2;
    cout << binary_search( arrInt, &arrInt[7], 4);
    cout << *search_n(&arrInt[0], &arrInt[7], 2, a);
}
