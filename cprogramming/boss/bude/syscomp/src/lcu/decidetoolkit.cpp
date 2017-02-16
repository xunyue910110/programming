/**
 * <p>Title:  DecideToolkit类，该类封装了拆分条件表达式的方法和提交表达式计算的方法</p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: Linkage</p>
 * @author juyf@lianchuang.com
 * @version 1.0
 * @date 2004-12-20
 */

#include "decidetoolkit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <functional>
#include "CSExpression.h"


using namespace std;
using namespace ebpm4c;

void DecideToolkit::setErrorDecision(bool isErrorDecision){
   this->errorDecision = isErrorDecision;
}
bool DecideToolkit::isErrorDecision(){
   return this->errorDecision;
}

//通过变量名，取得变量值
string DecideToolkit::getVariantValuebyName(const string& vname){  
   if(this->errorDecision){
      int p=vname.find('.');
      if(p > 0 && p < vname.size()){
         return this->context->getVariant(vname);
      } else {
         char str[10];
         sprintf(str,"%d",this->context->getErrorCode());
         return str;
      }
   }
   return this->context->getVariant(vname);
};

//返回路径名
string DecideToolkit::getDecideResult(Context* ctx,TransList& translist){
	this->context = ctx;
	TransItem* item;
	TransList::iterator it = translist.begin(); 
	string retrans = "NOTRANSITION"; 
	bool transok = false;
	string expr;   
	for(; it != translist.end(); it++){ 
		item = &(*it); 
		
		//cout << "-------:---------"<< endl;
		//cout << "transname:" << item->transname<< endl;
		//cout << "configure:" << item->configure<< endl;
		//cout << "iselse:"  << item->iselse<< endl;
		//cout << "varlist:" << item->varlist.size()<<endl; 	
		//cout << "constmap:" << item->constmap.size()<<endl;
		
		if(item->iselse){
			transok = true;
			retrans = item->transname;
			break;
		}   
		expr = putContext2expression(item->configure,&item->varlist,&item->constmap);
		
		if(!expr.empty() && expr.at(0)=='-'){
		    expr = "0" + expr;
		}
		transok = isPassExpression(expr);
		
		if(transok){
			retrans = item->transname;
			break;
		}      	   
	}	
	if(!transok && this->errorDecision){
		//没有找到且是error批判则抛出异常,
		//正常流程没有找到都可以继续执行,即流程结束。
		//throw execution_error("[LCU]ErrorDecision找不到下一个节点的路径,作为异常抛出");
		THROW_C(LCUExecutionException,-100,"ErrorDecision找不到下一个节点的路径,作为异常抛出");
	}
	return retrans;
};
/**
* 判断表达式是否为真
* @TODO
*/
bool DecideToolkit::isPassExpression(const string& expresion){
	int pos = expresion.find("\"");
	if(pos >= 0) {  
		//cout << expresion << ":"<< pos << endl;		
		THROW(LCUDefinitionException,"[LCU]表达式有非法字符.表达式中禁止包含\"\":" + expresion );  
	}	
	string result = "0"; 
	//cout << "表达式:" << expresion <<endl;	
	if(expresion.size() <= 1 ){
		result = expresion;		  
	}else {	  
		CSExpression* expr = new CSExpression();   	  
		expr->ExpressionCompile(expresion.c_str(),true);//true逻辑 false数学    
		if(expr->isException()){
			string errinfo = "[ebm4c]表达式.ExpressionErrorMessage:" ;
			errinfo += expr->getErrorMessage(); 
			THROW(LCUDefinitionException,errinfo);
		} else {
			result = expr->getResult();
		} 	
		delete expr;
	}		
	//cout << "返回值:" << result <<endl;	
	return (""!=result)?(result!="0"):false;
}
bool DecideToolkit::isFunction(const string & expr){
   bool r = false;
   if("substr" == expr || "strlen" == expr){
      r = true;
   } else if("atoi" == expr || "atof" == expr || "atol" == expr){
      r = true;   
   }else if("trim" == expr || "sysdate" == expr){
      r = true;
   }else if("YYYY" == expr || "yyyy" == expr || "YY" == expr || "yy" == expr || "MM" == expr || "mm" == expr || "DD" == expr || "dd" == expr || "HH24" == expr || "hh24" == expr || "HH" == expr || "hh" == expr || "SS" == expr  || "ss" == expr){
      r = true;
   }else if("lower" == expr || "upper " == expr){
      r = true;
   }else if("datetimediff" == expr || "datetimeadd" == expr || "print" == expr){
      r = true;   
   }   
   return r;
}

