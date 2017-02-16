// ##########################################
// Source file : FieldValidateRule_2.h
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

#ifndef FIELDVALIDATERULE_2_H_HEADER_INCLUDED_B7A675F1
#define FIELDVALIDATERULE_2_H_HEADER_INCLUDED_B7A675F1


#include "FieldValidateRule.h"


//##ModelId=485645760290
class FieldValidateRule_2 : public FieldValidateRule
{
  public:
  		FieldValidateRule_2(const int iSubChannelNum):FieldValidateRule(iSubChannelNum){};
    //##ModelId=4856458502CE
    virtual bool initialize(string ruleText);

    //##ModelId=4856458E00DA
    
    //virtual int validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);
    virtual int validField();

  private:
    //##ModelId=48564597000F
   string m_validValue;

};



#endif /* FIELDVALIDATERULE_2_H_HEADER_INCLUDED_B7A675F1 */
