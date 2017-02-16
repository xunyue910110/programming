#include <vector>
#include <iostream>
using namespace std;
class Person{
  private:
    string * pName_;
  public:
    Person(string name) {  pName_ = new string(name);}
    ~Person(  ) { delete pName_;}
    void printName() { cout << *pName_ << endl;}
    Person(const Person& rhs) {
      pName_ = new string(*(rhs.pName())) ;
    }
    string* pName() const { return pName_;}

    Person& operator=(const Person& rhs) {
      string* pOrig  = pName_;
      pName_ = new string(*(rhs.pName())) ;
      delete pOrig;
    }
    Person* clone() {
      return new Person(*(pName_));
    }
};

int main() {
  vector<Person> persons;
  persons.push_back(Person("George"));
  Person p1 = Person("g");
  Person p2 = Person("d");

  p1 = p2;
  cout << *p1.pName() << endl;
  cout << *p2.pName() << endl;

//  delete p1;
//  delete p2;

  persons.back().printName();

}
