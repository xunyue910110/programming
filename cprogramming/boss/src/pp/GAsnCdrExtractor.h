// ##########################################
// Source file : GAsnCdrExtractor.h
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

#ifndef GASNCDREXTRACTOR_H_HEADER_INCLUDED_B7A64CC7
#define GASNCDREXTRACTOR_H_HEADER_INCLUDED_B7A64CC7

#include "config-all.h"
#include "CdrExtractor.h"
#include "AsnFormat.h"
#include "appfrm/ErrorHandle.h"
#include <string>

extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std);

//##ModelId=48522E67038A
//##Documentation
//## 与 AsnCdrExtractor 不同的是,该类处理每条cdr以不同的tag作为开始标志的ans.1格式的原始话单
//## ASN变长类型记录处理对象。通用类，继承自GeneralCdr抽象类
class GAsnCdrExtractor : public CdrExtractor
{
  public:
    //##ModelId=48522E7D005D
    virtual ~GAsnCdrExtractor();

    //##ModelId=48522EB3006D
    //##Documentation
    //## 该方法先调用ASN_1对象中的方法,求出记录中的全部元素,再根据字段定义得到字段值
    virtual int getNextSrcCdr(unsigned char *&srcCdrBuffer);

    //##ModelId=48522EBD00BB
    virtual bool initialize(const string &theCdrParameter, const int theBlockHeaderSize);

  private:
    //##ModelId=48522ECB0271
    vector<int> m_nTags;

    //##ModelId=48522EDE006D
    vector<string> m_Tags;

    //##ModelId=48522EEC00CB
    bool m_isHuawGprs;

};



#endif /* GASNCDREXTRACTOR_H_HEADER_INCLUDED_B7A64CC7 */
