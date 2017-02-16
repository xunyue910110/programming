// ##########################################
// Source file : FieldValidateRule_1.cpp
// System      : Mobile Billing System
// Version     : 0.1.9
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020422
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldValidateRule_1.h"

//##ModelId=3BAAA7DB0386
bool FieldValidateRule_1::initialize(string ruleText)
{
	string tmpStr;
	vector<string> tmpStrs,tmpStrs1;

    // rule_1;fields;01;ALL;DURATION,value,0~10800			    # 超长超短校验
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 5)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	m_callType = tmpStrs[2]; //跳过 "rule_1","fields"
	
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
	
	// 提取 "DURATION,value,0~10800"
	splitStrByToken(tmpStrs[4],",",tmpStrs1);
	if( tmpStrs1.size() == 3 )
	{
		m_fieldName = tmpStrs1[0];  // get "DURATION"
		m_valudType = tmpStrs1[1];  // get "value"
		
		tmpStr      = tmpStrs1[2];	// get "0~10800"
		splitStrByToken(tmpStr,"~",tmpStrs1);
		if( tmpStrs1.size() == 2 )
		{
			m_min = atoi(tmpStrs1[0].c_str());
			m_max = atoi(tmpStrs1[1].c_str());	
		}
		else
		{
			return false;	
		}
	}
	else
	{
		return false;	
	}

	return true;
}

//##ModelId=3BAAAA25008F
int FieldValidateRule_1::validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    FieldInfo *aFieldInfo=0;
    int len,fieldValue,errorCode=0;
    static string callType("CALL_TYPE"),all("ALL");

    if( (m_cdrFlag->m_errorCode != FORBIDDEN_CHARACTER) && (m_cdrFlag->m_errorCode > 0) )
        return m_cdrFlag->m_errorCode;

    if( (aFieldInfo=getFieldMap(theFieldInfoMaps,callType))==0 )  // edit by zhangy in 2002.4.22
	 	return 0;

    // 条件1：通话类型
    if(m_callType.compare(all)!=0) 
    {
        if(aFieldInfo->m_FieldDesValue.compare(m_callType)!=0)
            return 0;
    }

    // 条件2：其他字段值
	if(m_conditionFieldName.compare(all)!=0) 
	{      // not is "ALL"
	    if( (aFieldInfo=getFieldMap(theFieldInfoMaps,m_conditionFieldName))==0 )
		    return 0;
		if(m_conditionFieldValue.compare(all)!=0) 
		{
            if(aFieldInfo->m_FieldDesValue.compare(m_conditionFieldValue)!=0) 
            {
               return 0;
            }
        } 
        else if(aFieldInfo->m_FieldDesValue.size()==0) 
        {      // 如果字段为空
              return 0;
        }
    }

    if( (aFieldInfo=getFieldMap(theFieldInfoMaps,m_fieldName))==0 ) 
    {
	     return 0;
	}

    if(m_valudType.compare("length")==0) 
    {
         fieldValue=(aFieldInfo->m_FieldDesValue).size();
    } 
    else if(m_valudType.compare("value")==0) 
    {
         fieldValue=atoi( (aFieldInfo->m_FieldDesValue).c_str() );
    } 
    else 
    {
         return -1;
    }

    if( (fieldValue<m_min) || (fieldValue>m_max) ) 
    {
        if( m_fieldName.compare("DURATION")==0 || m_valudType.compare("value")==0 ) 
        {
            errorCode = INVALID_DURATION;
        } 
        else 
        {
            errorCode = LENGTH_FIELD;
        }
    } 
    else 
    {
        return 0;
    }

//    if (!aFieldInfo->m_IsCheck) 
    {
//         aFieldInfo->m_IsCheck    = true;
         aFieldInfo->m_errorCode  = errorCode;
    }
    m_cdrFlag->m_isError    = true;                      // edit by zhangy in 2002.4.22
	
	//如果是时长检验出错,则将出错字段名置为"START_TIME",以防"时长"不输出时,无法带出错误码 by chenmin 2002-11-8 14:01
	if(m_fieldName.compare("DURATION")==0)               
	{
		m_cdrFlag->m_fieldName  = "START_TIME";	
	}
	else
	{
		m_cdrFlag->m_fieldName  = aFieldInfo->m_FieldName_;   // edit by zhangy in 2002.4.22
	}

	return errorCode;
}

