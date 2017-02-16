#include <iostream>
using namespace std;
#include <stdio.h>

typedef enum Color{
  RED=1000,GREEN,BLUE
}Color;
union Union1{
int x1;
char y1[4];
} ;

void show(const char*,  char gender = 'M'){}
void show2(const char*){ show( "赵俊");}
int main()
{
  Color c1;
  c1 = static_cast<Color>(1);
  //int *p = reinterpret_cast<int*>(&c2);
  //Color*q = reinterpret_cast<Color*>( p);
  //Color*q = reinterpret_cast<Color*>( p);
  const int i = 0x10203040;
  //const int *pi = &i;
  *const_cast<int *>(&i) = 0x40302010; 
  //*pi2 = 0x40302010; 
  printf("%x\n", i);

  const int c = 100;
  cout << "c=" << c << endl;
  *const_cast<int*>(&c) = 200;
  cout << "c=" << c << endl;
  short s = 2;
  char c2 = 3;
  char x = 97;
  Color color1;
  color1 = Color::GREEN;
  printf("%d\n", color1);
  Union1 ua;
  ua.x1 = 0xff64;
  printf("%hhu\n", (ua.y1[1]));

  //show ("赵俊");
  show2("sss");
}


  

