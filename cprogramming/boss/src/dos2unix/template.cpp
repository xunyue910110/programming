#include <iostream>
#include <vector>
#include <string>

using namespace std;

template <class T> 
class He {
public:
   T add(const T&);
private:
   T  a;
};


template <class T>
T He<T>::add(const T& x)
{
	a.hello();
	a = x;
}
