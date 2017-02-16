#ifndef _FLOWLINE_UTIL_H_
#define _FLOWLINE_UTIL_H_

#include <string>
#include <time.h>
#include <vector>
#include <iostream>
#include <sstream> 
#include <exception>
#include <map>
using namespace std;

namespace flowline{

	//解析configuration
	void parseParameter(const string& configure, map<string,string>& param);
	
	string getParameter(const string& key , const map<string ,string >& param);
	
	void printParameter(const map<string ,string >& param);
	
	ostream & operator << (ostream & os, const map<string ,string > & param);
    // 把${macro}宏和非宏依次存到vector中: pair<string, bool> bool=true 为宏 
    void parserMacro(const string& instr , vector< pair<string,bool>  > & va);
	//把"sub(123,234)"函数的名字存在args[0],参数依次args[1]~args[n]
    void parserMethod(const string& key, vector <string>& args);
    
    class MacroAttribute{   
        public:
        virtual string getAttribute(const string& name) const = 0;
        virtual string getAttributeFilter(const string& name) const = 0;
    };

    //联合宏的值,从MacroAttribute取值
    string resultMacro(vector< pair<string,bool> > & ora,MacroAttribute & macro);
    string resultMacro(const string & instr,MacroAttribute & macro);
    
    string resultMacrofilter(const string & instr,MacroAttribute & macro);
    //联合宏的值,从MacroAttribute取值
    string resultMacrofilter(vector< pair<string,bool> > & ora,MacroAttribute & macro);
    
    //字符串操作   
	inline  string& ltrim(string &ss){
		string::iterator p = ss.begin();
		while(p!= ss.end()){
			if(*p!=' '&&*p!='\t'&& *p!='\n' && *p!='\r') break;
			p++;
		}
		ss.erase(ss.begin(),p);
		return ss;			
	}
	
	inline  string& rtrim(string &ss){
        string::reverse_iterator p = ss.rbegin();
        while(p!= ss.rend()){
        	if(*p!=' '&&*p!='\t' && *p!='\n' && *p!='\r') break;
        	p++;
        }	
        ss.erase(p.base(),ss.end());
        return ss;
	}
	
	inline string& trim(string &st){
		ltrim(rtrim(st));
		return st;
	}

    /*    
    void filter_string( string & str,string filt_elems = string("\",?.")) 
    {
        // 过滤元素
        //string filt_elems( "\",?.");
        string::size_type pos = 0;
        while (( pos = str.find_first_of( filt_elems, pos ))
                != string::npos ){           
         str.erase( pos, 1 );
        }
    }
   
    
    //以下是filter_string()的更一般化的版本它接受一对iterator 由它们标记出需要过滤的元素范围
    template <class InputIterator>
    void filter_string( InputIterator first, InputIterator last,
    string filt_elems = string("\",?."))
    {
        for ( ; first != last; first++ ){
            string::size_type pos = 0;
            while (( pos = (*first).find_first_of( filt_elems, pos ))
                        != string::npos )
                (*first).erase( pos, 1 );
        }
    }
     */
};
#endif //_FLOWLINE_UTIL_H_
