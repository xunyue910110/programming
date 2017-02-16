// ##########################################
// Source file : FieldConvertRule_2.h
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080925
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDCONVERTRULE_2_H_HEADER_INCLUDED_B7A63526
#define FIELDCONVERTRULE_2_H_HEADER_INCLUDED_B7A63526

#include "FieldConvertRule.h"
class FieldInfos;


//##ModelId=485615490203
//##对以下情况进行转换
//##已知module的值，对各种字段进行赋值，如果这些字段不存在，则创建字段
class FieldConvertRule_2 : public FieldConvertRule
{
  public:
  	FieldConvertRule_2(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=4856155D02FD
    virtual bool initialize(string ruleText);

    //##ModelId=48561568038A
    virtual bool convertField();

    //##ModelId=48561574005D
    virtual ~FieldConvertRule_2() {};

  protected:
  	virtual void getFieldObj();
  	bool condJudge();

  private:
    //##ModelId=48561581003E
    bool getField(const string &value);

    //##ModelId=4856159F02EE
    vector<string> m_conditionFieldName;
    

    //##ModelId=485615B10177
    vector<string> m_conditionFieldValue;
    

    //##ModelId=485615BA0157
    vector<string> m_conditionExp;

    //##ModelId=485615C6001F
    vector<string> m_dstFieldNames;

    //##ModelId=485615D40290
    vector<string> m_dstFieldValues;

    //##ModelId=485615E2000F
    vector<string> m_dstExps;
    
    vector<FieldInfo *> m_pFieldConditions; 
    vector<FieldInfo *> m_pFieldTargets; 
    
    vector<int>         m_iCondExpRightKeySeqs; // 条件表达式右键字段下标
    vector<int>         m_iOperExpRightKeySeqs; // 赋值表达式右键字段下标

};



#endif /* FIELDCONVERTRULE_2_H_HEADER_INCLUDED_B7A63526 */
