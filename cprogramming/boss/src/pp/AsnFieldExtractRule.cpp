// ##########################################
// Source file : AsnFieldExtractRule.cpp
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

#include "AsnFieldExtractRule.h"
#include "FieldInfo.h"
#include "base/StringUtil.h"
#include "Pp_Exception.h"
#include "RepeatableFieldInfo.h"

//##ModelId=485212EE01B5
bool AsnFieldExtractRule::initialize(vector<string> *argname
    							, FieldExtractRule *aParentsField
    							, const int aFloor
    							, const bool isRepeatable
    							, RepeatableFieldInfo *pCompsiteFieldInfo)
{
    // CDR_NO                  ASN,DF30,0,Binary,0
	vector<string> strResults;
	
	// 1.剥离字段名和字段数据类型的配置
	FieldExtractRule::parseConfigRule(*(argname->begin()),strResults);
	
	// 2.得到字段数据类型的子配置项
	if(strResults.size()> 1)
	{
		m_Tag_     = strResults[1];this->strTag2Binary();  // 转换tag 码为二进制码
	}
	
	if(strResults.size()>2)
	{
		m_position = atoi(strResults[2].c_str());
	}
	else
	{
		m_position = 0;	
	}
	if(strResults.size()>3)
	{
		m_dataType = strResults[3];
		if(strResults.size()>4)
		{
			FieldExtractRule::getDataTypeRule(strResults[4]);
		}
	}
    
    getFieldInfoByName(isRepeatable,pCompsiteFieldInfo);

    return true;
}

//##ModelId=4852131E03C8
bool AsnFieldExtractRule::extract(const unsigned char *buffer
		    						, int length
		    						, RecordFormat &aRecordFormat)
{
    int theFieldLength;
    // added by zaiww 2009-09-25 tagSize/valueSize都有值,且valueSize为0
	bool tagAll=false;    

    aRecordFormat.reset(buffer, length, 0, 1);
    theFieldLength = aRecordFormat.getValue(m_tag, m_pFieldInfo, m_position, length, tagAll);

    m_pFieldInfo->m_tagAll		= tagAll;

    return true;
}

// 转换tag 码为二进制码
void AsnFieldExtractRule::strTag2Binary()
{
    char Tmp[100], xxx[10];
    int iLength,i,j;
    
    strcpy(Tmp, m_Tag_.c_str());
    strcpy(m_ss, (char *)"0123456789ABCDEF");
    iLength = m_Tag_.size();

    for(i = 0; i < iLength; i++)
    {
        for(j = 0; j < 16; j++)
        {
            if(m_ss[j] == Tmp[i])
            {
                break;
            }
        }
        xxx[i] = j;
    }

    for(i = 0, j = 0; i < iLength; i += 2, j++)
    {
       m_tag[j] = xxx[i] * 16 + xxx[i + 1];
    }
    	
	return;
}
