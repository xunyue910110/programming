// ##########################################
// Source file : AsnFieldExtractRule.h
// System      : Mobile Billing System
// Version     : 0.1.17
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20020529
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#ifndef ASNFIELD_H_INCLUDED_C4880FE0
#define ASNFIELD_H_INCLUDED_C4880FE0

#include "config-all.h"


#include <vector>
#include <string>
#include "FieldExtractRule.h"
#include "AsnFormat.h"

USING_NAMESPACE(std);

//##ModelId=3B51388600E7
//##Documentation
//## ASN变长字段提取方法类，继承自Field抽象对象。该对象用于从原始记录中提取字段。
class AsnFieldExtractRule : public FieldExtractRule
{
  public:
	//##ModelId=3B7D257F010E
	virtual bool initialize(vector<string> *argname, FieldExtractRule *aParentsField, const int aFloor);

	//##ModelId=3B77DE4700E4
	virtual bool extract(const unsigned char *buffer, int length,
						 multimap<MyString,FieldInfo *> &afieldMap,RecordFormat &aRecordFormat);

  private:
	//##ModelId=3B513DC8038B
	//##Documentation
	//## 保存ASN的Tag码。
	string m_Tag_;

	//##ModelId=3BF468EA0213
	unsigned char m_tag[10];

	//##ModelId=3BF46A210148
    char m_ss[16];

	//##ModelId=3CF467640012
	int m_position;


};



#endif /* ASNFIELD_H_INCLUDED_C4880FE0 */
