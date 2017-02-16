// ##########################################
// Source file : FieldConvertRule_18.cpp
// System      : Mobile Billing System
// Version     : 0.1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Tao Li
// E_mail      : litao@lianchuang.com
// mender      : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20070803
// Update      : 20080926
// Copyright(C): 2002 by Tao Li, Linkage.
// ##########################################

#include "FieldConvertRule_18.h"
#include "FieldInfos.h"

/**
 * initialize  解析处理规则19
 * @param   ruleText   用来存放处理规则
 * @return  true 成功, false 失败
 */
bool FieldConvertRule_18::initialize(string ruleText)
{
	vector<string> theVector;
	vector<string> tmpStrs,tmpStrs1;

    //rule_18:CALL_TYPE=ALL;FIELD=secondDiff;timeSubtrahend,timeMinuend
    StringUtil::split(ruleText,";",tmpStrs);
    
    if (tmpStrs.size() < 2)
    {
        return false;
    }
    
    if (tmpStrs.size() == 2)
    {
        tmpStrs[0]=tmpStrs[0].substr(8);
        if (tmpStrs[0].compare(0,5,"FIELD") == 0)
        {
            tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
            if (tmpStrs1.size() != 2)
            {
                return false;
            }
            else
            {
                m_SecondDiffName = tmpStrs1[1];
                m_callType = "ALL";
            }
        }
        else
        {
            return false;
        }
        tmpStrs1.clear();StringUtil::split(tmpStrs[1],",",tmpStrs1);
        if (tmpStrs1.size() != 2)
        {
            return false;
        }
        else
        {
            m_TimeSubtrahend = tmpStrs1[0];
            m_TimeMinuend = tmpStrs1[1];
        }
        this->getFieldObj();
        return true;
    }
    else if(tmpStrs.size() == 3)
    {
        tmpStrs[0]=tmpStrs[0].substr(8);
        if (tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
        {
            tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
            m_callType = tmpStrs1[1];
        }
        else
        {
            return false;
        }
        
       
        if (tmpStrs[1].compare(0,5,"FIELD") == 0)
        {
            tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);
            m_SecondDiffName = tmpStrs1[1];
        }
        else
        {
            return false;
        }
        
        tmpStrs1.clear();StringUtil::split(tmpStrs[2],",",tmpStrs1);
        if (tmpStrs1.size() != 2)
        {
            return false;
        }
        else
        {
            m_TimeSubtrahend = tmpStrs1[0];
            m_TimeMinuend = tmpStrs1[1];
        }
        this->getFieldObj();
        return true;
    }
    else
    {
        return false;
    }
}


/**
 * convertField  计算两个时间（格式：YYYYMMDDHHMMSS）之差（以秒为单位）
 * @param   theFieldMap   字段容器
 * @return  true 成功, false 失败
 */
bool FieldConvertRule_18::convertField()
{
	int iYear,iMonth,iDay,iHour,iMinute,iSecond;
    DateTime subtrahendDateTime,minuendDateTime;
    char cDuration[50],cTmp1[20],cTmp2[4+1];

    if(m_callType.compare(CONDITION_ALL)!=0) 
    {
        if( m_pFieldCallType->m_iDesValueLen == 0
        	||strcmp(m_pFieldCallType->m_FieldDesValue,m_callType.c_str())!=0 ) 
        {
            return false;
        }
    }
    
    if( m_pFieldSubtrahend->m_iDesValueLen < 14 
    	||m_pFieldMinuend->m_iDesValueLen < 14   )
	{
		m_CdrFlag->m_isError       = true;
	    m_CdrFlag->m_fieldName     = m_TimeSubtrahend;
	    m_CdrFlag->m_errorCode     = INVALID_DATE_TIME;
		
		return false;	
	}
	
	//初始化时间减数
	strcpy(cTmp1,m_pFieldSubtrahend->m_FieldDesValue);

    strncpy(cTmp2,cTmp1,4);cTmp2[4] = 0;iYear = atoi(cTmp2);
    strncpy(cTmp2,cTmp1+4,2);cTmp2[2] = 0;iMonth = atoi(cTmp2);
    strncpy(cTmp2,cTmp1+6,2);cTmp2[2] = 0;iDay = atoi(cTmp2);
    strncpy(cTmp2,cTmp1+8,2);cTmp2[2] = 0;iHour = atoi(cTmp2);
    strncpy(cTmp2,cTmp1+10,2);cTmp2[2] = 0;iMinute = atoi(cTmp2);
    strncpy(cTmp2,cTmp1+12,2);cTmp2[2] = 0;iSecond = atoi(cTmp2);
    
	subtrahendDateTime.setDateTime(iYear,iMonth,iDay,iHour,iMinute,iSecond);
	
	//初始化时间被减数
	strcpy(cTmp1,m_pFieldMinuend->m_FieldDesValue);

	strncpy(cTmp2,cTmp1,4);cTmp2[4] = 0;iYear = atoi(cTmp2);
    strncpy(cTmp2,cTmp1+4,2);cTmp2[2] = 0;iMonth = atoi(cTmp2);
    strncpy(cTmp2,cTmp1+6,2);cTmp2[2] = 0;iDay = atoi(cTmp2);
    strncpy(cTmp2,cTmp1+8,2);cTmp2[2] = 0;iHour = atoi(cTmp2);
    strncpy(cTmp2,cTmp1+10,2);cTmp2[2] = 0;iMinute = atoi(cTmp2);
    strncpy(cTmp2,cTmp1+12,2);cTmp2[2] = 0;iSecond = atoi(cTmp2);
    
    minuendDateTime.setDateTime(iYear,iMonth,iDay,iHour,iMinute,iSecond);
	
	//算出时间差
	sprintf(cDuration,"%d",subtrahendDateTime.secondDiff(&minuendDateTime));
	
    m_pFieldTarget->setDesValue(cDuration);
	
    return true;
}

void FieldConvertRule_18::getFieldObj()
{
    m_pFieldSubtrahend = m_pFieldInfos->getFieldInfoByName(m_TimeSubtrahend.c_str());  
    m_pFieldMinuend    = m_pFieldInfos->getFieldInfoByName(m_TimeMinuend.c_str());
    m_pFieldTarget     = m_pFieldInfos->getFieldInfoByName(m_SecondDiffName.c_str());

	return FieldConvertRule::getFieldObj();		
}
