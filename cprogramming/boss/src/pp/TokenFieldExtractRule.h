// ##########################################
// Source file : TokenFieldExtractRule.h
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

#ifndef TOKENFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A66FC4
#define TOKENFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A66FC4

#include "config-all.h"
#include "FieldExtractRule.h"
#include "RecordFormat.h"
#include <string>
#include <vector>

USING_NAMESPACE(std);

//##ModelId=485219B0036B
//##Documentation
//## 分隔符记录字段提取方法类,继承自Field抽象对象. 该对象用于从原始记录中提取字段
class TokenFieldExtractRule : public FieldExtractRule
{
  public:
    TokenFieldExtractRule(int iSubNumber):FieldExtractRule(iSubNumber){}	
    //##ModelId=485219C70203
    virtual bool initialize(vector<string> *argname
    							, FieldExtractRule *aParentsField
    							, const int aFloor
    							, const bool isRepeatable=false
    							, RepeatableFieldInfo *pCompsiteFieldInfo=NULL);

    //##ModelId=485219D3004E
    virtual bool extract(const unsigned char *buffer, int length,
                                               RecordFormat &aRecordFormat);

    //##ModelId=485219DF0148
    int getValue(const unsigned char *buffer, unsigned char *aValue, int len);

    //##ModelId=48521A0403A9
    int getNextToken(const char *cdrBuffer, int offset, int len);
    
    //得到字段的长度和偏移量  add by zhangch2 2009-12-25 16:06
    int getLength(const unsigned char *buffer,int len,int &offset);

  private:
    //##ModelId=48521A2501D4
    int m_position;

};



#endif /* TOKENFIELDEXTRACTRULE_H_HEADER_INCLUDED_B7A66FC4 */
