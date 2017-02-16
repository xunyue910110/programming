// ##########################################
// Source file : FieldConvertRule_8.h
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080926
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#ifndef FIELDCONVERTRULE_8_H_HEADER_INCLUDED_B7A631B2
#define FIELDCONVERTRULE_8_H_HEADER_INCLUDED_B7A631B2

#include "FieldConvertRule.h"
#include "FieldInfo.h"
class FieldInfos;
class RecordFormat;

//##ModelId=485618750203

/*此规则用于根据配置中的字符，分割已有字段中的值，并按照配置中的顺序提取相应的值*/
class FieldConvertRule_8 : public FieldConvertRule
{
  public:
  	FieldConvertRule_8(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
    //##ModelId=485618870109
    virtual bool initialize(string ruleText);

    //##ModelId=4856188D0232
    virtual bool convertField();

  protected:
  	virtual void getFieldObj();

  private:
    //##ModelId=485618B1036B
    char m_myToken[10];               //分隔符

    //##ModelId=485618BB0128
    string m_compositeFieldName;       //复合字段名

    //##ModelId=485618C402AF
    int m_postion;                   //子字段顺序号

    //##ModelId=485618CC005D
    string m_dstFieldName;           //子字段名

    FieldInfo *m_pCompField;
    FieldInfo *m_pDesField;

};



#endif /* FIELDCONVERTRULE_8_H_HEADER_INCLUDED_B7A631B2 */
