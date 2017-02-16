#include <iostream>
#include <cstring>
using namespace std;

class S
{
private:
  int len;
  char *p;
public:
  //S():len(0),p(new char[1]){*p = '\0';}
  private:
  S(const S&);
public:
  S(const char* str = "\0")
  {
    len = strlen(str);
    p = new char[strlen(str)+1];
    strcpy(p, str);
  }
  S& operator=(const S&right)
  {
     if ( this == &right)
      return *this;
    len = right.len;
    delete[] p; p = NULL;
    p = new char[right.len+1];
    strcpy(p, right.p);
    return *this;
  }
  char& operator[](int i )
  {
    return p[3];
  } 
public:
  ~S() { delete[] p; p=NULL;}
  friend ostream& operator<<(ostream&o, const S&s)
  {
    o << s.p ;
    return o;
  }
  operator const char*()
  {
    return p;
  }
  operator string()
  {
    return (p);
  }
};

int main()
{
  S a;
  //S b = "芙蓉"; //先调用S()再调用S(const S&);
  S b("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX芙蓉");//只调用 S()
  S c;
  c = a = b;
  cout << a << "," << b << "," << c;
  b = b;
  cout << b[10];
  b[10] = 'y';
  cout << b;

  const char *x = b;
  cout << endl;
  cout << x;
  string s = b;
  cout << endl;
  cout << s;
}
