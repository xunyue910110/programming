#include <stdio.h>
#include <iostream>
#include <new>
#include <stdlib.h>
using namespace std;
class newc{
public:
  ~newc(){cout << "destructor call\n"<<endl;}
};
int main()
{
  //int *p = new int(2);
  //char *pc = new char[2]{99,97};
  //char *pd = new (nothrow) char[~0];
  //*pd = '1';
  //int *pi2 ;
  //*pi2 = 10;
  //delete[] pd;
  //delete[] pc;
  //delete p;
//  printf("%p\n", p);
  //p[0x20000] = 10;
  /*short *ps = new short();
  printf("%d\n", *ps);
  double *pdb = new double();
  printf("%f\n", *pdb);
  */
  //*pd = 123.45;
/*  int *p2 = NULL;
  //delete p2;
  free(p2);
  printf("111");
  //int *p3 ;
  //free(p3);
  cout << &p2;
*/
  int *p = static_cast<int*>(malloc(40));
  fprintf(stderr,"%p", p);
  //printf("%p", p);
  //p[200000] = 0;
  delete p;
  //delete p;

  //newc *pi = new newc[3000];
//  char **pi = new char*[2000];
  
  //delete[] pi;
  //new newc;
 newc();
  //pi = NULL;
}

