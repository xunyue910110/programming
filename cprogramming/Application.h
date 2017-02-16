#include <iostream>
using namespace std;

class Application {
    public:
    Application(){ cout << "Application ctor" << endl;}
    ~Application(){ cout << "Application dector" << endl;}
    void setopt(int argc, char*argv[]){ cout << "Application setopt" << endl;}
    virtual int run(){cout << "Application run" << endl;return 1;}
};
