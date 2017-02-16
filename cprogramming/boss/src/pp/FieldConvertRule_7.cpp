// ##########################################
// Source file : FieldConvertRule_7.cpp
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080926
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldConvertRule_7.h"
#include "FieldInfos.h"


//##ModelId=485618140186
bool FieldConvertRule_7::initialize(string ruleText)
{
    char cTmp[100];
	vector<string> tmpStrs,tmpStrs1;

    //rule_7:CALL_TYPE=01;iHour,iMinute,iSecond
	StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() < 2)
	{
		// 小于1个分号的,不符合规则的书写格式
		return false;	
	}

	// 提取规则中的信息
	if (tmpStrs.size() == 2)
	{
	    tmpStrs[0] = tmpStrs[0].substr(7);
	    if (tmpStrs[0].compare(0,5,"FIELD") == 0)
	    {
	        m_callType = "ALL";
	        m_conditionFieldName = "ALL";
	    }
	    else if (tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
	    {
	       tmpStrs1.clear(); StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	        m_callType = tmpStrs1[1];
	        m_conditionFieldName = "ALL";
	    }
	    else
	    {
	        // 得到条件中的运算符
            strcpy(cTmp,tmpStrs[0].c_str());
            if(strstr(cTmp,"!=")!=NULL) 
            {
                m_conditionExp = "!=";
            } 
            else if(strstr(cTmp,">=")!=NULL) 
            {
                m_conditionExp = ">=";
            }
            else if(strstr(cTmp,"<=") !=NULL)
            {
                m_conditionExp = "<=";
            } 
            else if(strstr(cTmp,"=")!=NULL)
            {
                m_conditionExp = "=";
            }
            else 
            {
                return false;
            }
            // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
            tmpStrs1.clear();StringUtil::split(tmpStrs[0],m_conditionExp.c_str(),tmpStrs1);
            if(tmpStrs1.size()!=2)
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
	    // 提取 iHour,iMinute,iSecond
	    tmpStrs1.clear();StringUtil::split(tmpStrs[1],",",tmpStrs1);
	    if( tmpStrs1.size() == 3 )
	    {
	        m_hour = tmpStrs1[0];
		    m_minute = tmpStrs1[1];
		    m_second = tmpStrs1[2];
	    }
	    else
	    {
		    return false;	
	    }
	    this->getFieldObj();
        return true;
	}
	else if (tmpStrs.size() == 3)
	{
	    tmpStrs[0] = tmpStrs[0].substr(7);
	    
	    if (tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
	    {
	        tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	        m_callType = tmpStrs1[1];
	    }
	    else
	    {
            return false;	  
	    }
	    
	    strcpy(cTmp,tmpStrs[1].c_str());
	    
	    // 得到条件中的运算符
            if(strstr(cTmp,"!=")!=NULL) 
            {
                m_conditionExp = "!=";
            } 
            else if(strstr(cTmp,">=")!=NULL) 
            {
                m_conditionExp = ">=";
            }
            else if(strstr(cTmp,"<=")!=NULL)
            {
                m_conditionExp = "<=";
            } 
            else if(strstr(cTmp,"=")!=NULL)
            {
                m_conditionExp = "=";
            }
            else 
            {
                return false;
            }
            // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
            tmpStrs1.clear();StringUtil::split(tmpStrs[1],m_conditionExp.c_str(),tmpStrs1);
            if(tmpStrs1.size()!=2)
            {
        	    return false;	
            }
            else
            {
                m_conditionFieldName = tmpStrs1[0];
        	    m_conditionFieldValue = tmpStrs1[1];
            }	  
	    
	    tmpStrs1.clear();StringUtil::split(tmpStrs[2],",",tmpStrs1);
	    if( tmpStrs1.size() == 3 )
	    {
	        m_hour = tmpStrs1[0];
		    m_minute = tmpStrs1[1];
		    m_second = tmpStrs1[2];
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
	

//##ModelId=4856181B02CE
bool FieldConvertRule_7::convertField()
{
    char tmp[10];
    int iHour,iMinute,iSecond,l_field,l_value;

	if( FieldConvertRule::generalCondJudge() == false )
	{
		return false;	
	}

	iHour	= m_pFieldDU_hour->m_FieldDesValue==NULL?0:atoi(m_pFieldDU_hour->m_FieldDesValue);
	iMinute	= m_pFieldDU_minute->m_FieldDesValue==NULL?0:atoi(m_pFieldDU_minute->m_FieldDesValue);
	iSecond	= m_pFieldDU_second->m_FieldDesValue==NULL?0:atoi(m_pFieldDU_second->m_FieldDesValue);

    iSecond += iHour * 3600;
    iSecond += iMinute * 60;

    sprintf(tmp,"%d",iSecond);
    m_pFieldDu->setDesValue(tmp);
    editTime( iSecond );    

    return true;
}

void FieldConvertRule_7::getFieldObj()
{
    m_pFieldCondition = m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str());  
	m_pFieldDU_hour   = m_pFieldInfos->getFieldInfoByName(m_hour.c_str());
	m_pFieldDU_minute = m_pFieldInfos->getFieldInfoByName(m_minute.c_str());
	m_pFieldDU_second = m_pFieldInfos->getFieldInfoByName(m_second.c_str());
	m_pFieldDu        = m_pFieldInfos->getFieldInfoByName(DURATION);
	
	return FieldConvertRule::getFieldObj();		
}
