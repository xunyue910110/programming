// ##########################################
// Source file : AsnFormat.h
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

#ifndef ASNFORMAT_H_HEADER_INCLUDED_B7A65B25
#define ASNFORMAT_H_HEADER_INCLUDED_B7A65B25

#include "RecordFormat.h"

//##ModelId=4855BFD7004E
//##Documentation
//## 为纯方法类,通用类  包含ASN.1协议的解包算法
class AsnFormat : public RecordFormat
{
  public:
    //##ModelId=4855C00403A9
    AsnFormat();

    //##ModelId=4855C0070138
    virtual ~AsnFormat();

    //##ModelId=4855C01D030D
    //##Documentation
    //## 使用ASN_1协议,对给定字符串求出全部的TAG和contents,通过一个map容器(使用TAG为键值)指针返回结果
    //## 本方法会解开全部的符合元素.返回元素个数.暂时不用
    virtual void getContents();

    //##ModelId=4855C03801A5
    //##Documentation
    //## 使用ASN_1协议,根据给定的记录TAG标识,求出该记录的实际数据(存放在参数内)
    //## 返回记录长度
    virtual int getCdrSize(const char *theSign, const char *cdrTmp, int &aBlockOffset);

    //##ModelId=4855C0660138
    //##Documentation
    //## 返回: 字段值的长度
    //针对只出现tag码,且valueSize为0的情况增加参数tagALL, by zaiww 20090925 
    virtual int getValue(unsigned char * theSign, FieldInfo *pFieldInfo, int position, int theLength, bool &tagAll);

    //##ModelId=4855C0A6032C
    //##Documentation
    //## 初始化
    virtual void initialize();

    //##ModelId=4855C0C001F4
    //##Documentation
    //## 返回: ASN记录长度的字段数量
    int getAsnLength(const unsigned char *lengthBuffer, int &aAsnCdrSize);

    //##ModelId=4855C0DF031C
    //##Documentation
    //## 返回: tag码长度
    int getAsnTag(const unsigned char *buffer, unsigned char *aTag);

  private:
    //##ModelId=4855C106000F
    //##Documentation
    //## 对于ASN.1标准的不定长形式,求取结束标识为两个值为0组成的字节的话单长度
    int getZeroLen(const unsigned char * buffer);

    //##ModelId=4855C11A007D
    //##Documentation
    //## 是否为符合字段
    bool isCompositeTag(int tag);

};



#endif /* ASNFORMAT_H_HEADER_INCLUDED_B7A65B25 */
