//单例模式
#include <iostream>
using namespace std;

class Chairman{
  Chairman(const string& name):name(name) {
  }
  private:
    Chairman(const Chairman&);
  public:
  const string name;
  public:
  static Chairman* getInstance()
  {
    static Chairman cm(string("习近平")); //静态局部变量
    return &cm;
  }
  void show(){cout<<"同志们好， 我是主席"<< name << endl;}
};

int main()
{
  Chairman* c1 = Chairman::getInstance();
  c1->show();
 // c1->name = "芙蓉";
  //c1->show();
  return 0;
}