//如果是字符串数值的不加,字符串: 'a3' 
void DecideToolkit::addVariant(const string& varname, VarArray& va, ConstArray& mc){
     string vv = string(varname);
     string v = trim(vv);
     int ls = v.size()-1;   
     if(ls < 0){
        return;
     }     
     if(isFunction(v)){
        return ;
     }
     string constkey = "@";
     if((v.at(0)=='\'' &&  v.at(ls)=='\'') || (v.at(0)=='\"' && v.at(ls)=='\"')) { 
     	   constkey += mc.size(); 
     	   constkey += "@"; 
     	   string vs = v.substr(1,ls-1);//内容取出来把所有""换成''
       	   v = "'" + vs + "'";  
       	   //cout << "[LCU]常量:" << v <<endl;       	      	   
           mc.insert(pair<string,string>(constkey,v));              
     } else {
          //cout << "[LCU]变量:" << v <<endl;    
     	  va.insert(va.end(),v);
     }
};

inline string& DecideToolkit::ltrim(string &ss){
	string::iterator p = ss.begin();
	while(p!= ss.end()){
		if(*p!=' '&&*p!='\t'&& *p!='\n' && *p!='\r') break;
		p++;
	}
	ss.erase(ss.begin(),p);
	return ss;			
}

inline string& DecideToolkit::rtrim(string &ss){
	string::reverse_iterator p = ss.rbegin();
	while(p!= ss.rend()){
		if(*p!=' '&&*p!='\t' && *p!='\n' && *p!='\r') break;
		p++;
	}	
	ss.erase(p.base(),ss.end());
	return ss;
}

inline string& DecideToolkit::trim(string &st){
	ltrim(rtrim(st));
	return st;
}

//解析脚本到TransList中
void DecideToolkit::parseScript(const string& script,TransList& translist){
	//cout << "script:"<< script << endl;  
	string transvar = "transition";   
	int	transize = transvar.size();  	
	int pos=0;int pre=0;int kb,ke;
	bool iselse = false;
	string strbef,strend,strlast,transname,transif;   
	string nn = string(script);
	strlast = rtrim(nn);
	TransItem titem;     
   	while( pos >= 0 ){      
	      pos = strlast.find(transvar);          
	      if( pos <= 0 ) break;  
	      strbef = strlast.substr(0,pos);	  
	      kb = strbef.find('(');
	      ke = strbef.find_last_of(')');	    
	      if ( ke>kb && kb > 0) {
	         transif = strbef.substr(kb+1, ke-kb-1);
	      } else if (strbef.find("else")>=0){
	         iselse = true ;
	      } else {
	         break; //error
	      }	
        
	      pre = strlast.find(';') + 1;        
	      if( pre <= 0 ) break;	
	      strend = strlast.substr(pos + transize, pre-pos-transize);	    
	      kb = strend.find( '\"');
	      ke = strend.find_last_of('\"');
	      if ( ke > kb && kb > 0) {
	         transname = strend.substr(kb+1,ke-kb-1);
	      } else {	    	
	         break; //error
	      }
	      titem = TransItem();
	      	      
	      titem.transname = transname;
	      titem.configure = trim(transif);
	      titem.iselse = iselse;
	      translist.insert(translist.end(),titem);	
	        
	      iselse = false ;
	      transif = "";	
	      strlast = strlast.substr(pre);	  
   		}//end while     
   
	   TransList::iterator it = translist.begin();     
	   for(; it != translist.end(); it++){ 
	      TransItem& item = *it;
	      getVariant(item.configure,item.varlist,item.constmap);
	      compileconst(item.configure,item.constmap);       
	   }   
   //cout << "[LCU]javascript parse finished." << endl;    
}

/**
* 是否为操作符，非操作符=0 ，算术符=1，逻辑符号=2 
*/
int DecideToolkit::isOpertor(const char& c){
	int r = 0;
	switch (c){
   	case '+':
   	case '-':
   	case '*':
   	case '/':
   	case '%':
   	case '(':
   	case ')': 
   	case ',': 
   	    r = 1;
   	    break; 
   	case '>':
   	case '<':
   	case '=':
   	case '&':
   	case '|':
   	case '!':      
   	    r = 2;
   	    break;  
   	default:
   	    r = 0;
   	    break;     
	   }
	return r;
}

