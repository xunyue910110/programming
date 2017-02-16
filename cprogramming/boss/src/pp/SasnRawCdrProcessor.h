// ##########################################
// Source file : SasnRawCdrProcessor.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef SASNRAWCDRPROCESSOR_H_HEADER_INCLUDED_C4835451
#define SASNRAWCDRPROCESSOR_H_HEADER_INCLUDED_C4835451

#include "config-all.h"
#include "RawCdrProcessor.h"
#include "SasnFormat.h"
#include <string>
#include <stdio.h>
#include <iostream>

USING_NAMESPACE(std);

//##ModelId=3B6020D802F8
//##Documentation
//## 西门子 ASN 格式话单记录处理类
class SasnRawCdrProcessor : public RawCdrProcessor
{
  public:
  	SasnRawCdrProcessor(const int iSubChannelNum):RawCdrProcessor(iSubChannelNum){}

	//##ModelId=3B978CA501F4
	virtual~SasnRawCdrProcessor(){}

	//##ModelId=3B60213F0198
	virtual int getDesCdr(const unsigned char *aCdrValue, int length);

	//##ModelId=3B96EBE302A1
	virtual bool initialize( vector<string> aParameterList)  ;

  private:
	//##ModelId=3B66B75301F2
	string getCdrSign(const unsigned char *srcCdrValue	, int CdrLength, FieldExtractRule *aField , RecordFormat &theRecordFormat);

};

#endif /* SASNRAWCDRPROCESSOR_H_HEADER_INCLUDED_C4835451 */
