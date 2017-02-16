#include <stdio.h>
#include <iostream>
using namespace std;

class Dog{
  private:
    int age;
    string name;
  public:
    Dog() { age = 3 ; name = "dummy";}
    void setAge(const int &a) { age = a; cout << "const" << endl;}
    void setAge(int &a) { age = a; cout << "non const" << endl;}

    void printName() const {cout << name << endl; cout << "const" << endl;}
    void printName() {cout << name << endl; cout << "non const" << endl;}
};

class OpenFile{
  public:
    OpenFile(string filename){ cout << "File " << filename << "opened" <<endl;}
    void destroyMe() { delete this;}
  private:
    ~OpenFile( ) {  cout << "File destructed" << endl;}
};

int main(){
   const int i = 9;
   const_cast<int&>(i) = 6;
   printf("%d\n", i);
//   i = 6;
   const int *pi = &i; // data is constant, pointer is not
//   *pi = 5;
//   pi++;
   int j;
   //static_cast<const int &>(j) = 7;

   Dog d;
   int x = 4;
   d.setAge(x + 1);

   d.printName();
   const Dog d2;
   d2.printName();

   OpenFile* f = new OpenFile("bob");
   f->destroyMe();
}

