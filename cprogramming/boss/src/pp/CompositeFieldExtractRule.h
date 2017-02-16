// ##########################################
// Source file : CompositeFieldExtractRule.h
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

#ifndef COMPOSITEFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A64125
#define COMPOSITEFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A64125

#include "config-all.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <vector>
#include <string>
#include <stdio.h>

#include "FieldExtractRule.h"
#include "RecordFormat.h"
#include "FieldInfo.h"
#include "AsnFieldExtractRule.h"
#include "SasnFieldExtractRule.h"
#include "TokenFieldExtractRule.h"
#include "FixedFieldExtractRule.h"
#include "GasnFieldExtractRule.h"
#include "RepeatableFieldInfo.h"

USING_NAMESPACE(std);

class MyString;

//##ModelId=4851CB500399
//##Documentation
//## 基于Composite(组合)模型的字段类,用于生成字段对象
//## (任意层数,任意分支. 实际数目和层数完全由配置文件指定,本模型对此没有限制)
class CompositeFieldExtractRule : public FieldExtractRule
{
  public:
    //##ModelId=4851CB6502FD
    CompositeFieldExtractRule(int iSubNumber);

    //##ModelId=4851CB6C004E
    virtual ~CompositeFieldExtractRule();

    //##ModelId=4851CB8A0399
    //##Documentation
    //## 初始化CompositeFieldExtractRule模型
    virtual bool initialize(vector<string> *argname
    							, FieldExtractRule *aParentsField
    							, const int aFloor
    							, const bool isRepeatable=false
    							, RepeatableFieldInfo *pCompsiteFieldInfo=NULL);

    //##ModelId=4851CBBB01D4
    virtual bool extract(const unsigned char *buffer, int length
    						, RecordFormat &aRecordFormat);

    //##ModelId=4851CC3301A5
    bool setToken(string token);

    //##ModelId=4851CC5A02FD
    //##Documentation
    //## 子字段(field or compositeFieldExtractRule)对象指针保存容器
    vector<FieldExtractRule *> m_theChildField_;

  private:
    //##ModelId=4851CC7B0109
    void getCdrSign(const unsigned char *srcCdrValue, string &theSign, RecordFormat &aRecordFormat);

    //##ModelId=4851CCC90167
    string getFieldType(string srcString, string &desString);

    //##ModelId=4851CCF4004E
    FieldExtractRule *createChildFieldObject(FieldExtractRule *& aChildField
												, const string aFieldType
												, bool &isRepeatableLocal);

    //##ModelId=4851CD2F006D
    void getSectionName(string &aSectionName);

    //##ModelId=4851CD7C005D
    char m_myToken;
    
    RepeatableFieldInfo *m_pMySelfFieldInfo;  // composite字段的信息 不输出

};



#endif /* COMPOSITEFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A64125 */
