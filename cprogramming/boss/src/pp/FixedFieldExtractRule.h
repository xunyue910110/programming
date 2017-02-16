// ##########################################
// Source file : FixedFieldExtractRule.h
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

#ifndef FIXEDFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A616F3
#define FIXEDFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A616F3

#include "FieldExtractRule.h"
#include "RecordFormat.h"
#include "config-all.h"

#include <vector>
#include <string>

USING_NAMESPACE(std);

//##ModelId=4852156C038A
//##Documentation
//## 定长字段提取方法类,继承自FieldExtractRule抽象对象.该对象用于从原始记录中提取字段
class FixedFieldExtractRule : public FieldExtractRule
{
  public:
    FixedFieldExtractRule(int iSubNumber):FieldExtractRule(iSubNumber){}	
    //##ModelId=485217F101D4
    virtual bool initialize(vector<string> *argname
    							, FieldExtractRule *aParentsField
    							, const int aFloor
    							, const bool isRepeatable=false
    							, RepeatableFieldInfo *pCompsiteFieldInfo=NULL);

    //##ModelId=48521800033C
    virtual bool extract(const unsigned char *buffer, int length, RecordFormat &aRecordFormat);

    //##ModelId=48521867003E
    //##Documentation
    //## 字段开始字节
    int m_StartByte_;

    //##ModelId=4852187102AF
    //##Documentation
    //## 字段开始比特
    int m_StartBit_;

    //##ModelId=4852187A03B9
    //##Documentation
    //## 字段长度(单位:比特)
    int m_BitLength_;

  private:
    //##ModelId=4852187A03B9
    //##Documentation
    //## 记录nortel交换机附加模块在话单中出现的位置
    int m_position;

};



#endif /* FIXEDFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A616F3 */
