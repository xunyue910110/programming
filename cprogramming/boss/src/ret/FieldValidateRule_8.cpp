// ##########################################
// Source file : FieldValidateRule_8.cpp
// System      : Mobile Billing System
// Version     : 0.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : chengm@lianchuang.com
// Create      : 20021114
// Update      : 20021114
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldValidateRule_8.h"

bool FieldValidateRule_8::initialize(string ruleText)
{
	vector<string> tmpStrs;
	// example:	rule_8;fields;ALL;other_party   
	// 此校验方法专用于对给定的字段进行非法字符校验
    splitStrByToken(ruleText,";",tmpStrs);
    
	if( tmpStrs.size() != 4 )
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	
	m_callType  = tmpStrs[2];  // skip "rule_5","fields" get "ALL"
	m_fieldName = tmpStrs[3];  // is "other_party"
 
	return true;
}

//##ModelId=3BAAAA29017C
// 此校验方法专用于对给定的字段进行非法字符校验
int FieldValidateRule_8::validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    int len;
    char tmp[80];
    FieldInfo *tmpFieldInfo;
    static string callType("CALL_TYPE"),all("ALL");
    
    if(m_cdrFlag->m_errorCode>0 && m_cdrFlag->m_errorCode!=FORBIDDEN_CHARACTER)  // edit by zhangy in 2002.4.22
         return m_cdrFlag->m_errorCode;

    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,callType))==0 )
	 	return 0;
 
    // 条件：通话类型
    if(m_callType.compare(all)!=0) {
       if(tmpFieldInfo->m_FieldDesValue.compare(m_callType)!=0)
          return 0;
    }
    
    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_fieldName))==0 )
	 	return 0;
    
    strcpy(tmp,tmpFieldInfo->m_FieldDesValue.c_str());
    len=strlen(tmp);
    for(int i=0;i<len;i++)
    {
        if( tmp[i]<0x30 || tmp[i]>0x39 ) {
            m_cdrFlag->m_isError    = true;
	 	    m_cdrFlag->m_fieldName  = tmpFieldInfo->m_FieldName_;

            return  FORBIDDEN_CHARACTER;
        }
    }

    return 0;
}

