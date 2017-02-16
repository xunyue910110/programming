#include <iostream>
#include <string>

int main()
{
    using namespace std;
    string in_string;
    
    cout << "Please enter your name: ";

    cin >> in_string;

    if ( in_string.empty())
        cerr << "error: input string is empty.\n";
    else
        cout << "hello, " << in_string << "!\n";
    
}
