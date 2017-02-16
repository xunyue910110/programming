#include <iostream>
using namespace std;
#include <time.h>
#include <stdlib.h>

class array{
private:
  int *a;
  int len;
//mutable  int cur;
  static int cur;
public:
  array():a(0), len(0){}
  array(int n):len(n) , a(new int[n]){
    for(int i =0; i <len; i++)
      a[i] = 0;
  }
  array(int n, int v):len(n),a(new int[n]){
    for(int i =0; i <len; i++)
      a[i] = v;
}

  ~array(){delete [] a; a = 0;}
  
  int size(){return len;}
  void randfill()
  {
    srand(time(NULL));
    for(int i = 0; i < len; ++i)
    {
      a[i] = random()%100;
    }
  }

  void use(const array& x) const
  {
    for (int i=0; i <3; i++)
      cout << x.next() << ' ' ;cout<< endl;
    for (int i=0; i <4; i++)
      cout << x.next() << ' '; cout << endl;
    for (int i=0; i <5; i++)
      cout << x.next() << ' ' ;cout << endl;
  } 

  int next()const {
    return a[cur++%len];
  }
  void print(){
    for(int i =0; i < len; i++)
      cout << a[i] << " ";
    cout << endl;
  }
  void set(int i, int v){
    if ( i < 0 || i >= len) throw "out of range";
    a[i] = v;
  }
  int get(int i ){ 
    if ( i < 0 || i >= len) throw "out of range";
    return a[i];
  }
};

int array::cur = 0;
int main()
{
  array x(10);
  x.randfill();
  
  x.print();
  x.use(x);
  for (int i = 0; i <=x.size() ; i++)

    cout << x.get(i) << " ";
  cout << endl;
}
