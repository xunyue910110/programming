#include <iostream>
using namespace std;

class P{
private:
  string name;
public:
  void setname(const char *p){name = p;}
  void eat(string food){
    cout << name << "吃" << food <<endl;
  }
  void speak(string words){

    cout << name << "说:\"" << words <<"\"\n";
  }
  const string& getname(){return name;}
};

class T : public P
{
string course;
public:
  void teach(string someclass){
    cout << getname() << "给" << someclass << "班讲" <<
    course << " 课程" << endl;
  }
  void setcourse(string c) { course = c; }

};

int main()
{
  P a;
  a.setname("狗");
  a.eat("水煮鱼");
  a.speak("大家好");

  T b;
  b.setname("权哥");
  b.setcourse("CPP");
  b.teach("CSD1212");
  
}
