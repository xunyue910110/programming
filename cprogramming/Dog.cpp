#include "Dog.h"
#include "Cat.h"
#include <iostream>
#include "Singleton.h"
using namespace std;

//Cat c ( "Smokey");

void Dog::bark(){
  std::cout << "Dog rules! My name is " << _name << endl;
}

Dog::Dog(char * name){
  std::cout << "Constructing Dog " << name << std::endl;
  _name = name;

//  Singleton::getCat() ->meow();
  //c.meow();
}
