#include <iostream>
using namespace std;
enum EmployeeType{
  Engineer,
  Sales,
  Manager
};

int main()
{
  cout << EmployeeType::Engineer << endl; 
  cout << EmployeeType::Sales<< endl; 
  EmployeeType et = EmployeeType::Manager;
  cout << et << endl;
}
