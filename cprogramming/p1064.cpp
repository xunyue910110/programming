#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
    string ifile;
    cout << "Please enter file to sort:  ";
    cin >> ifile;

    ifstream infile(ifile.c_str());
    if ( !infile)
    {
        cerr << "error: unable to open inputfile:"
            << ifile << endl;
        return -1;
    }
    string ofile = ifile + ".sort";

    ofstream outfile(ofile.c_str());
    if ( !outfile){
        cerr << "error: unable to open output file: "
            << ofile << endl;
        return -1;
    }
    string buffer;
    vector <string> text;
    int cnt = 1;

    while ( infile >> buffer ){
        text.push_back(buffer);
        cout << buffer << (cnt++ %8 ? " ": "\n");
    }

    sort( text.begin(), text.end());
    vector<string> :: iterator iter = text.begin();
    for ( cnt = 1; iter!= text.end(); ++iter, ++cnt)
        outfile << *iter << (cnt % 8 ? " ": "\n");
    return 0;
}
