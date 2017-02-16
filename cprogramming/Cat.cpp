#include "Cat.h"
#include "Dog.h"
#include <iostream>
#include "Singleton.h"
using namespace std;

//extern Dog d;
void Cat::meow (){
  std::cout << "Cat rules! My name is " << _name << endl;

}

Cat::Cat (char * name){
  std::cout << "Constructing Cat " << name << std::endl;
  _name = name;

 // d.bark();
  Singleton::getDog() -> bark();
}
