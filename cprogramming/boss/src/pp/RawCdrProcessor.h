// ##########################################
// Source file : RawCdrProcessor.h
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

#ifndef RAWCDRPROCESSOR_H_HEADER_INCLUDED_B7A67FCC
#define RAWCDRPROCESSOR_H_HEADER_INCLUDED_B7A67FCC

#include "DataFormat.h"
#include "DateTime.h"
#include "FieldConvertors.h"
#include "FieldValidators.h"
#include "base/supdebug.h"
#include "config-all.h"
#include "appfrm/ErrorHandle.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#else
#include <iostream>
#endif

#include <string>
#include <vector>

extern  ErrorMessages  *const theErrorMessages;

USING_NAMESPACE(std);

class CompositeFieldExtractRule;
class FieldExtractRule;
class PpLog;
class RecordFormat;
class CdrFlag;
class FieldInfos;

//##ModelId=4850D57B00BB
//##Documentation
//## 话单记录处理抽象类
//## 此类用于处理话单原始记录,处理结果为已经格式化的标准话单数据
//## 该类支持固定格式、ASN变长格式、分隔符格式三种话单记录
class RawCdrProcessor
{
  public:
    //##ModelId=4850FB96029F
    RawCdrProcessor(const int iSubChannelNum);

    //##ModelId=4850FB990148
    virtual ~RawCdrProcessor();

    //##ModelId=4850FBB00280
    //##Documentation
    //## 按顺序调用所有字段对象里的校验方法
    bool validCdr();

    //##ModelId=4850FBCD002E
    //##Documentation
    //## 按顺序调用所有字段对象里的转换方法
    bool convertCdr();

    //##ModelId=4850FBEC034B
    //##Documentation
    //## 按顺序调用所有字段对象里的相应方法,求得字段的标准格式数据
    virtual int getDesCdr(const unsigned char *aCdrValue, int length) = 0;

    //##ModelId=4850FC24029F
    //##Documentation
    //## 字段处理初始化方法,由主控对象(PreProcApplication)调用
    virtual bool initialize(vector<string> aParameterList);

    //##ModelId=4850FC39002E
    //##Documentation
    //## 规则设置,包括转换规则与校验规则的设置
    bool setRule(vector<string> aConvertRule, vector<string> aValidRule);

    //##ModelId=4850FC600222
    //##Documentation
    //## 字段值格式化
    bool formatField();

    //##ModelId=4850FC7500CB
    //##Documentation
    //## 获取下一个复合字段
    FieldExtractRule *getNextCompositeField(string &theCdrSign);

    //##ModelId=4850FC9201D4
    //##Documentation
    //## 设置分隔符
    bool setToken(string token);

  public:
    //##ModelId=4850FCC5029F
    RecordFormat *m_theRecordFormat;

    //##ModelId=4850FCD3030D
    CompositeFieldExtractRule *m_pCompositeField;

    //##ModelId=4850FCE20196
    DataFormat m_DataFormat;

    //##ModelId=4850FCEF005D
    FieldValidators m_FieldValidators;

    //##ModelId=4850FCFE0290
    FieldConvertors m_FieldConvertors;

  protected:
    void convertDataType(FieldInfo *pFieldInfo);

  protected:
    //##ModelId=4850FD0F0242
    //##Documentation
    //## 话单记录原始数据
    const unsigned char * m_SrcCdrValue;

    //##ModelId=4850FD1E02DE
    PpLog *m_PpLog_;

    //##ModelId=4850FD2D01F4
    CdrFlag *m_cdrFlag;

    int m_iSubChannelNum;
    
    FieldInfos *m_pFieldInfos;

  protected:
    //##ModelId=4850FD610271
    //##Documentation
    //## 日期时间类型转换
    bool convertDateTime();

	FieldInfo *m_pFieldStart_ye     ;
	FieldInfo *m_pFieldStart_mo     ;
	FieldInfo *m_pFieldStart_dy     ;
	FieldInfo *m_pFieldStart_ho     ;
	FieldInfo *m_pFieldStart_mi     ;
	FieldInfo *m_pFieldStart_se     ;
	FieldInfo *m_pFieldStart_ntime  ;
	FieldInfo *m_pFieldEnd_ye       ;
	FieldInfo *m_pFieldEnd_mo       ;
	FieldInfo *m_pFieldEnd_dy       ;
	FieldInfo *m_pFieldEnd_ho       ;
	FieldInfo *m_pFieldEnd_mi       ;
	FieldInfo *m_pFieldEnd_se       ;
	FieldInfo *m_pFieldDu           ;
	FieldInfo *m_pFieldDu_cy        ;
    FieldInfo *m_pFieldDu_rd        ;
	FieldInfo *m_pFieldDu_ncy       ;
	FieldInfo *m_pFieldDu_3ncy      ;

	FieldInfo *m_pFieldStartDate    ;
    FieldInfo *m_pFieldStartTime    ;
	FieldInfo *m_pFieldEndDate      ;
	FieldInfo *m_pFieldEndTime      ;
	
	FieldInfo *m_pFieldModule_Name  ;	

};



#endif /* RAWCDRPROCESSOR_H_HEADER_INCLUDED_B7A67FCC */
