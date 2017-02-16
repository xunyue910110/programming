#include <iostream>
#include <string>
using namespace std;
class Graph{
int x;
int y;
public:
  Graph(int x, int y):x(x), y(y){}
  void show() {
    cout << type()<< "Pos(" << x << "," << y << "),Area" << area() << endl;
  }
  virtual double area(){};
  virtual string type(){return "";};
};

class Rect: public Graph{
int w;
int h;
public:
  Rect(int x, int y,int w, int h ): Graph(x,y),w(w), h(h){}
  double area(){
    return w*h;
  }
  string type(){return "Rect";}
};

class Circle: public Graph{
 int r;
public:
  Circle(int x, int y, int r): Graph( x,y),r(r){}
  double area() {
    return 3.14*r*r;
  }
  string type() {return "Circle";}
};

class Computer{
public:
  static void usegraph(Graph& c) 
  {
    c.show();
  }
};

int main()
{
  Circle c (8,8,10);
  Rect r (0, 0, 20, 5);
  Computer::usegraph(c);
  Computer::usegraph(r);
}
