#include <cstring>
#include <iostream>
#include <stdio.h>

namespace czq {
  struct Student {
    char name[20];
    int age;
    void show(){ std::cout<< age << "岁的" << 
        name << "轻松搞定C++" << std::endl;
    }
  };
}

int main()
{
  czq::Student furong = { "芙蓉", 18};
  using namespace czq;
  Student s2 {"陈宗权", 20};
  furong.show();
  s2.show();
  printf("\n");

  union {
    int x;
    char y[4];
  } a ;
  using namespace std;
  a.x = 0x61626364;
  cout << a.y[0] << endl;
  //y[1] = '0';
  a.y[1] = 0x30;
  cout << showbase<<hex << a.x << endl;
  return 0;
}
