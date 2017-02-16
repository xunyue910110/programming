// ##########################################
// Source file : RecordFormat.h
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

#ifndef RECORDFORMAT_H_HEADER_INCLUDED_B7A67DF4
#define RECORDFORMAT_H_HEADER_INCLUDED_B7A67DF4

#include "AlertCode.h"
#include "PpLog.h"
#include "config-all.h"
#include "appfrm/ErrorHandle.h"
#include <string>
#include <vector>

extern ErrorMessages * const theErrorMessages;

class FieldInfo;

USING_NAMESPACE(std);

//##ModelId=4851C39F02EE
//##Documentation
//## 记录格式处理类,抽象公共方法类
//## 此类用于对原始话单记录进行解包运算,得到记录内所有的字段标识和字段值
//## 用字段标识作为键值把字段值存入一个容器内,供进一步处理
class RecordFormat
{
  public:
    //##ModelId=4851C3AD03C8
    //##Documentation
    //## 对象类型(=0:基类对象; =1:AsnFormat对象; =2:TokenFormat对象)
    RecordFormat();

    //##ModelId=4851C3B400EA
    virtual ~RecordFormat();

    //##ModelId=4851C3D100DA
    //##Documentation
    //## 得到记录全部字段的键值和值
    virtual void getContents() = 0;

    //##ModelId=4851C3ED02BF
    //##Documentation
    //## 得到一条记录的长度
    virtual int getCdrSize(const char *theSign, const char *cdrTmp, int &aCdrOffset) = 0;

    //##ModelId=4851C436007D
    //##Documentation
    //## 得到字段值的长度
    //针对只出现tag码,且valueSize为0的情况增加参数tagALL, by zaiww 20090925
    virtual int getValue(unsigned char * theSign, FieldInfo *pFieldInfo, int position, int theLength, bool &tagAll) = 0;

    //##ModelId=4851C49600DA
    //##Documentation
    //## 初始化
    virtual void initialize() = 0;

    //##ModelId=4851C4650157
    void reset(const unsigned char *aCdrBuffer, int cdrSize, int cdrOffset, int flag);

    //##ModelId=4851C4F1006D
    int getFormatType() const;

  public:
    //##ModelId=4851C51C02CE
    bool isUsed;

    //##ModelId=4851C527037A
    int m_subOffset;

    //##ModelId=4851C53301E4
    int m_bufferOffset;

    //##ModelId=4851C53E008C
    int m_cdrSize;

  protected:
    //##ModelId=4851C54C03C8
    RecordFormat *getNextPointer();

  protected:
    //##ModelId=4851C5D301A5
    static vector<RecordFormat *> m_RecordFormatPointer;

    //##ModelId=4851C61900AB
    const unsigned char *m_CdrBuffer;

    //##ModelId=4851C6A30196
    //##Documentation
    //## 对象类型(=0:基类对象; =1:AsnFormat对象; =2:TokenFormat对象)
    int m_FormatType;

    //##ModelId=4851C6B402BF
    PpLog *m_PpLog_;

};



#endif /* RECORDFORMAT_H_HEADER_INCLUDED_B7A67DF4 */
