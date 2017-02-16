// ##########################################
// Source file : FieldValidateRule_1.h
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080922
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDVALIDATERULE_1_H_HEADER_INCLUDED_B7A6481B
#define FIELDVALIDATERULE_1_H_HEADER_INCLUDED_B7A6481B

#include "FieldValidateRule.h"
#include "FieldInfos.h"


//##ModelId=4856452000EA
class FieldValidateRule_1 : public FieldValidateRule
{
  public:
  	FieldValidateRule_1(const int iSubChannelNum):FieldValidateRule(iSubChannelNum){};
    //##ModelId=48564530036B
    virtual bool initialize(string ruleText);

    //##ModelId=485645370177
    virtual int validField();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=485645400251
    //## 校验类型。类型如下：
	//## 1、字段长度校验
	//## 2、字段值校验
    string m_valudType;

    //##ModelId=48564554009C
    //## 允许最大值
    int m_max;

    //##ModelId=485645570242
    //## 允许最小值
    int m_min;
    
    FieldInfo *m_pFieldTarget;
};



#endif /* FIELDVALIDATERULE_1_H_HEADER_INCLUDED_B7A6481B */
