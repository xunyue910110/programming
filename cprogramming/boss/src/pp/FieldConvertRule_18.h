// ##########################################
// Source file : FieldConvertRule_18.cpp
// System      : Mobile Billing System
// Version     : 0.1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Tao Li
// E_mail      : litao@lianchuang.com
// mender      : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20070803
// Update      : 20080926
// Copyright(C): 2002 by Tao Li, Linkage.
// ##########################################

#ifndef CONVERTRULE_18_H_INCLUDED_C31CF69D
#define CONVERTRULE_18_H_INCLUDED_C31CF69D

#include "FieldConvertRule.h"
#include "FieldInfo.h"
#include "DataFormat.h"
#include "CdrFlag.h"
class FieldInfos;
//此转换用于计算两个时间（格式：YYYYMMDDHHMMSS）之差（以秒为单位）
class FieldConvertRule_18 : public FieldConvertRule
{
  public:
  	FieldConvertRule_18(const int iSubChannelNum):FieldConvertRule(iSubChannelNum){};
	/**
      * initialize  解析处理规则18
      * @param   ruleText   用来存放处理规则
      * @return  true 成功, false 失败
      */
	bool initialize(string ruleText);

	/**
     * convertField  计算两个时间（格式：YYYYMMDDHHMMSS）之差（以秒为单位）
     * @param   theFieldMap   字段容器
     * @return  true 成功, false 失败
     */
	//virtual bool convertField(vector<FieldMap> &theFieldMap);
	virtual bool convertField();
	virtual ~FieldConvertRule_18(){}	

  protected:
  	virtual void getFieldObj();
  
  private:
	
	//以秒为单位的开始时间和结束时间字段名
	string m_TimeSubtrahend;
	string m_TimeMinuend;
	string m_SecondDiffName;
	
	FieldInfo *m_pFieldSubtrahend;
	FieldInfo *m_pFieldMinuend;
	FieldInfo *m_pFieldTarget;
	
};

#endif
