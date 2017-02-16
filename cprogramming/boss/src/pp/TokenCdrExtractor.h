// ##########################################
// Source file : TokenCdrExtractor.h
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

#ifndef TOKENCDREXTRACTOR_H_HEADER_INCLUDED_B7A653AB
#define TOKENCDREXTRACTOR_H_HEADER_INCLUDED_B7A653AB

#include "config-all.h"
#include "CdrExtractor.h"
#include "PpLog.h"
#include <string>

USING_NAMESPACE(std);

//##ModelId=48522F3F02CE
//##Documentation
//## 分隔符类型记录处理对象. 通用类,继承自CdrExtractor抽象类
class TokenCdrExtractor : public CdrExtractor
{
  public:
    //##ModelId=48522F7200AB
    virtual int getNextSrcCdr(unsigned char *&srcCdrBuffer);

    //##ModelId=48522F820138
    virtual bool initialize(const string &theCdrParameter, const int theBlockHeaderSize);

    //##ModelId=48522F9300EA
    virtual void setToken(const string token);

  private:
    //##ModelId=48522FBD001F
    int getcdrSize(const unsigned char *srcCdrBuffer, int &controlCount);

    //##ModelId=48522FD700DA
    //##Documentation
    //校验该条读入的cdr记录中的字段数是否和配置文件中所配置的相同[不再校验]
    //bool checkTokenCount(const unsigned char *buffer, const int len);

  private:
    //##ModelId=48522FFC0000
    int m_CdrSize;

    //##ModelId=4852300303B9
    char m_token;

};



#endif /* TOKENCDREXTRACTOR_H_HEADER_INCLUDED_B7A653AB */
