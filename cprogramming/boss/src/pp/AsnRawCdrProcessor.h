// ##########################################
// Source file : AsnRawCdrProcessor.h
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

#ifndef ASNRAWCDRPROCESSOR_H_HEADER_INCLUDED_B7A67208
#define ASNRAWCDRPROCESSOR_H_HEADER_INCLUDED_B7A67208

#include "RawCdrProcessor.h"
#include "config-all.h"
#include <string>
#include <stdio.h>
#include <iostream>

USING_NAMESPACE(std);

//##ModelId=4851D511034B
//##Documentation
//## ASN 格式话单记录处理类
class AsnRawCdrProcessor : public RawCdrProcessor
{
  public:
    //##ModelId=4851D52000DA
    AsnRawCdrProcessor(const int iSubChannelNum):RawCdrProcessor(iSubChannelNum){}

    //##ModelId=4851D522030D
    virtual ~AsnRawCdrProcessor(){}

    //##ModelId=4851D535002E
    virtual int getDesCdr(const unsigned char *aCdrValue, int length);

    //##ModelId=4851D5780167
    virtual bool initialize(vector<string> aParameterList);

  private:
    //##ModelId=4851D59601C5
    string getCdrSign(const unsigned char *srcCdrValue, int CdrLength, FieldExtractRule *aField, RecordFormat &theRecordFormat);

};



#endif /* ASNRAWCDRPROCESSOR_H_HEADER_INCLUDED_B7A67208 */
