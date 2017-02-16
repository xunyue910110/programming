// ##########################################
// Source file : CdrExtractor.h
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

#ifndef CDREXTRACTOR_H_HEADER_INCLUDED_B7A67EAD
#define CDREXTRACTOR_H_HEADER_INCLUDED_B7A67EAD

#include "RecordFormat.h"
#include "config-all.h"
#include "FieldExtractRule.h"
#include "AlertCode.h"
#include "appfrm/ErrorHandle.h"
#include <string>

extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std);

class PpLog;

//##ModelId=48522A410242
//##Documentation
//## 通用话单记录处理抽象类
class CdrExtractor
{
  public:
    //##ModelId=48522A4600DA
    CdrExtractor();

    //##ModelId=48522A49004E
    virtual ~CdrExtractor();

    //##ModelId=48522A650399
    //##Documentation
    //## 该方法是纯虚方法,由其子类具体实现
    virtual int getNextSrcCdr(unsigned char *&srcCdrBuffer) = 0;

    //##ModelId=48522A8600AB
    virtual bool setBlockBuffer(const unsigned char *BlockBuffer, int RealBlockLength);

    //##ModelId=48522AC7000F
    //##Documentation
    //## 用于初始化的方法
    virtual bool initialize(const string &theCdrParameter, const int theBlockHeaderSize) = 0;

    //##ModelId=48522B100213
    virtual void setToken(const string token){}

  protected:
    //##ModelId=48522B350232
    //##Documentation
    //## 用于保存实际记录块的长度
    int m_RealBlockLength;

    //##ModelId=48522B42031C
    //##Documentation
    //## 用于保存话单记录块的缓存指针
    unsigned char *m_BlockBuffer;

    //##ModelId=48522B54034B
    int m_BlockHeaderSize;

    //##ModelId=48522B5E02CE
    RecordFormat *theRecordFormat;

    //##ModelId=48522B6B038A
    int m_BlockOffset;

    //##ModelId=48522B78000F
    PpLog *m_PpLog_;
    
    int m_iCdrBufferSize;
};



#endif /* CDREXTRACTOR_H_HEADER_INCLUDED_B7A67EAD */
