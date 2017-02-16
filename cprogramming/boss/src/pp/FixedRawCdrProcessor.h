// ##########################################
// Source file : FixedRawCdrProcessor.h
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

#ifndef FIXEDRAWCDRPROCESSOR_H_HEADER_INCLUDED_B7A62587
#define FIXEDRAWCDRPROCESSOR_H_HEADER_INCLUDED_B7A62587


#include "config-all.h"

#include <string>
#include "RawCdrProcessor.h"

USING_NAMESPACE(std);

//##ModelId=4851D644033C
//##Documentation
//## 定长格式话单记录处理类
class FixedRawCdrProcessor : public RawCdrProcessor
{
  public:
  	FixedRawCdrProcessor(const int iSubChannelNum):RawCdrProcessor(iSubChannelNum){}
    //##ModelId=4851D6570203
    virtual int getDesCdr(const unsigned char *aCdrValue, int length);

    //##ModelId=4851D6B700CB
    virtual bool initialize(vector<string> aParameterList);

    //##ModelId=4851D6D5035B
    int getCdrSign();

};



#endif /* FIXEDRAWCDRPROCESSOR_H_HEADER_INCLUDED_B7A62587 */
