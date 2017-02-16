/******************************************************************
 *Copyright (c) 2002-2003 南京联创公司联通软件开发中心
 *创建人: 陈忠敏
 *日　期: 2006.11.7
 *描　述:
 *版　本: 1.0
 *****************************************************************/
//惠普主机需要定义此宏
#ifndef _HP_NAMESPACE_STD
#define _HP_NAMESPACE_STD
#endif

#ifndef __DAETOOLS_H__
#define __DAETOOLS_H__

//#include <stdarg.h>
//#include "math.h"
//#include "Uunix.h"
//#include <atmi.h>
//#include <fml32.h>
//#include <fml1632.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <time.h>

#include <iostream>
#include <sstream>

using namespace std;

extern string LogFileName;
//记录日志到LogFileName定义的文件中
int muserlog(const char * formate,...);
//获取主机时间
//format:"%Y%m%d%H%M%S","%Y%m%d","%Y-%m-%d %H:%M:%S"
string GetHostDate(const char *format = "%Y%m%d");
//获取指定字符串中第cIndex个以SplitChar分割的字符串（cIndex从1开始编号）
int GetSubStr(char* SourceString,int cIndex,char SplitChar,char* cDest);
//判断srcstr是否在后续的字符串列表中，最后一个参数以NULL结束
int strinlist(const char *srcstr,...);
//守护进程初始化函数：main函数的入口调用，可使该进程成为守护进程
void daemon_init(const char * pname,int facility);
//获取一个环境变量
string GetEnv(const char * envstr);

void StrToUpper(char *str);

void StrToLower(char *str);

char *ltrim(char *string);

char * rtrim(char *string);
//从ini文件中读取信息
int ReadIniFileInfo(const char *cFileName,
					const char *cSectionName,
					const char *cKeyName,
					string &strValue,
					string &strErrMsg);
bool file_exists(const char * filename);
long file_size(FILE *fp);
long file_size(const char *filename);
/*
class TuxCall
{
public:
	TuxCall(long lSendBufSize,long lRecvBufSize);
	~TuxCall();
	int fchg32(FLDID32 fieldid,FLDOCC32 rownum,const string& value);
	int fget32(FLDID32 fieldid,FLDOCC32 rownum,string& value);
	int fchg32(const char *fldname,FLDOCC32 rownum,const string& value);
	int fget32(const char *fldname,FLDOCC32 rownum,string& value);
	int callsvc(const char *svcname);
	int vartobuf(int rownum,...);
	void loginbuff(const char * servername,const char *logfilename);
	void logoutbuff(const char * servername,const char *logfilename);
	string errmsg;
private:
	FBFR32 *sendBuf,*recvBuf;
	FLDID32 errorfieldid;
	void logbuff(const char * servername,FBFR32 *rqst,const char *logname);
};
*/
//tuxedo 函数 
//

#endif