// <= >= != == && ||是双符号逻辑符号，不包含 += *= -= ++ --等双运算符 
bool DecideToolkit::islogic(char la ,char cu){
  bool r = (la== cu)?(cu=='&'||cu=='|'||cu=='='):false;
  bool t = (cu=='=')?(la=='>'||la=='<'||la=='!'):false;    
  return r||t; 
};

bool DecideToolkit::isnumber(const string& st){
    const char * p;
    p = st.c_str();
    char ch;  
    while(*p!='\0'){       
        ch = *p++;
        if(ch=='.'||ch==' '){
          continue;
        }
        if(!isdigit(ch))
          return false;
    }
   return true;
};


void DecideToolkit::getVariant(const string& str,VarArray& va,ConstArray& mc){
   const char * p;    
   p = str.c_str();
   int len = str.size();
   int his=0;//两个下标 
   int pre=0; 
   string v = "";
   char ch;
   for(pre=0;pre<len;p++,pre++){
     ch = *p;       
     if(isOpertor(ch)){            
       if(pre-his){//双符号忽略
           v = str.substr(his,pre-his);
           if(!isnumber(v)){
             addVariant(v,va,mc);                  
           } 
        }               
        his = pre + 1;  
      };        
    }
    if(pre-his){//取最后一笔 
        v = str.substr(his);
        if(!isnumber(v)){
            addVariant(v,va,mc);                 
        } 
     }  
};
//把字符串常量用，map的key替代掉
void DecideToolkit::compileconst(string& rstr,ConstArray& mc){
   int ps = 0;	
   string value;    
   
   string key = "\"" ;   	 
   ps = rstr.find(key,ps); 
   while( ps>= 0){
      rstr= rstr.replace(ps,key.size(),"\'");
      ps = rstr.find(key,ps);              
   }     
   ConstArray::const_iterator mit = mc.begin();
   for(; mit != mc.end(); mit++){
      key = mit->first;
      value = mit->second;
      ps = rstr.find( value , ps ); 
      if(ps >= 0){
      	rstr= rstr.replace(ps, value.size(), key);			          
      }
   //cout << "constkey:"<< key << " value:" << rstr << endl;
   }	
}
// 取得把表达式里的变量用数值替代 
string DecideToolkit::putContext2expression(const string& str, VarArray* va, ConstArray* mc){
     string rstr = string(str);
     int ps=0;
     string key;
     string varname;  
     string value;  
     //cout << "rstr:" << rstr <<endl ;
     VarArray::iterator it = va->begin();
     //cout << "VarArray.size:"<< va->size() <<endl;
     int lps = 0; 
     for(; it != va->end(); it++){ 
            key = string(*it);
            value = getVariantValuebyName(key);
             
            ps = rstr.find(key,lps); 
            //cout << "key:" << key << " value:" << value << " ps:" << ps << " lps:"<<lps<<endl;
            if(ps >=0 ){  	         	  
                  rstr = rstr.replace(ps,key.size(),value); 
                  lps = ps + 1 ;           
                  //ps = rstr.find(key,lps);
            }
	      	//cout << "key:"<< (*it) << " next:" << rstr<< endl;
     }
     //cout << "---after --- rstr:" << rstr << endl;
     ps = 0;
     ConstArray::const_iterator mit = mc->begin();
     for(; mit!=mc->end(); mit++){
         key = mit->first;
         value = mit->second;
         ps = rstr.find( key , ps ); 
         while(ps >= 0){
            rstr= rstr.replace(ps, key.size(), value);
            ps = rstr.find(key, ps);              
         }
         //cout << "constkey:"<< key << " value:" <<rstr<< endl;
     } 
     return rstr;
}

void DecideToolkit::printtranslist(TransList&  translist){
	TransItem item;
	TransList::const_iterator it = translist.begin();  
	cout << "-------printTranslist:---------"<< endl;   
	for(; it != translist.end(); it++){ 
		item = (TransItem)(*it); 
		cout << "-------:---------"<< endl;
		cout << "transname:" << item.transname<< endl;
      	cout << "configure:" << item.configure<< endl;
      	cout << "iselse:"  << item.iselse<< endl;
      	cout << "varlist:" << item.varlist.size()<<endl; 	
      	cout << "constmap:" << item.constmap.size()<<endl;
      	cout << "-------:---------"<< endl;      
     }	
}
