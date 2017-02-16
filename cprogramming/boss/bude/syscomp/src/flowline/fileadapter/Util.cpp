#include "Util.h"
#include <map>
#include <string>
#include <time.h>
#include <vector>
#include <iostream>

using namespace std;
using namespace flowline;

namespace flowline {
    
// 解析 aa=123;bb=456;cc=123 这样的键值对.分割符号可以为回车
void parseParameter(const string& configure,map<string ,string >& param){
   char ch = '=';           //键对
   const char* sp = ";\n";  //分割符
   int pos , ref;  
   string key , value; 
   string conf(configure); 
      
   while((pos = conf.find_first_of( ch ))> 0 ){   
      key = conf.substr(0, pos);
      value = conf.substr(pos + 1); 
      while(key.find_first_of(sp)==0){
         key = key.substr(1);//去掉重复分割符
      }
      if((ref = value.find_first_of( sp )) >= 0 ){
         conf  =  value.substr(ref + 1);
         value =  value.substr(0,ref);
         param.insert( pair<string,string> (key,value));
      } else {         
         param.insert( pair<string,string> (key,value));   
         break; 
      }          
  }//end while
}       
    
//从map 里取值
string getParameter(const string& key,const map<string ,string >& param)
{
   	 map<string,string>::const_iterator it = param.find(key);
     return (it != param.end()) ? it->second : "";
}
// print
void printParameter(const map<string ,string >& param)
{
	map<string,string>::const_iterator pos = param.begin();
    while(pos != param.end())
    {   
        cout << pos->first << "=" << pos->second << endl;
        ++pos;
    }   
}

//cout << Parameter
ostream & operator << (ostream & os, const map<string ,string> & param)
{
	map<string,string>::const_iterator pos = param.begin();
    while(pos != param.end())
    {   
    	os  << pos->first << "=" << pos->second << endl;
        ++pos;
    }
    return os;
}



// 把${macro}宏和非宏依次存到vector中: pair<string, bool> bool=true 为宏 
void parserMacro(const string& instr , vector< pair<string,bool>  > & va)
{
    int his = 0;
    int pos = 0; 
    char sp = '$';
    char en = '}'; 
    string constr;
    while( pos >= 0 )
    {      
		pos = instr.find(sp, his);	
		if(his!=pos){
            if( pos < 0){
                constr = instr.substr(his);
                va.push_back(pair<string , bool> (constr,false));               
                break;  
            } else {
                constr = instr.substr(his,pos-his);
                va.push_back(pair<string , bool> (constr,false));                	
            }
        }     
        his = pos;
       	pos = instr.find(en, his);	
       	if(pos < 0){		  
                break;  
        } else {
    	       constr = instr.substr(his+2 , pos-his+1-3);
    	       va.push_back(pair<string , bool> (constr,true));
    	       //cout << "varstr:" << constr << endl;		
        } 
        his = pos + 1;
    }   
}

//把"sub(123,234)"函数的名字存在args[0],参数依次args[1]~args[n]
void parserMethod(const string& key, vector <string>& args)
{
        string arg;
        int pos = key.find('(');
        int his = key.find(')');
        if(pos < 0 || his < 0 || pos == key.size() - 2){          
            arg = (pos <= 0) ? key : key.substr(0, pos); 
            args.push_back(arg);   //只有函数名没有参数//args.front();
            return ;       
        } 
        arg = key.substr(0, pos);       
        args.push_back(arg);
        his  = pos + 1 ;
        while ((pos = key.find(",",his)) > 0){           
           arg = key.substr(his , pos - his);          
           args.push_back( arg );
           his = pos + 1;           
        } 
        arg = key.substr(his,key.size() - his -1);       
        args.push_back(arg);
}

string resultMacro(const string & instr,MacroAttribute & macro){
    vector< pair<string,bool>  >  ora;
    parserMacro(instr,ora);
    return resultMacro(ora,macro);   
}

// Macro & macro
string resultMacro(vector< pair<string,bool> > & ora,MacroAttribute & macro){
   ostringstream sstr;   
   vector< pair<string,bool> >::iterator pos = ora.begin();
    while(pos != ora.end())
    {   
        if(pos->second == false){    	   
    	    sstr << pos->first;    	    
        } else {
            string  value = macro.getAttribute((string)pos->first);
            sstr << value;
        }      
        ++pos;
    }   
    return sstr.str();    
}

string resultMacrofilter(const string & instr,MacroAttribute & macro){
    vector< pair<string,bool>  >  ora;
    parserMacro(instr,ora);
    return resultMacrofilter(ora,macro);   
}
//联合宏的值,从MacroAttribute取值
string resultMacrofilter(vector< pair<string,bool> > & ora,MacroAttribute & macro){
   ostringstream sstr;   
   vector< pair<string,bool> >::iterator pos = ora.begin();
    while(pos != ora.end())
    {   
        if(pos->second == false){          
            sstr << pos->first;         
        } else {
            string  value = macro.getAttributeFilter((string)pos->first);
            sstr << value;
        }      
        ++pos;
    }   
    return sstr.str();    
}
    

}//end namespace

using namespace flowline;
int main2(){
	string str = "aa=0fdf01;\nbb=2\n;;cc="  ;
    
  cout << str <<endl << "-----------" << endl;
  map <string,string> param;
  try {
  	parseParameter(str,param); 
  	//printParameter(param); 	
  	//cout << getParameter(param,"aa") << endl;
  	cout << param;
  }catch(exception& e){
    cout << e.what() <<endl;
  } catch(...){
  	 cout << "error" <<endl;
  }
  cout <<"--end--"<<endl;
	system("echo flowline::Util.main");return 0;
			
}
