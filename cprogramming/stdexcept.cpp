#include <iostream>
#include <exception>

using namespace std;
int main()
{
  try{
  double *p = new double[~0];
  cout << p << endl;
  } 
  catch(exception &e ){
    cout << e.what() << endl;
  }
  cout << "oh yeah" << endl;
}
