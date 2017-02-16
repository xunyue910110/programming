#include <set>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

struct stbook{
    string publisher;
    string title;
    bool operator<(stbook t2) const
    {
        if ( publisher.compare(t2.publisher) < 0 )
            return true;
        else if ( publisher.compare(t2.publisher) > 0)
            return false;
        else 
            return title.compare(t2.title) < 0;
    }
};

void readIn(multiset<stbook> &books);
void sort(multiset<stbook> &books);
void compact(multiset<stbook> &books);
void myprint(multiset<stbook> &books);

int main()
{
    multiset<stbook> books;
    readIn(books);
    sort(books);
    compact(books);
    myprint(books);
    return 0;
}
stbook splitbytab(string s)
{
    stbook book;
    string::size_type pos = s.find('\t');
    if ( pos != string::npos)
    {
        book.publisher= s.substr(pos+1);
        book.title = s.substr(0, pos);
    } 
    else 
    {
        cout << "data error" << endl;
        exit(1); 
    }
    return book;
}
void readIn(multiset<stbook> &books)
{
    string textline;
    while (getline(cin, textline, '\n') ){
        stbook book = splitbytab(textline);
        books.insert(book);
    }
}

void sort(multiset<stbook> &books){
    return;
}
void compact(multiset<stbook> &books){
    return;
}
void myprint(multiset<stbook> &books){
    multiset<stbook>::iterator iter = books.begin();
     multiset<stbook>::iterator end = books.end();

    string pub = "";
    string title = "";
    int startfg = 1;
    int count = 0;
    while ( iter != end)
    {
        if ( startfg == 1)
        {
            pub = iter->publisher;
            startfg = 0;
        }
        if ( iter->publisher != pub  )
        {
            cout << "小计: publisher  " << pub << " " << count << " books saled." << endl;
            pub = iter->publisher;
            count = 1;
        }
        else 
        {
            count++;    
        }
        
        cout << iter->publisher << ", " << iter->title << endl; 
        iter++;
    }
    
    cout << "小计: publisher  " << pub << " " << count << " books saled." << endl;
}
