#include <iostream>
using namespace std;

class Vehicle{
public:
  virtual void stop() = 0;
};

class Car : public Vehicle{
 public:
  void stop() {cout <<"洗车踩离合踩刹车停住"<<endl;}
};
class Bike : public Vehicle{
  public:
  void stop() {cout <<"自行车捏停住"<<endl;}
};
class Light{
public:
  void stopvehicle(Vehicle& v){ v.stop();}
};
int main()
{
  Car car;
  Bike bike;
  Light l;
  l.stopvehicle(car);
  l.stopvehicle(bike);
}


