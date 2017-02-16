#include <iostream>
using namespace std;

class Dog {
  public:
    Dog() { cout << "Dog born." << endl; bark();}
    virtual void bark() { cout << "I am just a dog." << endl;}
    void seeCat() { bark();}
    virtual ~Dog () {bark();}
};

class YellowDog: public Dog {
  public:
    YellowDog() { cout << "Yellow dog born." << endl;bark();}
    virtual void bark() { cout << "I am a yellow dog." << endl;}
    ~YellowDog(){}
};

int main() {
  YellowDog d;
  d.seeCat();
}
