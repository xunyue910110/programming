// ##########################################
// Source file : FieldValidateRule_10.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080922
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldValidateRule_10.h"
#include "FieldInfos.h"

//##ModelId=485647D400DA
bool FieldValidateRule_10::initialize(string ruleText)
{
    vector<string> tmpStrs,tmpStrs1;
	// example:	rule_10:VALID_TYPE=fields;CALL_TYPE=ALL;other_party   
	// 此校验方法专用于对给定的字段进行非法字符校验
  //splitStrByToken(ruleText,";",tmpStrs);
	StringUtil::split(ruleText,";",tmpStrs);
    
	if( tmpStrs.size() != 3 )
	{
		// 小于2个分号的,不符合规则的书写格式
		return false;	
	}
	//splitStrByToken(tmpStrs[1],"=",tmpStrs1);
  tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);
	if (tmpStrs1.size() != 2)
	{
	    return false;
    }
    else
    {
        m_callType = tmpStrs1[1];  //get "ALL"
    }
    
	m_fieldName = tmpStrs[2];  // get "other_party"
 
	return true;
}

//##ModelId=485647DB009C
int FieldValidateRule_10::validField()
{
    int len;
    char tmp[80];
    FieldInfo *tmpFieldInfo;
    static string callType("CALL_TYPE"),all("ALL"),t1,t2;
    
    if(m_cdrFlag->m_errorCode > 0 && m_cdrFlag->m_errorCode!=FORBIDDEN_CHARACTER)             // edit by zhangy in 2002.4.22
    {
        return m_cdrFlag->m_errorCode;
    }

    //if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,callType))==0 )
    if((tmpFieldInfo=m_pFieldInfos->getFieldInfoByName(callType.c_str()))==0)
	{
	    return 0;
	}
 
    // 条件：通话类型
    if(m_callType.compare(all)!=0)
    {
    	 t1=tmpFieldInfo->m_FieldDesValue;
       if(t1.compare(m_callType)!=0)
       {
           return 0;
       }
    }
    
    //if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_fieldName))==0 )
    if((tmpFieldInfo=m_pFieldInfos->getFieldInfoByName(m_fieldName.c_str()))==0)
    {
	    return 0;
	}
    t2=tmpFieldInfo->m_FieldDesValue;
    strcpy(tmp,t2.c_str());
    len=strlen(tmp);
    for(int i=0;i<len;i++)
    {
        if( tmp[i]>=0x41 &&  tmp[i]<=0x5A || tmp[i]>=0x61 &&  tmp[i]<=0x7A )
        { 
            ;
        }
        else
        {
            m_cdrFlag->m_isError    = true;
	 	    m_cdrFlag->m_fieldName  = tmpFieldInfo->m_FieldName_;

            return  FORBIDDEN_CHARACTER;
        }
    }

    return 0;
}


