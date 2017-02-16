#ifndef _FMLOPERATOR_H_
#define _FMLOPERATOR_H_

#include <iostream>
#include <string>
#include "mwci/mwci.h"
#include "mwlcu/fmlcontext.h"
#include <iterator>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;
using namespace ebpm4c;

namespace ebpm4c{
	
enum Operation{
	OP_NONE,          //;
	OP_BUF_ADD,	      //b1 +=b2;   buf加等于
	OP_BUF_MUTI,	  //b1 *=b2;   buf乘等于
	OP_PRINT_VALUE,	  //?b2.a[2];
	OP_COVER_LINE,    //b1[0]= b2[1];//从b2中取第一行盖掉b1第0行
	OP_BUF_NULL,      //b1 = NULL;	//从Context中移除b1
	OP_COPY_BUFF,     //b1 = b2;		
	OP_FLD_NULL,      //b1.c = NULL; //从b1中移除c字段
	OP_COPY_FLD,      //b1.c = b3.d ; //拷贝d字段的值到c字段
	OP_SET_VALUE_BUF, //b1.b[0] = b3.c[0];
	OP_SET_VALUE,     //b1.d = 0.0 ;  //int/double  数字型可以给字符串
	OP_SET_VALUE_STR, //b2.c = "1" ;  //char*       字符串不能给数字,加""肯定是字符串
	OP_PLUSPLUS,      //b3.c++  ;
	OP_SUBTRACT,      //b4.c--  ;
	OP_OTHER
};	

struct OperItem{
	string origin;
	Operation oper;
	string   leftbuf; 
	string   leftfld;
	int      leftidx;
	string   leftidx_var;//左边下标是变量
	string   rightbuf;
	string   rightfld;
	int      rightidx;
	string   rightidx_var;//左边下标是变量  
	string   value;     //常量	已经去掉了引号""
	bool     isIdx_Var; //这个为true还得去查 idx_var是否为空，不空取下标变量，空的话，还是用idx作为下标
	bool     isAll;     //是否已经解析完成 true完成
};


class FmlOperator
{
public:
	FmlOperator();
	virtual ~FmlOperator();
	void print(const OperItem& item);
	
	//用流去解析
    void parser(const string& config,vector <OperItem>& operlist);
	
    void execute(FMLContext& ctx,vector <OperItem>& operlist);



	
	
private:	
	//解析一条语句，包括';\n'
    void parserline(string& line,OperItem& opItem);
	/**        
	GetFldType
	- 0	short int
	- 1	long int 
	- 2	character
	- 3	single-precision float 
	- 4	double-precision float 
	- 5	char * - null terminated
	- 6	character array 
	*/
    bool execute(FMLContext& ctx, OperItem & item);
    //字段对考
	bool copyFld(PCFmlBuf pbuf,PCFmlBuf rbuf,const string& leftfldname,const string& rightfldname,int fldtype,int fldtype2);

	inline string& ltrim(string &ss);	
	inline string& rtrim(string &ss);	
	inline string& trim(string &st);

};
}
#endif //_FMLOPERATOR_H_
