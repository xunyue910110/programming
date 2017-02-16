// ##########################################
// Source file : FieldExtractRule.h
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

#ifndef FIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A62782
#define FIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A62782

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#else
#include <iostream>
#endif

#include <vector>
#include <string>
#include <map>

#include "FieldInfo.h"
#include "RecordFormat.h"
#include "config-all.h"
#include "DataFormat.h"
#include "appfrm/ErrorHandle.h"
#include "PpConstDef.h"

class MyString;
class FieldInfos;
class RepeatableFieldInfo;
class PpLog;

USING_NAMESPACE(std);

extern ErrorMessages * const theErrorMessages;

//##ModelId=48520CFD0157
//##Documentation
//## Composite(组合)模型的抽象字段类
class FieldExtractRule
{
  public:
    //##ModelId=48520D220222
    FieldExtractRule(int iSubNumber);

    //##ModelId=48520D250128
    virtual ~FieldExtractRule();

    //##ModelId=48520D44005D
    //##Documentation
	//## 用于把原始字段值转换为ASCII值
    void getFmtValue();

    //##ModelId=48520D510109
    //##Documentation
	//## 对象里的校验方法由PPParameter根据校验参数进行初始化.此方法使用这些参数对本字段对象里的值进行校验
    void checkField();

    //##ModelId=48520D640196
    virtual bool initialize(vector<string> *argname
    							, FieldExtractRule *aParentsField
    							, const int aFloor
    							, const bool isRepeatable=false
    							, RepeatableFieldInfo *pCompsiteFieldInfo=NULL) = 0;

    //##ModelId=48520D9F03C8
    virtual bool extract(const unsigned char *buffer, int length
    						, RecordFormat &aRecordFormat) = 0;

    //##ModelId=48520DEF0280
    string getFieldName() const;

    //##ModelId=4852127E01B5
    string getSectionName() const;

    //##ModelId=48520EB9038A
    int getLaye() const;

    //##ModelId=48520FED008C
    void setSectionName(const string &left);

    //##ModelId=4852108D037A
    FieldExtractRule *getMySelf() const;

  public:
    //##ModelId=485210ED03A9
    //##Documentation
	//## 保存指向父字段的指针
    FieldExtractRule *m_theParentsField_;

    //##ModelId=4852110D03D8
    //##Documentation
	//## 对象树的层数
    int m_theLayer_;

    //##ModelId=485211170290
    //##Documentation
	//## 配置文件中用于生成字段对象的标识名称
    string m_SectionName_;

    //##ModelId=4852112100BB
    //##Documentation
	//## 字段数据类型
    string m_dataType;

    //##ModelId=4852112B036B
    //##Documentation
	//## 字段分隔符
    char m_token;

  protected:
    void getFieldInfoByName(const bool isRepeatable,FieldInfo *pCompsiteFieldInfo);
    void parseConfigRule(const string &argStr,vector<string> &strResults);
    void getDataTypeRule(const string &strDataTypeRule);
		
  protected:
    //##ModelId=48521140033C
    PpLog *m_PpLog_;

    //##ModelId=485211A2030D
    //##Documentation
	//## 字段名称(标识)
    string m_FieldName_;

    //##ModelId=485211AD0399
    //##Documentation
	//## BCD码填充字符
    char m_padding;

    //##ModelId=485211B601B5
    //##Documentation
	//## 二进制数据高低位交换标志
	//## =0 不交换; =1 交换
    int m_order;

    //##ModelId=485211C1038A
    FieldInfo *m_pFieldInfo;

    //##ModelId=485211D20222
    //##Documentation
	//## 指向composite自身处理对象的指针
    FieldExtractRule *m_CompositeFieldSelf_;
    
    FieldInfos *m_pFieldInfos;
    int m_iSubChannelNum;
    
    bool m_isRepeatable;
};



#endif /* FIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A62782 */
