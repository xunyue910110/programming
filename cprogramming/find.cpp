#include <iostream>
#include <string.h>
using namespace std;

template<class T>
T* find (T a[], int n,  T v)
{
    for (int i = 0; i < n ; i++)
        if ( a[i] == v) return &a[i];
    return NULL;
}
template<class T, class Pred>
T* find_if (T a[], int n, Pred pred )
{
  for (int i = 0; i < n; i++)
      if (pred(a[i]))
        return &a[i];
  return NULL;
}
template <>
const char** find(const char* a[], int n, const char*v)
{
  for(int i = 0; i < n; i++)
    if (strcmp(a[i] , v) == 0)
      return &a[i];
  return NULL;
}

bool isb(char& c)
{
  return c == 'b';
}
class classisb{
  public:
    bool operator()(char& c)
     {
      return c == 'b';
        }
};

int main()
{
  char ac[5] = {'a', 'b', 'c', 'd', 'e'};
  const char* acc[5] = { "good", "morning", "yes"};

  char * pc = find (ac,5, 'c');
    if (pc )
      cout << *pc << endl;
  const char s[] = "yes"; 
  const char**ppc = find(acc, 3, s);
    if (ppc != NULL)
        cout << *ppc << endl;
  char *pc2 = find_if(ac, 5, classisb());
    if (pc2) cout << *pc2 << endl;
}
