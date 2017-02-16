#include <iostream>
#include <vector>
#include <string>

using namespace std;

template <class T> 
class He {
public:
   add(T&);
private:
   T  a;
};


template <class T>
He<T>::add(T& x)
{
	a.hello();
	a = x;
}

