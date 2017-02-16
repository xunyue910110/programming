#include <iostream>
#include <memory>
using namespace std;

class Dog {
  public:
  virtual  ~Dog () { cout << "Dog destroyed" << endl;}

};
class YellowDog : public Dog{
  public:
  ~YellowDog() { cout << "YellowDog destroyed" << endl;}
};

class DogFactory{
  public:
    static shared_ptr<Dog> createYellowDog()
    {
      return shared_ptr<Dog> (new YellowDog());
    }
};
int main()
{
  shared_ptr<Dog >  pd = DogFactory::createYellowDog();
  //delete pd;
}
