#include <iostream>
#include <functional>
#include <map>
#include <stdio.h>
#include <set>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <stdexcept>
using namespace std;
/*
class Even
{
    public:
    bool operator()(int a ){ return a % 2 == 0 ;}
};
*/
bool Even(int a) { return a % 2 == 0 ; }
class intgreator
{
public:
    bool operator()( int a, int b) { return  a > b; }
};

class beforeTwice
{
public:
    bool operator () (const int &a, const int&b)
    {
        return a == 2*b;
    }
};
struct student
{
public:
    student(int stuid, const string &stuname){this->stuid = stuid; this->stuname=stuname;}
    int stuid;
    string stuname;
    bool operator<(const student& stu2)const { return this->stuid < stu2.stuid;}
};

struct studentcmp{
    bool operator() (const student& stu1, const student& stu2){return stu1.stuid < stu2.stuid; }
};
int main()
{
/*
    string s("1234567890");
    string substring1 =  s.substr( 9, 1); 
    char c = '\0';
    try {
     c = s.at(9);
    } catch ( out_of_range& e)
    {
        cout << "out of range: " << e.what() << endl;
        }
    cout << substring1 << endl;
    cout << c << endl;
    cout << s << endl;
    operator<< ( cout , s) << s  ;
    //cout.operator << (s);
    //operator<< (cout, s);
    char buff [100] = {'c'};
    int cnt = s.copy(buff, 7, 0);
    cout << buff;

    s = "hello ";
    string s2 = "world ";
    s += "world!";
    cout << s;
    s .append("ggggggg", 2);
    cout << s;
    s.append("xyz", 2, 1);
    cout << s;
using namespace std;
    cout << endl;
    sort( s.begin(), s.end());
    cout << s;

    string::iterator ps1 = s.begin();
    while ( ps1 != s. end())
    {
        cout << *ps1;
        ps1++;
    }
    string::reverse_iterator ps2 = s.rbegin();
    while ( ps2 != s. rend())
    {
        cout << *ps2;
        ps2++;
    }
    reverse(s.begin(), s.end());
    cout << s;
    cout << endl;
    int iArray[] = { 1, 3, 2, 3, 3, 3, 4, 3, 5};
    list< int> listInt (iArray, iArray +sizeof(iArray)/sizeof(iArray[0]));
    typedef list<int>::iterator vecintiter;
*/
/*
    vecintiter iter =remove(vecInt.begin(), vecInt.end(), 3);
    vecInt.erase(iter, vecInt.end());
*/
    //listInt.remove(3);
    //listInt.reverse(); 
 /*
    vecintiter iter = listInt.begin();
    iter = remove(listInt.begin(), listInt.end(), 3);
    listInt.erase(iter, listInt.end());
    iter = listInt.begin();
    while(iter != listInt.end())
    {
        cout << *iter;
        iter++;
    }
    cout << *s.begin() << endl; 
    cout << *(s.end()-1) << endl; 

    set<int, intgreator> setInt (listInt.begin(), listInt.end());
    set<int>::iterator it = setInt.begin();
    while (it != setInt.end()){ cout << *it << endl; it++;}
    set<student > setStu;

    setStu.insert(    student(100, ("zhaojun")));
    setStu.insert(    student(101, "luoyan"));
    setStu.insert(    student(101, "luoyan"));
    setStu.erase(student(101, "luoyan"));
    set<student>::iterator setit = setStu.begin();
    while( setit != setStu.end())
    {
        cout << setit->stuid << setit->stuname << " ";
        setit++; 
    }
    cout << (setStu.find(student(100, ("zhaojun"))))->stuid << endl;
    pair<set<student>::iterator , set<student>::iterator> pair1 = 
        setStu.equal_range(student(100, ("zhaojun")));

*/
/*
    int x;
    scanf("%d", &x);
    multiset<int> msetInt;
    while( x != 0)
    {
        msetInt.insert(x);
        scanf("%d", &x);
    }

    multiset<int> ::iterator; 
    while ( !msetInt.empty())
    {it = msetInt.begin();
            printf("%d " , *it); 
             msetInt.erase(it);
    }
*/
/*
    map<int, string> mapstus;
    mapstus.insert(pair<int,string>(1, "zhaojun"));
    mapstus.insert(make_pair(1, "zhaojun"));
    mapstus.insert(map<int,string>::value_type(2, "luoyan"));
    pair<map<int, string>::iterator, bool> mypair = mapstus.insert(map<int,string>::value_type(2, "zzz"));
    if ( mypair.second)
    {
        cout << "插入成功";
        
    }
    else 
    {
        cout << "插入失败";
    }
    mapstus[3]="xxx";
    mapstus[3]="yyy";
    map<int, string>::iterator itlower = mapstus.lower_bound(2);
    map<int, string>::iterator itupper = mapstus.upper_bound(2);
    cout << itlower->first << endl;
    cout << itupper->first << endl;
    map<int, string>::iterator itmap= mapstus.begin();
    while ( itmap != mapstus.end())
    {
        cout << itmap->first;
        cout << itmap->second;
        itmap++;
    } 
    int ia[] = { 1, 8, 4, 4};
    vector <int> vec(ia, ia+4);
    vector<int>::iterator vecit1 = adjacent_find(vec.begin() , vec.end(), beforeTwice());
    //int *vecit1 = adjacent_find(vec.begin() , vec.end());
    if ( vecit1 != vec.end())
        cout << *vecit1 << endl;
*/

/*
    int ia[] = { 29, 23, 20, 22, 17, 15,26, 51, 19, 12, 35, 40};
    sort(ia, ia+12, greater<int>());
    bool found = binary_search ( ia, ia + 12, 19, greater<int>());
    cout << found;
*/
    int ia[] = { 0, 1,1, 2, 3, 5, 8, 13, 21, 34};
    list<int> ilist(ia, ia+10);
    int ia_count = 0;
    cout << count_if(&ia[0], &ia[10], Even);
    cout << count_if(&ia[0], &ia[10], bind2nd(greater<int>(), 10));
    cout << endl;
    cout << "bool" << sizeof(bool);
    cout << "wchar_t" << sizeof(wchar_t) ;
    cout << " char" << sizeof(char);
    cout << "short" << sizeof(short);
    cout << "int " << sizeof(int);
    cout<< "float" << sizeof(float);
    cout << "double" << sizeof(double);
    cout << "long" << sizeof(long);
    cout << "long long " << sizeof(long long);
    cout << "long double" << sizeof(long double);

}

