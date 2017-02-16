// ##########################################
// Source file : TokenRawCdrProcessor.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef TOKENRAWCDRPROCESSOR_H_HEADER_INCLUDED_B7A67500
#define TOKENRAWCDRPROCESSOR_H_HEADER_INCLUDED_B7A67500

#include "RawCdrProcessor.h"
#include "config-all.h"
#include <string>

USING_NAMESPACE(std);

//##ModelId=4851D7AB03A9
//##Documentation
//## 分隔符格式话单记录处理类
class TokenRawCdrProcessor : public RawCdrProcessor
{
  public:
  	TokenRawCdrProcessor(const int iSubChannelNum):RawCdrProcessor(iSubChannelNum){}
    //##ModelId=4851D7D702AF
    virtual int getDesCdr(const unsigned char *aCdrValue, int length);

    //##ModelId=4851D8180261
    virtual bool initialize(vector<string> aParameterList);

    //##ModelId=4851D831006D
    int getCdrSign();

};



#endif /* TOKENRAWCDRPROCESSOR_H_HEADER_INCLUDED_B7A67500 */
