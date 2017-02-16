// ##########################################
// Source file : TokenFormat.h
// System      : Mobile Billing System
// Version     : 1.03
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010823
// Update      : 20020529
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef TOKENFORMAT_H_INCLUDED_C4906214
#define TOKENFORMAT_H_INCLUDED_C4906214
#include "RecordFormat.h"


//##ModelId=3B6022570303
//##Documentation
//## 为纯方法类，通用类。
//## 包含分隔符记录的解包算法。
class TokenFormat : public RecordFormat {
  public:
	//##ModelId=3B60229202FE
	virtual void getContents();

	//##ModelId=3B84FD0F0329
	virtual int getCdrSize(const char *theSign,const char *cdrTmp,int &aCdrOffset);

	//##ModelId=3B9587360244
	virtual int getValue(unsigned char * theSign,unsigned char *aValue,int position);

	//##ModelId=3B96E4350222
	virtual void initialize() ;
	//##ModelId=3C048D7702B8
	TokenFormat();


};

#endif /* TOKENFORMAT_H_INCLUDED_C4906214 */
