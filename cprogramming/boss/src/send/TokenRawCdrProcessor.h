// ##########################################
// Source file : TokenRawCdrProcessor.h
// System      : Mobile Billing System
// Version     : 1.03
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010817
// Update      : 20010906
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef TOKENCDR_H_INCLUDED_C483119D
#define TOKENCDR_H_INCLUDED_C483119D

#include "config-all.h"

#include <string>
#include "RawCdrProcessor.h"

USING_NAMESPACE(std)

//##ModelId=3B6020EB00F7
//##Documentation
//## 分隔符格式话单记录处理类
class TokenRawCdrProcessor : public RawCdrProcessor
{
  public:
	//##ModelId=3B60214600F8
	virtual int getDesCdr(const unsigned char *aCdrValue,int length,
						  multimap<MyString,FieldInfo *> &m_theFieldInfoMaps) ;

	//##ModelId=3B96EBE500C4
	virtual bool initialize( vector<string> aParameterList)  ;

	//##ModelId=3B66B74F0002
	int getCdrSign();
};



#endif /* TOKENCDR_H_INCLUDED_C483119D */
