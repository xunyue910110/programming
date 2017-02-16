#include <map>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

void readIn(multimap<string,string> &books);
void sort(multimap<string,string> &books);
void compact(multimap<string,string> &books);
void myprint(multimap<string,string> &books);

int main()
{
    multimap<string,string> books;
    readIn(books);
    sort(books);
    compact(books);
    myprint(books);
    return 0;
}
pair<string, string> splitbytab(string s)
{
    pair<string, string> book;
    string::size_type pos = s.find('\t');
    if ( pos != string::npos)
    {
        book.first = s.substr(pos+1);
        book.second = s.substr(0, pos);
    } 
    else 
    {
        cout << "data error" << endl;
        exit(1); 
    }
    return book;
}
void readIn(multimap<string,string> &books)
{
    string textline;
    while (getline(cin, textline, '\n') ){
        pair<string, string> book = splitbytab(textline);
        books.insert(book);
    }
}

void sort(multimap<string,string> &books){
    return;
}
void compact(multimap<string,string> &books){
    return;
}
void myprint(multimap<string,string> &books){
    multimap<string,string>::iterator iter = books.begin();
     multimap<string,string>::iterator end = books.end();

    while ( iter != end)
    {
        cout << iter->first << ", " << iter->second << endl; 
        iter++;
    }
}
