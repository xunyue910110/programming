// ##########################################
// Source file : TokenFieldExtractRule.h
// System      : Mobile Billing System
// Version     : 0.1.22
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010813
// Update      : 20020102
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef TOKENFIELD_H_INCLUDED_C4887FD6
#define TOKENFIELD_H_INCLUDED_C4887FD6

#include "config-all.h"

#include <string>
#include <vector>
#include "FieldExtractRule.h"

USING_NAMESPACE(std)

//##ModelId=3B6014200396
//##Documentation
//## 分隔符记录字段提取方法类，继承自Field抽象对象。该对象用于从原始记录中提取字段。

class TokenFieldExtractRule : public FieldExtractRule {
  public:
	//##ModelId=3B7D2580021E
	virtual bool initialize(vector<string> *argname, FieldExtractRule *aParentsField, const int aFloor);

	//##ModelId=3B77DFCB03A9
	virtual bool extract(const unsigned char *buffer, int length,
					   	 multimap<MyString,FieldInfo *> &afieldMap,RecordFormat &aRecordFormat);

  private:
	//##ModelId=3C316DBD00B7
   int getValue(const unsigned char *buffer,unsigned char *aValue,int len);

	//##ModelId=3C31B553038A
   int getNextToken(const char *cdrBuffer,int offset,int len);

  private:

	//##ModelId=3C31B59602C8
	int m_position;


};

#endif /* TOKENFIELD_H_INCLUDED_C4887FD6 */
