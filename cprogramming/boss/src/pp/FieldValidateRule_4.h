// ##########################################
// Source file : FieldValidateRule_4.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080922
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDVALIDATERULE_4_H_HEADER_INCLUDED_B7A62CA6
#define FIELDVALIDATERULE_4_H_HEADER_INCLUDED_B7A62CA6

#include <map>
#include "FieldValidateRule.h"
//#include "FieldInfos.h"


//##ModelId=485645DD01E4
// 此校验方法专用于根据交换机号区分相同目录下的原始文件，进行文件连续性校验。
class FieldValidateRule_4 : public FieldValidateRule
{
  public:
   	FieldValidateRule_4(const int iSubChannelNum):FieldValidateRule(iSubChannelNum){};
    //##ModelId=485645EB01B5
    virtual bool initialize(string ruleText);

    //##ModelId=485645F30167
    virtual int validField();

  private:
    //##ModelId=485645FD0222
    map<string,int> m_FileNoMap;

    //##ModelId=4856460A0280
    int m_iSubSwitchStart;

    //##ModelId=48564612009C
    int m_iSubSwitchLenth;

    //##ModelId=485646190271
    int m_iSubFileStart;

    //##ModelId=4856462003C8
    int m_iSubFileLenth;

};



#endif /* FIELDVALIDATERULE_4_H_HEADER_INCLUDED_B7A62CA6 */
