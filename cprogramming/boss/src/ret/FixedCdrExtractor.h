// ##########################################
// Source file : FixedCdrExtractor.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20011123
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef FIXEDEXTRACTCDR_H_INCLUDED_C486352C
#define FIXEDEXTRACTCDR_H_INCLUDED_C486352C

#include "config-all.h"

#include <string>
#include "DataFormat.h"
#include "CdrExtractor.h"

#include "appfrm/ErrorHandle.h"
extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std)

//##ModelId=3B51462C01EF
//##Documentation
//## 定长记录处理对象。通用类，继承自GeneralCdr抽象类。
class FixedCdrExtractor : public CdrExtractor {
  public:
	//##ModelId=3B51520D03AB
	//##Documentation
	//## 顺序调用所有字段中的GetSrcValue方法，得到全部字段的原始数据。
	virtual int getNextSrcCdr(unsigned char *&srcCdrBuffer);

	//##ModelId=3B8209FF020F
	virtual bool initialize(const string theCdrParameter,const int theBlockHeaderSize) ;


  private:
	//##ModelId=3BFE0CED02DD
   int getcdrSize(const unsigned char *srcCdrBuffer);
  private:
	//##ModelId=3B847A5100DD
  	int m_CdrSize;

	//##ModelId=3BFE0AF10015
	int m_StartByte;

	//##ModelId=3BFE0B3203B2
	int m_Length;

	//##ModelId=3BFE0BAE0021
	string m_dataType;

	//##ModelId=3BFE0BC403D0
	char m_padding;

	//##ModelId=3BFE0BD70188
	int m_order;

	//##ModelId=3BFE0E0F027F
	DataFormat m_DataFormat;
};



#endif /* FIXEDEXTRACTCDR_H_INCLUDED_C486352C */
