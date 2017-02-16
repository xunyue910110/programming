// ##########################################
// Source file : TokenFieldExtractRule.cpp
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

#include "TokenFieldExtractRule.h"
#include "base/StringUtil.h"
#include "Pp_Exception.h"
#include "RepeatableFieldInfo.h"

//##ModelId=485219C70203
bool TokenFieldExtractRule::initialize(vector<string> *argname
										, FieldExtractRule *aParentsField
										, const int aFloor
										, const bool isRepeatable
										, RepeatableFieldInfo *pCompsiteFieldInfo)
{
    // CDR_NO            TOKEN,1,Ascii
	vector<string> strResults;
	
	// 1.剥离字段名和字段数据类型的配置
	FieldExtractRule::parseConfigRule(*(argname->begin()),strResults);
	
	// 2.得到字段数据类型的子配置项	
	if(strResults.size()< 3)
	{
		throw Pp_Exception (__FILE__, __LINE__,"解析配置记录 %s 错!",argname->begin()->c_str());		
	}	
	m_position  = atoi(strResults[1].c_str());
	m_dataType  = strResults[2];
	if(strResults.size()>3)
	{
		FieldExtractRule::getDataTypeRule(strResults[3]);
	}

    getFieldInfoByName(isRepeatable,pCompsiteFieldInfo);
    
    return true;
}

//##ModelId=485219D3004E
bool TokenFieldExtractRule::extract(const unsigned char *buffer
				    						, int length
				    						, RecordFormat &aRecordFormat)
{
    int theFieldLength, theFieldOffset;
    
    //当theFieldLength大于1023时,theValue数组会越界 add by zhangch2 2009-12-25 16:06
    theFieldLength  = getLength(buffer, length, theFieldOffset);
    
    //cout<<"*****token len1:"<<theFieldLength<<"*** the len2:"<<theFieldOffset<<endl;
    
	  m_pFieldInfo->setSrcValue(buffer + theFieldOffset, theFieldLength);
	    
    //string fname1=m_pFieldInfo->m_FieldName_;
    //char tmpa[1000];
    //memcpy(tmpa,m_pFieldInfo->m_FieldSrcValue,theFieldLength);
    
    //cout<<"*****token filed name:"<<fname1<<"*** the value:"<<tmpa<<endl;
    return true;
}


//add by zhangch2 2009-12-26 for getValue()函数中参数aValue初始化大小小于实际值长度的情况
int TokenFieldExtractRule::getLength(const unsigned char *buffer,int len,int &offset)
{
    int n, offset1, fieldLen;

    n        = 1;
    offset   = 0;
    fieldLen = -1;
    while((offset1 = getNextToken((char *)buffer, offset, len)) <= len)
    {
        fieldLen = offset1 - offset;
        
        if(n == m_position)
        {
            break;
        }
        offset1 = getNextToken((char *)buffer, offset, len) ;
        offset += (fieldLen + 1);
        n++;
    }
    if(offset1 > len)
    {
        //offset1越出字串长度，直接置fieldLen=0, by zaiww 20090811
        //fieldLen--;
        fieldLen = 0;
    }

    return fieldLen;
}


//##ModelId=485219DF0148
int TokenFieldExtractRule::getValue(const unsigned char *buffer, unsigned char *aValue, int len)
{
    int n, offset, offset1, fieldLen;

    n        = 1;
    offset   = 0;
    fieldLen = -1;

    while((offset1 = getNextToken((char *)buffer, offset, len)) <= len)
    {
        fieldLen = offset1 - offset;
        if(n == m_position)
        {
            memcpy(aValue, buffer + offset, fieldLen);
            aValue[fieldLen] = 0;
            break;
        }
        offset1 = getNextToken((char *)buffer, offset, len) ;
        offset += (fieldLen + 1);
        n++;
    }
    if(offset1 > len)
    {
        //offset1越出字串长度，直接置fieldLen=0, by zaiww 20090811
        //fieldLen--;
        fieldLen = 0;
        aValue[fieldLen] = 0;
    }

    return fieldLen;
}

//##ModelId=48521A0403A9
int TokenFieldExtractRule::getNextToken(const char *cdrBuffer, int offset, int len)
{
    int i;

    for(i = offset; i < len; i++)
    {
        if(cdrBuffer[i] == m_token)
        {
            return i;
        }
    }

    if(i == offset)
    {
        len++;
    }

    return len;
}

