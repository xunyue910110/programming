// ##########################################
// Source file : FieldValidateRule_5.h
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080924
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDVALIDATERULE_5_H_HEADER_INCLUDED_B7A62C02
#define FIELDVALIDATERULE_5_H_HEADER_INCLUDED_B7A62C02

#include "FieldValidateRule.h"
//#include "FieldInfos.h"

//##########################################
//##此校验规则用于统计交换机所下话单的时效性
//##########################################

//##ModelId=4856464A0148
class FieldValidateRule_5 : public FieldValidateRule
{
  public:
   	FieldValidateRule_5(const int iSubChannelNum):FieldValidateRule(iSubChannelNum){};
    //##ModelId=4856465D008C
    virtual bool initialize(string ruleText);

    //##ModelId=48564664000F
    virtual int validField();

  private:
    //##ModelId=4856466F001F
    map<string,int> m_FileNoMap;

    //##ModelId=4856467B00CB
    string m_checkDate;

    //##ModelId=485646830280
    string m_checkTime;

    //##ModelId=485646B00186
    int m_iRange1;

    //##ModelId=485646B303D8
    int m_iRange2;

    //##ModelId=485646B80000
    int m_iRange3;

    //##ModelId=485646BB0186
    int m_iRange4;

    //##ModelId=485646BE03D8
    int m_iRange5;

    //##ModelId=485646C3004E
    int m_iRange6;

};



#endif /* FIELDVALIDATERULE_5_H_HEADER_INCLUDED_B7A62C02 */
