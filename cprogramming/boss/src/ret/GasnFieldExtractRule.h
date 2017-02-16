// ##########################################
// Source file : GasnFieldExtractRule.h
// System      : Mobile Billing System
// Version     : 0.1.17
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : xu jun
// E_mail      : xujun@lianchuang.com
// Create      : 20031113
// ##########################################

#ifndef GASNFIELDEXTRACTRULE_H_INCLUDED_C4880FE0
#define GASNFIELDEXTRACTRULE_H_INCLUDED_C4880FE0

#include "config-all.h"


#include <vector>
#include <string>
#include "FieldExtractRule.h"
#include "AsnFormat.h"

USING_NAMESPACE(std);

//##ModelId=3B51388600E7
//##Documentation
//## ASN变长-链表字段格式的提取方法类，继承自Field抽象对象。该对象用于从原始记录中提取字段。
class GasnFieldExtractRule : public FieldExtractRule
{
  public:
	//##ModelId=3B7D257F010E
	virtual bool initialize(vector<string> *argname, FieldExtractRule *aParentsField, const int aFloor);

	//##ModelId=3B77DE4700E4
	virtual bool extract(const unsigned char *buffer, int length,
						 multimap<MyString,FieldInfo *> &afieldInfoMaps,RecordFormat &aRecordFormat);

  private:
	//##ModelId=3B513DC8038B
	//##Documentation
	//## 保存GASN的Tag码。
	string m_Tag_;

	//##ModelId=3BF468EA0213
	unsigned char m_tag[10];

	//##ModelId=3BF46A210148
    char m_ss[16];

	//##ModelId=3CF467640012
	int m_position;

};



#endif /* GASNFIELD_H_INCLUDED_C4880FE0 */
