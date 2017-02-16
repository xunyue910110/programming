// ##########################################
// Source file : TokenCdrExtractor.h
// System      : Mobile Billing System
// Version     : 0。1.3
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20011231
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef TOKENEXTRACTCDR_H_INCLUDED_C4866CCD
#define TOKENEXTRACTCDR_H_INCLUDED_C4866CCD

#include "config-all.h"

#include <string>
#include "CdrExtractor.h"

USING_NAMESPACE(std)

//##ModelId=3B6012CB021A
//##Documentation
//## 分隔符类型记录处理对象。通用类，继承自GeneralCdr抽象类。
class TokenCdrExtractor : public CdrExtractor
{
  public:
	//##ModelId=3B60136001BA
	//##Documentation
	//## 分隔符类型记录处理对象。通用类，继承自GeneralCdr抽象类。
	virtual int getNextSrcCdr(unsigned char *&srcCdrBuffer);

	//##ModelId=3B820A00030B
	virtual bool initialize(const string theCdrParameter,const int theBlockHeaderSize) ;

	virtual void setToken(const string token);
  private:
	//##ModelId=3C30275E00A5
   int getcdrSize(const unsigned char *srcCdrBuffer,int &controlCount);

	//##ModelId=3C4CF8D00138
	int m_CdrSize;
	
	bool checkTokenCount(const unsigned char *buffer,const int len);
	
	char m_token;

};



#endif /* TOKENEXTRACTCDR_H_INCLUDED_C4866CCD */
