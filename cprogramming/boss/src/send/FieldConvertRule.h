// ##########################################
// Source file : FieldConvertRule.h
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020516
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef FIELDCONVERTRULE_H_INCLUDED_C45511D8
#define FIELDCONVERTRULE_H_INCLUDED_C45511D8

#include "config-all.h"

#include <string>
#include <vector>
#include <map>
#include <stdio.h>

#include "SendLog.h"
#include "CdrFlag.h"
#include "FieldInfo.h"
#include "CdrErrorCode.h"
#include "DateTime.h"


USING_NAMESPACE(std)

//##ModelId=3BAABBC302ED
//##Documentation
//## 字段值转换抽象类
class FieldConvertRule
{
  public:
	//##ModelId=3BAAEAC8004D
	virtual bool initialize(string ruleText) = 0;

	//##ModelId=3BAFF7BD00BF
	FieldConvertRule();

	//##ModelId=3BAAEB050330
	virtual bool convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps) = 0;
	
	virtual ~FieldConvertRule(){}
	
	//##ModelId=3BAF2A7B03DE
	static FieldInfo *getFieldMap(multimap<MyString,FieldInfo *> &theFieldInfoMaps,const string &theFieldName);

	// add by chenm 获取多个同名字段的转换后的值,结果由desValues带出
	void getMultiFieldValues(multimap<MyString,FieldInfo *> &theFieldInfoMaps,
	                         const string &theFieldName,
	                         vector<string> &desValues);
  protected:
	//##ModelId=3BEB8CF70280
	CdrFlag *m_CdrFlag;

	//##ModelId=3BAF2A7B0399
	SendLog *m_SendLog;

	//##ModelId=3C19C1750189
	string m_callType;

	//##ModelId=3C244E6C0006
	// created by yaop at 2001.12.22 in hainan.
	int strnrps(const char* src,const char* token,const char *rps,char *des,int max);

	//##ModelId=3BAFF7BD0173
	string lrTrim(string theString);

	//##ModelId=3CE312990130
	//##Documentation
	//## 用于根据开始时间和被修改的时长，重新计算出结束时间。
	//## add by zhangy in 2002.5.16
	void editTime(int duration, multimap<MyString,FieldInfo *> &theFieldInfoMaps);


  private:


};



#endif /* FieldConvertRule_H_INCLUDED_C45511D8 */
