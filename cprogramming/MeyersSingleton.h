#include <iostream>
#include <stdlib.h>
#include <unistd.h>
using namespace std;
class MeyersSingleton
{
public:
    static MeyersSingleton& GetInstance()
    {
        cout << "Enter GetInstance() " << endl;
        static  MeyersSingleton ms;
        cout << "GetOUT from GetInstance() "<< endl;
        return ms;
    }
    void Print()
    {
        cout << "MeyersSingleton::Print called , m_count= " << ++m_count << endl;
    }
private:
    MeyersSingleton(){
       cout << "MeyersSingleton constructor called\n"; 
        sleep(15);
        m_count = 100;
       cout << "MeyersSingleton constructor called completed\n"; 
        
    }
    ~MeyersSingleton(){ 
        cout << "MeyersSingleton destructor called\n"; 
     }
    MeyersSingleton(const MeyersSingleton&){}
    MeyersSingleton& operator=(const MeyersSingleton&){}
    int m_count;
};

