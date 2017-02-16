#include <iostream>
#include "Dog.h"
#include "Cat.h"
#include "Singleton.h"
using namespace std;

//Dog d("Gunner");

int main(){

  Singleton::getCat()->meow();
return 0;}
