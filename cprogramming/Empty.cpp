#include <iostream>
#include <string>
using namespace std;
class Empty {
public:
    Empty(const Empty& empty){cout << "Empty copy constructor" << endl;}

    //Empty(){}
    Empty(int i = 2){}
    Empty& operator=(const Empty& empty){cout << "Empty = called " << endl;return *this;}
    Empty& operator=(int e){return *this;}
    //Empty(int a){};
};

template < typename T>
class NamedObject {
public:
    NamedObject(const char*name, const T& value);
    NamedObject(std::string& name, const T value){
        nameValue = name;
    //    objectValue = value;
    }
    //:x
    //NamedObject<T>& operator=(const NamedObject<T>& hrs){cout << "NamedObject = called" << endl;return *this;}
    //NamedObject(const NamedObject& hrs){ cout << "NamedObject copy constructor" << endl;}
private:
     std::string nameValue;
    //const T objectValue;
};
int main()
{
 /*   Empty empty1;
    Empty empty2(2);
    cout << sizeof(empty1) << endl;
    empty1 = empty2;
    empty1 = 2;
    Empty *pe = new Empty();
    delete pe; 
*/
    //NamedObject<Empty> no1 ("Smallest Prime Number", 2);
    //NamedObject<Empty> no2(no1);
//    no2 = no1;
    std::string newDog("Persephone");
    std::string oldDog("Satch");
    NamedObject<int> p(newDog, 2);
    //NamedObject<int> s(oldDog, 36);
}
