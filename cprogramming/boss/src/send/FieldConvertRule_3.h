// ##########################################
// Source file : FieldConvertRule_3.h
// System      : Mobile Billing System
// Version     : 1.05
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20011107
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef CONVERTRULE_3_H_INCLUDED_C45579AE
#define CONVERTRULE_3_H_INCLUDED_C45579AE

#include "FieldConvertRule.h"


//##ModelId=3BAAC04A034E
//##Documentation
//## 此转换程序用于去除电话号码中的国际长途区号前缀"86"或"0086"(具体前缀由配置文件读取)。
//## 转换规则为：号码长度大于11，并且前缀为"86"或"086"的，去除前缀。
class FieldConvertRule_3 : public FieldConvertRule
{
  public:
	//##ModelId=3BAAEAC2029D
	virtual bool initialize(string ruleText);

	//##ModelId=3BAAEB020321
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps);

  private:
	//##ModelId=3C2D434F014B
   int mvZero(string src,char *des);

  private:
	//##ModelId=3BE7409002E8
	string m_phoneFlag;

	//##ModelId=3BE745930365
	string m_phone;

	//##ModelId=3C2D7A1703A9
	int m_order;

};

#endif /* CONVERTRULE_3_H_INCLUDED_C45579AE */
