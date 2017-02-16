// ##########################################
// Source file : FixedCdrExtractor.h
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

#ifndef FIXEDCDREXTRACTOR_H_HEADER_INCLUDED_B7A631F7
#define FIXEDCDREXTRACTOR_H_HEADER_INCLUDED_B7A631F7

#include "CdrExtractor.h"
#include "DataFormat.h"
#include "config-all.h"
#include "appfrm/ErrorHandle.h"
#include "FieldInfo.h"

extern ErrorMessages * const theErrorMessages;

USING_NAMESPACE(std);

//##ModelId=48522DA90148
//##Documentation
//## 定长记录处理对象.通用类,继承自CdrExtractor抽象类
class FixedCdrExtractor : public CdrExtractor
{
  public:
    //##ModelId=48522DC70000
    //##Documentation
	//## 顺序调用所有字段中的GetSrcValue方法,得到全部字段的原始数据
    virtual int getNextSrcCdr(unsigned char *&srcCdrBuffer);

    //##ModelId=48522DCE003E
    //##Documentation
    //CDMA nortel话单文件多条定长cdr的特殊性
    virtual bool initialize(const string &theCdrParameter, const int theBlockHeaderSize);

  private:
    //##ModelId=48522DD9034B
    int getcdrSize(const unsigned char *srcCdrBuffer);

    //##ModelId=48522DF6005D
    vector<string> m_CdrSize;

    //##ModelId=48522E010290
    int m_StartByte;

    //##ModelId=48522E0A02EE
    int m_Length;

    //##ModelId=48522E140251
    string m_dataType;

    //##ModelId=48522E2701E4
    char m_padding;

    //##ModelId=48522E3000CB
    int m_order;

    //##ModelId=48522E3A01F4
    DataFormat m_DataFormat;
    
    FieldInfo m_fielInfo;

};



#endif /* FIXEDCDREXTRACTOR_H_HEADER_INCLUDED_B7A631F7 */
