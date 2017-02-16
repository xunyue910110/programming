#include <iostream>
#include <algorithm>
using namespace std;

class Date{
public:
  int month;
  int day;
  int year;
};
void sort(Date a[], int n, int Date::*p)
{
  for (int i = 0; i<n; i++)
    for (int j = i+1; j<n;j++)
      if ( a[i].*p > a[j].*p)
        std::swap(a[i], a[j]);
}

int main()
{
  int Date::*p = &Date::year;
  Date a = { 1, 15, 2013};
  Date b = { 8, 18, 2008};
  cout << a.*p << ',' << b.*p << endl;
  p = &Date::day;
  cout << a.*p << ',' << b.*p << endl;

  int *p2 = &a.year;
  cout << *p2 << endl;
  Date x[4] = {a, b, {7, 7, 2015}, {9,9, 2009}};
  sort (x, 4, &Date::month);
  for (int i = 0; i < 4; i++)
    cout << x[i].year << '-' << x[i].month <<'-'<< x[i].day<<endl;
}
