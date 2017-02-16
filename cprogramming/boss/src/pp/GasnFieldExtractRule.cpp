// ##########################################
// Source file : GasnFieldExtractRule.cpp
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

#include "GasnFieldExtractRule.h"
#include "RepeatableFieldInfo.h"

//##ModelId=48521470037A
bool GasnFieldExtractRule::initialize(vector<string> *argname
		    							, FieldExtractRule *aParentsField
		    							, const int aFloor
		    							, const bool isRepeatable
		    							, RepeatableFieldInfo *pCompsiteFieldInfo)
{
	// CDR_NO                  GASN,DF30,0,Binary,0
	return AsnFieldExtractRule::initialize(argname
				    							, aParentsField
				    							, aFloor
				    							, true
				    							, pCompsiteFieldInfo);
}

//##ModelId=485214A6003E
bool GasnFieldExtractRule::extract(const unsigned char *buffer
				    						, int length
				    						, RecordFormat &aRecordFormat)
{
    int theFieldLength;
    // added by zaiww 2009-09-25 tagSize/valueSize都有值,且valueSize为0
	bool tagAll=false;

    aRecordFormat.reset(buffer, length, 0, 1);

    while((theFieldLength = aRecordFormat.getValue(m_tag, m_pFieldInfo, 0, length, tagAll)) > 0);
    
    return true;
}

