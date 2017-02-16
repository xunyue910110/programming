// ##########################################
// Source file : FieldValidateRule_3.cpp
// System      : Mobile Billing System
// Version     : 0.1.7
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020422
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldValidateRule_3.h"

//##ModelId=3BAAA7E20029
// 此方法专用于进行字段值为空校验
bool FieldValidateRule_3::initialize(string ruleText)
{
	vector<string> tmpStrs,tmpStrs1;
	
	// example:    rule_3;fields;01;ALL;IMSI,OTHER_PARTY,CALL_TYPE
	splitStrByToken(ruleText,";",tmpStrs);
	
	if( tmpStrs.size() != 5 )
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	m_callType = tmpStrs[2]; // 跳过 "rule_3","fields" 得到 "01" 

	// 提取规则中除了呼叫类型外的条件: get "MSRN_FLAG=91" or "ALL"
    if(strcmp(tmpStrs[3].c_str(),"ALL")==0)      // "MSRN_FLAG=91" or "ALL"
    {
    	m_conditionFieldName = "ALL";
    } 
    else 
    {
        // 运算符"=",分割条件字段,取得运算符左右键值
        splitStrByToken(tmpStrs[3],"=",tmpStrs1);
        if(tmpStrs1.size()!=2)
        {
        	return false;	
        }
        else
        {
        	m_conditionFieldName  = tmpStrs1[0];
        	m_conditionFieldValue = tmpStrs1[1];	
        }	
	}	
	// 得到 "IMSI,OTHER_PARTY,CALL_TYPE" 的信息
	splitStrByToken(tmpStrs[4],",",tmpStrs1);
	m_fields = tmpStrs1;

	return true;
}

int FieldValidateRule_3::validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    int i,len1;
    FieldInfo *aFieldInfo=0;
    int len,fieldValue,errorCode=0;
    static string callType("CALL_TYPE"),all("ALL");

    if(m_cdrFlag->m_errorCode>0 && m_cdrFlag->m_errorCode!=FORBIDDEN_CHARACTER)  // edit by zhangy in 2002.4.22
         return m_cdrFlag->m_errorCode;

    if( (aFieldInfo=getFieldMap(theFieldInfoMaps,callType))==0 )
	 	return 0;

    // 条件1：通话类型
    if(m_callType.compare(all)!=0) {
       if(aFieldInfo->m_FieldDesValue.compare(m_callType)!=0)
          return 0;
    }

    // 条件2：其他字段值
	if(m_conditionFieldName.compare(all)!=0) {      // not is "ALL"
	    if( (aFieldInfo=getFieldMap(theFieldInfoMaps,m_conditionFieldName))==0 )
		   return false;
	    if(m_conditionFieldValue.compare(all)!=0) {
            if(aFieldInfo->m_FieldDesValue.compare(m_conditionFieldValue)!=0) {
               return 0;
            }
        } else if(aFieldInfo->m_FieldDesValue.size()==0) {      // 如果字段为空
            return 0;
        }
    }

    len1=m_fields.size();

    for(i=0;i<len1;i++)
    {
		FieldInfo *pFieldInfo;
		pFieldInfo = getFieldMap( theFieldInfoMaps,m_fields[i] );
		if( (pFieldInfo->m_FieldDesValue).size()==0) 
		{
		   m_cdrFlag->m_isError    = true;
		   m_cdrFlag->m_fieldName  = aFieldInfo->m_FieldName_;
		
		   return  EMPTY_FIELD;
		}
    }

	return 0;
}

