// ##########################################
// Source file : FieldValidateRule_6.h
// System      : Mobile Billing System
// Version     : 0.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : min chen
// E_mail      : chenm@lianchuang.com
// Create      : 20020602
// Update      : 20020602
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef VALIDRULE_6_H_INCLUDED_C45527BB
#define VALIDRULE_6_H_INCLUDED_C45527BB
#include <vector>
#include "FieldValidateRule.h"

//###############################################################################
//## 此校验规则专门针对爱立信交换机vpmn主叫话单剔除问题,当service_key=3(可配)时,
//## 还要判断该主叫话单紧跟着的"AE"话单中的free_format的值是否为配置值时,才删除 
//## 做法:
//##   1.只要主叫话单中的service_key为配置值,就将该话单置为无效话单,输出,但在内存中保留
//##     所有该话单的信息.
//##   2.判断紧跟着的"AE"话单中的free_format的值是否为配置值,如果是,则不作变动,如果不是,
//##     将上一条保存在内存中的主叫话单的信息代替该"AE"话单输出		
//###############################################################################

class cdrErrLog
{
	public:
		void clear()
		{
			m_errFieldName = "";
			m_isErr = false;
			m_errCode = 0;
			return;		
		}
	public:
		string m_errFieldName;
		bool m_isErr;
		int m_errCode;	
};

class FieldValidateRule_6 : public FieldValidateRule
{
  public:
	virtual bool initialize(string ruleText);
	virtual int validField(vector<FieldInfo> &theVectFieldMap);
  
  private:
  	// add by chenm 2004-11-24 
  	void setVpmnFlag();
  	void setCdrFlag(cdrErrLog & theCdrErrLog);         // 把缓存中的错误代码信息付给当前cdr的错误代码
  	void getCdrFlag(const cdrErrLog & theCdrErrLog);         // 记录当前cdr中的错误信息

  private:
	vector<FieldInfo> m_vectFieldMaps;
	string m_callType1;
	string m_fciFieldName;           //FCI字段在配置文件中的字段名称
	string m_fciFieldValue;          //FCI在原始话单中的值
	string m_freeFieldName;          //FREE_FORMAT字段在配置文件中的字段名称
	string m_freeFieldValue;         //FREE_FORMAT在原始话单中的值
	int m_sequnce;                   //m_sequnce 代表该话单在复合话单中出现的顺序,1:在...之前,2:在...之后
	bool m_isVpmnCall;               //记录是否为vpmn话单 add by chenm 2004-12-9
	string m_vpmnLongNum;            //记录AD话单中的对方号码长号 add by chenm 2004-12-9
	
	cdrErrLog m_localCdrErrLog;           // 记录缓存中cdr的错误代码信息
};



#endif /* VALIDRULE_5_H_INCLUDED_C45527BB */
