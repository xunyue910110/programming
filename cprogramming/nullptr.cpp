#include <iostream>
using namespace std;

void f(long) { cout << "f(long)\n";}
void f(char *) { cout << "f(char*)\n";}

int main()
{
    //f((long)0);
    //f(0L);
    //f(static_cast<char*>(0));
    f(nullptr);
}
