#include <memory>
#include <iostream>
#include <algorithm>
using namespace std;
template<class T>
class Deleter
{
public:
    void operator()(T* p){
    cout << "cb" << endl;
    }
};
unique_ptr<int[]> getptr()
{
    //unique_ptr<int[]> up(new int [2]{18888,19999}, default_delete<int[]>());
    unique_ptr<int[]> up(new int [2]{18888,19999});
    return move(up);
}

int main()
{
    unique_ptr<int> up (new int ( 1729));
    cout  <<*up << endl;
    unique_ptr<int[]> up2 = getptr();
    if (up ) 
    cout << "uh oh" << endl;
    else
    cout << "yay" << endl;
    
    cout << up2[1] << endl;
//    cout << up2.use_count()<< endl;
    cout << "done" << endl;
}
