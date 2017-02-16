// ##########################################
// Source file : AsnFieldExtractRule.h
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

#ifndef ASNFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A60C38
#define ASNFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A60C38

#include "FieldExtractRule.h"
#include "RecordFormat.h"
#include "AsnFormat.h"
#include "config-all.h"
#include <vector>
#include <string>

USING_NAMESPACE(std);

class FieldInfo;
class MyString;

//##ModelId=485212D900FA
//##Documentation
//## ASN变长字段提取方法类,继承自FieldExtractRule抽象对象.该对象用于从原始记录中提取字段
class AsnFieldExtractRule : public FieldExtractRule
{
  public:
    //##ModelId=485212EE01B5
    //##Documentation
    //## 初始化
    AsnFieldExtractRule(int iSubNumber):FieldExtractRule(iSubNumber){}
	
    virtual bool initialize(vector<string> *argname
    							, FieldExtractRule *aParentsField
    							, const int aFloor
    							, const bool isRepeatable=false
    							, RepeatableFieldInfo *pCompsiteFieldInfo=NULL);

    //##ModelId=4852131E03C8
    //##Documentation
    //## 解析
    virtual bool extract(const unsigned char *buffer
    						, int length
    						, RecordFormat &aRecordFormat);

  protected:
  	void strTag2Binary();
  	
  protected:
    //##ModelId=4852135E01B5
    //##Documentation
    //## 保存ASN的Tag码
    string m_Tag_;

    //##ModelId=4852136B0196
    unsigned char m_tag[10];

    //##ModelId=4852137A0280
    char m_ss[16];

    //##ModelId=4852138400EA
    int m_position;

};



#endif /* ASNFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A60C38 */
