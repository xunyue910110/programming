// ##########################################
// Source file : FieldInfo.h
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

#ifndef FIELDINFO_H_HEADER_INCLUDED_B7A62C5C
#define FIELDINFO_H_HEADER_INCLUDED_B7A62C5C

#include "config-all.h"
#include <string>

USING_NAMESPACE(std);

class MyString;

//##ModelId=4850CE02031C
//##Documentation
//## 处理字段保存类
class FieldInfo
{
  public:
    //##ModelId=4850CE560157
    FieldInfo();
    FieldInfo(const FieldInfo &fieldInfo);

    //##ModelId=4850CE670251
    virtual ~FieldInfo();
    
    virtual void reset();
    virtual int getNumCount(){return 0;}
    
    virtual void setDesValue(string &value);
    
    virtual void setDesValue(const char *value,int iStrLength=-1);
    
    virtual void setSrcValue(const unsigned char *value,const int iLength);
    
    virtual void reserveDesStr(int &iLength);
  public:
    //--------------以下为话单字段原始值、标准数据类型值的属性----------------
    //##ModelId=4850CE84038A
    //##Documentation
    //## 经数据格式化后的字段值
    string m_FieldName_;
    char *m_FieldDesValue;

    //##ModelId=4850CE9701E4
    //##Documentation
    //## 字段原始值
    unsigned char *m_FieldSrcValue;

    //##ModelId=4850CEA501B5
    //##Documentation
    //## 字段原始值长度
    int m_iSrcValueLen;
    int m_iDesValueLen;

    //---------------------以下为字段处理结果的属性-----------------------------
    //##ModelId=3B9EC4D201B5
    //##Documentation
    //## 字段数据标准化标志
    bool m_IsFormat;

    //##ModelId=3B9EC4D40064
    //##Documentation
    //## 字段校验标志
    bool m_IsCheck;

    //##Documentation
    //## 错误代码
    //##ModelId=4850CEB4006D
    int m_errorCode;
    int m_FieldLength  ;      //added by zhonghai 2011/2/15 10:13:01

    //---------------------以下为数据类型转换使用的属性-----------------------------
    //##ModelId=3B9EC4D400E6
    //##Documentation
    //## 字段数据类型,包括以下几种：
    //## EBCD,PBCD,TBCD,UBCD,Binary,HEX,Ascii
    //##ModelId=4850CEBE0196
    string m_dataType;

    //##ModelId=4850CEC70261
    //##Documentation
    //## 填充字符(如：'F'),由BCD数据格式处理使用
    char m_padding;

    //##ModelId=4850CECF0232
    //##Documentation
    //## 二进制数据高低位交换标志(＝0,不交换;＝1,交换)
    int m_order;
    
    //针对只出现tag码,且valueSize为0的情况 by zaiww 20090925
	bool m_tagAll;
	
	int m_iMaxSrcValueLen;
	int m_iMaxDesValueLen;

};



#endif /* FIELDINFO_H_HEADER_INCLUDED_B7A62C5C */
