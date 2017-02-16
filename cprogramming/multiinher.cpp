#include <iostream>
using namespace std;
class Phone{
public:
  Phone(){cout << "Phone()"<<endl;}
  void call(){cout << "打电话" <<endl;}

};

class Camera{
public:
 Camera(){cout << "Camera()" <<endl;}
  void takephoto(const string& target){
    cout << "照相" << target<<endl;
  }
};

class CellPhone : public Phone, public Camera{
public:
  CellPhone():Camera(),Phone(){ cout << "CellPhone()"<<endl;}
  
};
int main()
{
  CellPhone cp;  
  cp.call();
  cp.takephoto("风景");
}
