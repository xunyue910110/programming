// ##########################################
// Source file : SasnFieldExtractRule.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef SASNFIELDEXTRACTRULE_H_HEADER_INCLUDED_C4880FE0
#define SASNFIELDEXTRACTRULE_H_HEADER_INCLUDED_C4880FE0

#include "AsnFieldExtractRule.h"
#include "SasnFormat.h"
#include "config-all.h"
#include <vector>
#include <string>

USING_NAMESPACE(std);

class MyString;

//##ModelId=3B51388600E7
//##Documentation
//## ASN变长字段提取方法类,继承自Field抽象对象.该对象用于从原始记录中提取字段
class SasnFieldExtractRule : public AsnFieldExtractRule
{
  public:
    SasnFieldExtractRule(int iSubNumber):AsnFieldExtractRule(iSubNumber){}	
    //##ModelId=3B7D257F010E
    virtual bool initialize(vector<string> *argname
    							, FieldExtractRule *aParentsField
    							, const int aFloor
    							, const bool isRepeatable=false
    							, RepeatableFieldInfo *pCompsiteFieldInfo=NULL);

    //##ModelId=3B77DE4700E4
    virtual bool extract(const unsigned char *buffer
				    						, int length
				    						, RecordFormat &aRecordFormat);

};



#endif /* SASNFIELDEXTRACTRULE_H_HEADER_INCLUDED_C4880FE0 */
