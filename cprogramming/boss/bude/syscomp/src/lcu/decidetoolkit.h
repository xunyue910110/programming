/**
 * <p>Title:  DecideToolkit类，该类封装了拆分条件表达式的方法和提交表达式计算的方法</p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004</p>
 * <p>Company: Linkage</p>
 * @author juyf@lianchuang.com
 * @version 1.0
 * @date 2004-12-20
 */

#ifndef LCU_DECIDE_H
#define LCU_DECIDE_H

#include <string>
#include <map>
#include <vector>

#include "lcu/lcu.h"
#include "lcu/plugins.h"


namespace ebpm4c{
	
typedef vector <string> VarArray;
typedef map <string,string> ConstArray;

struct TransItem { 
  string transname;
  bool iselse;
  string configure; 
  VarArray varlist;//变量名表
  ConstArray constmap;//string常量值表对, @1@;
};

typedef vector<TransItem> TransList;

class DecideToolkit{
	private:
	   Context* context;	  
	   //获得变量的值
	   string getVariantValuebyName(const string& vname);
	   bool errorDecision;
	   
	public:	
	
		DecideToolkit(){errorDecision = false; }
		virtual ~DecideToolkit(){};
		//解析脚本，输出到路径列表中
		void parseScript(const string& script,TransList& translist);	       
		/* getDecideResult 返回表达式结果 通过表达式得到下一路径名
		 * @param ctx  上下文
		 * @param translist 路径列表
		 */
		string getDecideResult(Context* ctx,TransList& translist);	 
		bool isPassExpression(const string& expr); 
		 
		inline string& ltrim(string &ss);		
		inline string& rtrim(string &ss);		
		inline string& trim(string  &st); 
      void setErrorDecision(bool isErrorDecision);
      bool isErrorDecision();
       //打印一下
      void printtranslist(TransList&  translist);
   private:	    
      bool isFunction(const string& expr);  
      /**
      * 是否为操作符，非操作符=0 ，算术符=1，逻辑符号=2 
      */
      int isOpertor(const char& c);
      // <= >= != == && ||是双符号逻辑符号，不包含 += *= -= ++ --等双运算符 
      bool islogic(char la ,char cu);
      //是否为数字
      bool isnumber(const string& st);
      //如果是字符串数值的不加,字符串: 'a3' 
      void addVariant(const string& v, VarArray& va, ConstArray& mc);
      //得到变量列表和常量列表
      void getVariant(const string& exprstr,VarArray& va,ConstArray& mc);
      //把字符串常量用，map的key替代掉
      void compileconst(string& rstr,ConstArray& mc);     
      //取得把表达式里的变量用数值替代
      string putContext2expression(const string& str, VarArray* va, ConstArray* mc);
};
};	

#endif // DECIDE_H
