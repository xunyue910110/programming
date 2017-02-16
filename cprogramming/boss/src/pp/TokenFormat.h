// ##########################################
// Source file : TokenFormat.h
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

#ifndef TOKENFORMAT_H_HEADER_INCLUDED_B7A60D53
#define TOKENFORMAT_H_HEADER_INCLUDED_B7A60D53

#include "RecordFormat.h"
class FieldInfo;

//##ModelId=4855C15A02DE
//##Documentation
//## 为纯方法类,通用类
//## 包含分隔符记录的解包算法
class TokenFormat : public RecordFormat
{
  public:
    //##ModelId=4855C16A038A
    TokenFormat();

    //##ModelId=4855C1770261
    virtual void getContents();

    //##ModelId=4855C18E0000
    virtual int getCdrSize(const char *theSign, const char *cdrTmp, int &aCdrOffset);

    //##ModelId=4855C1BC01C5
    virtual int getValue(unsigned char * theSign, FieldInfo *pFieldInfo, int position, int theLength, bool &tagAll);

    //##ModelId=4855C2160232
    virtual void initialize();

};



#endif /* TOKENFORMAT_H_HEADER_INCLUDED_B7A60D53 */
