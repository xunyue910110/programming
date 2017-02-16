#include <iostream>
#include <string.h>
#include <string>
using namespace std;

template <typename T>
bool lessthan(const T& a,const T& b) 
{
  return a < b;
}


/*
template <const char*>
bool lessthan(const char* a, const char*b )
{
  return strcmp(a, b) < 0;
}
*/
template <>
bool lessthan(const char* const& a, const char*const &b )
{
  return strcmp(a, b) < 0;
}

template<class T>
void sort(T a[],int n)
{
  for (int i = 0; i < n ; i++)
    for(int j =0; j <i; j++)
        if (lessthan(a[i], a[j]) )
          swap(a[i], a[j]);
}


template<class T>
void print(T a[], int n)
{
  for (int i = 0; i < n ; i++)
    cout << a[i] << " ";
  cout <<endl;
}
int size(int (&ra)[5])
{
  return sizeof(ra)/sizeof(ra[0]);
}
int main()
{
  int ai[6] = { 8, 1,6, 9, 3,5};
  char ac[5] = { 'd', 'x', 's', 'a', 'm'};
  double ad[5] = { 5.5,3.3,9.9,6.6,2.2};
  string as[4] = { "good", "morning", "dear", "friends"};
  const char * acc[5] = {"good", "morning", "dear", "friend", "xxxxx"};
  sort(ai,6);
  sort(ac,5);
  sort(ad,5);
//  sort(as,4);
  sort(acc,5);

  print(ai,6);
  print(ac,5);
  print(ad,5);
 // print(as,4);
  print(acc,5);

  int arri[5] ;
  //int (&ra)[5]  = arri;
  cout <<size(arri);
}

