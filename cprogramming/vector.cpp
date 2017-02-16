#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
using namespace std;

class CC1
{
public:
    CC1(){cout << "C1 constructor called\n";}
    ~CC1() {cout << "C1 destructor called\n";}
    CC1(const CC1& other) { cout << "copy ctor called\n";} 
    CC1& operator= (const CC1&other) { cout << "operator= called\n";}
};

int main()
{
/*
    vector<shared_ptr<CC1>> vecPtr;
    shared_ptr<CC1> spc1(new CC1());
    vecPtr.push_back(shared_ptr<CC1>(new CC1()));
    vecPtr.push_back(spc1);
    auto i =  remove(vecPtr.begin(), vecPtr.end(), spc1);
    vecPtr.erase(i);
*/
/*
    vector<CC1>  vecCC1;
    vecCC1.push_back(CC1());
    vecCC1.push_back(CC1());
    vecCC1.push_back(CC1());
*/
    vector<shared_ptr<CC1>>  vecCC1;
    vecCC1.push_back(shared_ptr<CC1>(new CC1()));
    vecCC1.push_back(shared_ptr<CC1>(new CC1()));
    vecCC1.push_back(shared_ptr<CC1>(new CC1()));
    //vecCC1.clear();
    return 0;
    
}
