// ##########################################
// Source file : SasnFieldExtractRule.cpp
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

#include "SasnFieldExtractRule.h"
#include "RepeatableFieldInfo.h"

//##ModelId=3B7D257F010E
bool SasnFieldExtractRule::initialize(vector<string> *argname
    							, FieldExtractRule *aParentsField
    							, const int aFloor
    							, const bool isRepeatable
    							, RepeatableFieldInfo *pCompsiteFieldInfo)
{
    // CDR_NO                  SASN,DF30,0,Binary,0
	return AsnFieldExtractRule::initialize(argname
			    							, aParentsField
			    							, aFloor
			    							, isRepeatable
			    							, pCompsiteFieldInfo);
}

//##ModelId=3B77DE4700E4
bool SasnFieldExtractRule::extract(const unsigned char *buffer
				    						, int length
				    						, RecordFormat &aRecordFormat)
{
    int theFieldLength;
    // added by zaiww 2009-09-25 tagSize/valueSize都有值,且valueSize为0
	bool tagAll=false;

    aRecordFormat.reset(buffer, length, 0, 1);
    theFieldLength = aRecordFormat.getValue(m_tag,m_pFieldInfo,m_position,length,tagAll);
    m_pFieldInfo->m_tagAll		= tagAll;

    return true;
}
