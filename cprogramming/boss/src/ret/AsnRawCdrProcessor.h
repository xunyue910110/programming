// ##########################################
// Source file : AsnRawCdrProcessor.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX,AIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010817
// Update      : 20011127
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef ASNCDR_H_INCLUDED_C4835451
#define ASNCDR_H_INCLUDED_C4835451

#include "config-all.h"
#include <string>
#include <stdio.h>
#include <iostream>

#include "RawCdrProcessor.h"

USING_NAMESPACE(std);

//##ModelId=3B6020D802F8
//##Documentation
//## ASN 格式话单记录处理类
class AsnRawCdrProcessor : public RawCdrProcessor
{
  public:

	//##ModelId=3B978CA501F4
	virtual~AsnRawCdrProcessor();


	//##ModelId=3B60213F0198
	virtual int getDesCdr(const unsigned char *aCdrValue,int length,
						       multimap<MyString,FieldInfo *> &m_theFieldInfoMaps) ;

	//##ModelId=3B96EBE302A1
	virtual bool initialize( vector<string> aParameterList)  ;

  private:
	//##ModelId=3B66B75301F2
	string getCdrSign(const unsigned char *srcCdrValue	,int CdrLength,
					      FieldExtractRule *aField ,RecordFormat &theRecordFormat);

};

#endif /* ASNCDR_H_INCLUDED_C4835451 */
