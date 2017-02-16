//#include "Cat.h"
//#include "Dog.h"

class Dog;
class Cat;

class Singleton{
static Dog* pd;
static Cat* pc;

  public:
~Singleton();
static Dog* getDog();
static Cat* getCat();
};
