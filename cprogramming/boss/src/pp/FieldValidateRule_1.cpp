// ##########################################
// Source file : FieldValidateRule_1.cpp
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080922
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldValidateRule_1.h"


//##ModelId=48564530036B
//RULE_1:
bool FieldValidateRule_1::initialize(string ruleText)
{
    string tmpStr;
	vector<string> tmpStrs,tmpStrs1;

    // rule_1:VALID_TYPE=fields;FIELD=ALL;DURATION,value,0~10800			    # 超长超短校验
	StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() < 3)
	{
		// 小于2个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	
	if (tmpStrs.size() == 3)
	{                                                     
	    if (tmpStrs[1].compare(0,5,"FIELD") == 0)         //跳过"rule_1:VALID_TYPE=fields"
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);
	        m_conditionFieldName = tmpStrs1[1];
	        m_callType = "ALL";
	    }
	    else if(tmpStrs[1].compare(0,9,"CALL_TYPE") == 0)
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);
	        m_callType = tmpStrs1[1];
	        m_conditionFieldName = "ALL";
	    }
	    else
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);
	        if (tmpStrs1.size() != 2)
	        {
	            return false;
	        }
	        else
	        {
	            m_conditionFieldName = tmpStrs1[0];
	            m_conditionFieldValue = tmpStrs1[1];
	            m_callType = "ALL";
	        }
	    }
	    // 提取 "DURATION,value,0~10800"
	    tmpStrs1.clear();StringUtil::split(tmpStrs[2],",",tmpStrs1);
	    if( tmpStrs1.size() == 3 )
	    {
		    m_fieldName = tmpStrs1[0];     // get "DURATION"
            m_valudType = tmpStrs1[1];       // get "value"              
            tmpStr = tmpStrs1[2];	                      // get "0~10800"
		    tmpStrs1.clear();StringUtil::split(tmpStr,"~",tmpStrs1);
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
		this->getFieldObj();
	    return true;
	}
	else if(tmpStrs.size() == 4)
	{
	    if (tmpStrs[1].compare(0,9,"CALL_TYPE") != 0)
	    {
	        return false;
	    }
	    else
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);
	        m_callType = tmpStrs1[1];
	    }
	    tmpStrs1.clear();StringUtil::split(tmpStrs[2],"=",tmpStrs1);
	    m_conditionFieldName = tmpStrs1[0];
	    m_conditionFieldValue = tmpStrs1[1];

	    
	    // 提取 "DURATION,value,0~10800"
	    tmpStrs1.clear();StringUtil::split(tmpStrs[3],",",tmpStrs1);
	    if( tmpStrs1.size() == 3 )
	    {
		    m_fieldName = tmpStrs1[0];                // get "DURATION"
            m_valudType = tmpStrs1[1];                // get "value"
		    tmpStr=tmpStrs1[2];	                      // get "0~10800"
		    tmpStrs1.clear();StringUtil::split(tmpStr,"~",tmpStrs1);
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
		this->getFieldObj();
	    return true;
	}
	else
	{
	    return false;
    }
}

//##ModelId=485645370177
int FieldValidateRule_1::validField()
{
    int len,fieldValue,errorCode=0;

    if( (m_cdrFlag->m_errorCode != FORBIDDEN_CHARACTER) && (m_cdrFlag->m_errorCode > 0) )
    {
        return m_cdrFlag->m_errorCode;
    }

    if( generalCondJudge()==false )  
	{
	    return 0;
	}

    if( m_pFieldTarget->m_iDesValueLen==0 ) 
    {
	     return 0;
	}

    if(m_valudType.compare("length")==0) 
    {
         fieldValue=m_pFieldTarget->m_iDesValueLen;
    } 
    else if(m_valudType.compare("value")==0) 
    {
         fieldValue=atoi( m_pFieldTarget->m_FieldDesValue );
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

    if (!m_pFieldTarget->m_IsCheck) 
    {
         m_pFieldTarget->m_IsCheck    = true;
         m_pFieldTarget->m_errorCode  = errorCode;
    }
    m_cdrFlag->m_isError    = true;                      // edit by zhangy in 2002.4.22
	
	m_cdrFlag->m_fieldName  = m_fieldName;   // edit by zhangy in 2002.4.22

	return errorCode;
}

void FieldValidateRule_1::getFieldObj()
{
	m_pFieldTarget = m_pFieldInfos->getFieldInfoByName(m_fieldName.c_str());
	
	return FieldValidateRule::getFieldObj();	
}

