#ifndef RR_EXCEPTION_H_INCLUDE_20080818_
#define RR_EXCEPTION_H_INCLUDE_20080818_

#include "config-all.h"
#include <string>
#include <stdarg.h>
#include <stdexcept>
USING_NAMESPACE(std);

//??异常处理类
class RrException :public std::exception
{
  public:
  	//打印错误信息
  	void toString();
	
  	//获取错误信息
  	char *GetString();
  	int   getErrcode();
  	
  	//??构造函数
  	RrException(char * filename, int linenum,int errCode,const char * format, ...);
  	RrException(char * filename, int linenum,const char * format, ...);
  	RrException(char *  errstr);
  	RrException();
	
  	//??析构函数
  	//~RrException();
  protected:
  	string  m_filename;//异常发生的文件
  	int  m_linenum; //异常发生的行数 updated by hanyu 08-12-08
  	int     m_errCode; //异常代码
  	string  m_msg;     //异常信息
  	char    m_errstr[1024*2+1];//错误信息
};

#define RR_THROW(msg) throw RrException(__FILE__, __LINE__, -1, msg)
#define RR_THROW_C(code, msg) throw RrException(__FILE__, __LINE__, code, msg)
#define RR_THROW_CP1(code,msg,p1) throw RrException(__FILE__, __LINE__, code,msg,p1)
#define RR_THROW_CP2(code,msg,p1,p2) throw RrException(__FILE__, __LINE__, code,msg,p1,p2)
#define RR_THROW_CP3(code,msg,p1,p2,p3) throw RrException(__FILE__, __LINE__, code,msg,p1,p2,p3)

#endif // RR_EXCEPTION_H_INCLUDE_20080818_

