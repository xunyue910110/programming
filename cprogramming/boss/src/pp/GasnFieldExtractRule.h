// ##########################################
// Source file : GasnFieldExtractRule.h
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

#ifndef GASNFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A62D96
#define GASNFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A62D96

#include "AsnFieldExtractRule.h"
#include "RecordFormat.h"
#include "AsnFormat.h"
#include "config-all.h"
#include <vector>
#include <string>

USING_NAMESPACE(std);

class FieldInfo;

//##ModelId=4852145B02AF
//##Documentation
//## ASN变长-链表字段格式的提取方法类,继承自Field抽象对象.该对象用于从原始记录中提取字段
class GasnFieldExtractRule : public AsnFieldExtractRule
{
  public:
    GasnFieldExtractRule(int iSubNumber):AsnFieldExtractRule(iSubNumber){}
    //##ModelId=48521470037A
    virtual bool initialize(vector<string> *argname
    							, FieldExtractRule *aParentsField
    							, const int aFloor
    							, const bool isRepeatable=false
    							, RepeatableFieldInfo *pCompsiteFieldInfo=NULL);

    //##ModelId=485214A6003E
    virtual bool extract(const unsigned char *buffer, int length
    							,RecordFormat &aRecordFormat);
};



#endif /* GASNFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A62D96 */
