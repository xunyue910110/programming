// ##########################################
// Source file : AsnCdrExtractor.h
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

#ifndef ASNCDREXTRACTOR_H_HEADER_INCLUDED_B7A61B63
#define ASNCDREXTRACTOR_H_HEADER_INCLUDED_B7A61B63

#include "CdrExtractor.h"
#include "config-all.h"
#include "appfrm/ErrorHandle.h"
#include <string.h>

USING_NAMESPACE(std);

extern ErrorMessages * const theErrorMessages;

//##ModelId=48522CD3003E
//## ASN变长类型记录处理对象. 通用类,继承自CdrExtractor抽象类
class AsnCdrExtractor : public CdrExtractor
{
  public:
    //##ModelId=48522D430138
    virtual ~AsnCdrExtractor();

    //##ModelId=48522D5B03D8
    //## 该方法先调用ASN_1对象中的方法,求出记录中的全部元素,再根据字段定义得到字段值
    virtual int getNextSrcCdr(unsigned char *&srcCdrBuffer);

    //##ModelId=48522D7001F4
    //## 初始化
    virtual bool initialize(const string &theCdrParameter, const int theBlockHeaderSize);

  private:
    //##ModelId=48522D7D00CB
    string m_Tag;

};



#endif /* ASNCDREXTRACTOR_H_HEADER_INCLUDED_B7A61B63 */
