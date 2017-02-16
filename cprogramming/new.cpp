#include <iostream>
#include <cstdlib>
using namespace std;

class A
{
  int data;
public:
  A(int d=0):data(d){cout << "A(" << data<<")"<<endl;}
  ~A(){cout << "~A()"<< data<<endl;}
  void *operator new(size_t bytes){
    cout << "bytes=" << bytes << endl;
    return malloc(bytes); 
  }
  void *operator new[](size_t bytes){
    cout << "Bytes=" << bytes << endl;
    void *p = malloc(bytes);
    cout << "malloc = " << p << endl;
    return p;
  }
  void operator delete(void *p)
  {
    cout << "free " << p << endl;
    free(p);p=NULL; 
  }
  void operator delete[](void *p)
  {
    cout << "Free " << p << endl;
    free(p);p=NULL; 
  }
  
};
int main()
{
  A* p = new  A[3];
  cout << "sizeof A= " << sizeof(A) << endl;
  cout << "p = " << p << endl;
  cout << "data: "<< *((int*)p-1) << endl;
  delete[] p; p = NULL;
}
