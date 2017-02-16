// ##########################################
// Source file : FieldConvertRule_3.h
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

#ifndef FIELDCONVERTRULE_3_H_HEADER_INCLUDED_B7A62C7C
#define FIELDCONVERTRULE_3_H_HEADER_INCLUDED_B7A62C7C

#include "FieldConvertRule.h"

class FieldInfos;


//##ModelId=4856162700EA
//##此转换程序用于去除电话号码中的国际长途区号前缀"86"或"0086"(具体前缀由配置文件读取)
//##转换规则为：号码长度大于11，并且前缀为"86"或"0086"的，去除前缀。
class FieldConvertRule_3 : public FieldConvertRule
{
  public:
  	FieldConvertRule_3(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=4856163A02BF
    virtual bool initialize(string ruleText);

    //##ModelId=4856164200BB
    virtual bool convertField();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=4856164C0148
    int mvZero(string src, char *des);

    //##ModelId=4856166702DE
    string m_phoneFlag;

    //##ModelId=48561671002E
    string m_phone;

    //##ModelId=48561679002E
    int m_order;
    
    FieldInfo *m_pFieldPhoneFlag;
    FieldInfo *m_pFieldPhone;

};



#endif /* FIELDCONVERTRULE_3_H_HEADER_INCLUDED_B7A62C7C */
