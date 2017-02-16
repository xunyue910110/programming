#include <sstream>
#include <iostream>
#include <string>
using namespace std;

string program_name("out_program");
string version("0.01");

string mumble( int *array, int size)
{
    if ( ! array )
    {
        ostringstream out_message;
        out_message << "error: "
        << program_name << "--" << version 
        << ": " << __FILE__ << ": " << __LINE__
        << " -- ptr is set to 0; "
        << " must address some array.\n";
        return out_message.str();
    }
}
