#include <iostream>
using namespace std;

struct Point {
int x;
int y;

};

class Pos{

Point p;
public:
  Pos(int x=0, int y=0){p.x = x;p.y =y;}
  void move(int x, int y){p.x+=x; p.y+=y;}
  void moveto(int x, int y){p.x = x; p.y=y;}
  Point* operator->( ){ return &p;} 
};

int main()
{
  Pos a(20,80);
  cout <<a->x <<"," << a->y<<endl;
  cout << (a.operator->())->x << endl;
}
