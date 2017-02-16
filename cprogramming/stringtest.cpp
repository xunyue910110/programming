#include <iostream>
using namespace std;
#include <string>
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

int main()
{
  int a = 8;
  string s = "xy=";
  char buf[20];
  snprintf(buf, 20, "%d", a);
  s += buf;
  printf("%s\n", s.c_str());

  string s2 = boost::lexical_cast<string>(a);
  cout << s2 << endl;

  string s3("3.14e12");
  double d = boost::lexical_cast<double>(s3);
  cout << d << endl;

  int i = strtol("ff", 0, 16);
  cout << i << endl;
   cout << boost::lexical_cast<string>(0xff) << endl;

   try{
   //int i2 = boost::lexical_cast<int>("ff");
   } catch (boost::bad_lexical_cast& e){
    cout << e.what() << endl;

   }
    cout << boost::format("%1$d %2$s %1$d\n") % a % "hello";
}
