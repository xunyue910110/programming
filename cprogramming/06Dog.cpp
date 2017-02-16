#include <string>
#include <stdio.h>
using namespace std;
class dog {
  public:
    explicit dog(string name) { m_name = name;}
    string getName() { return m_name;}
    operator string() const { return m_name;}
  private:
    string m_name;
};

int main() {
 string dogname = "Bob";
 dog dog1 (dogname) ;
 string dogname1 = dog1;
 printf("My name is %s.", dogname1.c_str());
}
