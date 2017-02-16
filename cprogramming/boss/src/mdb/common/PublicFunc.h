// ############################################
// Source file : PublicFunc.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-4-23
// Update      : 2008-4-23
// Copyright(C): chen min, Linkage.
// ############################################

#ifndef PUBLICFUNC_H_200806011
#define PUBLICFUNC_H_200806011

#include "vector"
#include "base/config-all.h"

USING_NAMESPACE(std)
class InputParams;
class PublicFunc
{
	public:
		static int splitStr(const void *request,void ** &pValues);
		static int splitStr(const void *request,void ** &pValues,int * &pValuesLenth); // add by chenm 2009-7-9 增加pValuesLenth参数,记录server传过来的字段值的长度
		static int splitStr(const void *request,InputParams * &pInputParams);
		//通用的解析,数据集的串,格式fileldNum,length(field1)field1,length(field2)field2,length(field3)field3
		static int splitStr(const char *inPutStr,vector<string> &outPutStrs);
		static int splitStr(const void *request,vector<InputParams *> &pInputParams);

		//功能：大端字节返回true,否则返回false
		static bool isBigEndian();
	
	public :
		static int m_offSet;
		
};

#endif 
