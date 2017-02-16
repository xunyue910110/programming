template<typename T>
T sum(T n){return n;}

template<typename T, typename... Args>
T sum(T n , Args... rest)
{
    return n + sum (rest...);
}

#include <iostream>
int main(){
    std::cout << sum(1,2,3); 
}
