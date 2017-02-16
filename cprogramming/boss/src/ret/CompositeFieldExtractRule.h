// ##########################################
// Source file : CompositeFieldExtractRule.h
// System      : Mobile Billing System
// Version     : 0.1.33
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010813
// Update      : 20020102
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef COMPOSITEFIELD_H_INCLUDED_C4882CB4
#define COMPOSITEFIELD_H_INCLUDED_C4882CB4

#include "config-all.h"

#if defined(STDIOSTREAM_DISABLE)
#include <iostream.h>
#include <fstream.h>
#else
#include <iostream>
#include <fstream>
#endif

#include <vector>
#include <string>
#include <stdio.h>

#include "FieldExtractRule.h"
#include "FieldInfo.h"
//#include "AsnFieldExtractRule.h"
#include "TokenFieldExtractRule.h"
#include "FixedFieldExtractRule.h"
//#include "GasnFieldExtractRule.h"

USING_NAMESPACE(std);

//##ModelId=3B7887A800B8
//##Documentation
//## 基于Composite(组合)模型的字段类，用于生成字段对象（任意层数，任意分支。实际数目和层数完全由配置文件指定，本模型对此没有限制）
class CompositeFieldExtractRule : public FieldExtractRule {
  public:
	//##ModelId=3B7887F8003A
	//##Documentation
	//## 初始化CompositeField模型。
	virtual bool initialize(vector<string> *argname, FieldExtractRule *aParentsField, const int aFloor);

	//##ModelId=3B7887F80211
	virtual bool extract(const unsigned char *buffer, int length,
						 multimap<MyString,FieldInfo *> &afieldMap,RecordFormat &aRecordFormat);

	//##ModelId=3B7B67FB01A7
	CompositeFieldExtractRule();

	//##ModelId=3B7887F9005A
	virtual ~CompositeFieldExtractRule();
	//##ModelId=3C329E5200CB
	bool setToken(string token);

	//##ModelId=3B78888F03B3
	//##Documentation
	//## 子字段(field or compositeFieldExtractRule)对象指针保存容器
	vector<FieldExtractRule *> m_theChildField_;

  private:
    // add by chenm 2007-1-6
  	void getCdrSign(const unsigned char *srcCdrValue	,
	                string &theSign ,RecordFormat &aRecordFormat);

  private:
	//##ModelId=3B7DD0A902AB
  	string getFieldType(string srcString,string &desString);

	//##ModelId=3B7DDF4F0361
	FieldExtractRule *createChildFieldObject(FieldExtractRule * &aChildField,string aFieldType);

	//##ModelId=3B8E4DF802C5
	void getSectionName(string &aSectionName);

	//##ModelId=3C32A48C005F
	char m_myToken;


};

#endif /* COMPOSITEFIELD_H_INCLUDED_C4882CB4 */
