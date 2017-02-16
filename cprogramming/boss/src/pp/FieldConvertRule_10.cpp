// ##########################################
// Source file : FieldConvertRule_10.cpp
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

#include "FieldConvertRule_10.h"
#include "FieldInfos.h"


//##ModelId=4856197A0177
//##此转换用于把以秒为单位的时间转换为时分秒表示的时间值
bool FieldConvertRule_10::initialize(string ruleText)
{
    vector<string> tmpStrs,tmpStrs1;
	
    //RULE_10:CALL_TYPE=00;19940101;startSend,endSend
	tmpStrs.clear();StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 3)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}

	// 提取规则中的信息
	tmpStrs[0] = tmpStrs[0].substr(8);
	
    if(tmpStrs[0].compare(0,9,CALL_TYPE) == 0)
	{
	    tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	    m_callType = tmpStrs1[1];
	}
	else
	{
	    return false;
	}
	
	if ( tmpStrs[1].size() >= 8 )
	{
	    int iYear,iMonth,iDay;
	    iYear=iMonth=iDay=0;
	    iYear = atoi(tmpStrs[1].substr(0,4).c_str());
	    iMonth = atoi(tmpStrs[1].substr(4,2).c_str());
	    iDay = atoi(tmpStrs[1].substr(6,2).c_str());
	    m_baseDateTime.setDateTime(iYear,iMonth,iDay,0,0,0);
	}
	else
	{
	    return false;
	}
	
	   
	// 得到需要转换的时间值
	tmpStrs1.clear();StringUtil::split(tmpStrs[2],",",tmpStrs1); 	
	if( tmpStrs1.size() == 2 )
	{
	    m_startFieldName = tmpStrs1[0];
	    m_endFieldName   = tmpStrs1[1];
	}
	else
	{
		return false;	
	}	
	this->getFieldObj();

    return true;
}

//##ModelId=4856198000AB
bool FieldConvertRule_10::convertField()
{
    int iStartDuration,iEndDuration;
    DateTime startDateTime,endDateTime;
    char cDuration[6];

    if( m_pFieldCallType->m_iDesValueLen == 0 )
    {
		return false;
	}

    // 条件1：通话类型
    if(m_callType.compare(CONDITION_ALL)!=0) 
    {
       if( strcmp(m_pFieldCallType->m_FieldDesValue,m_callType.c_str() )!=0) 
       {
          return false;
       }
    }

    if( m_pFieldStartDu->m_iDesValueLen==0 
    	||m_pFieldEndDu->m_iDesValueLen==0  )
	{
		m_CdrFlag->m_isError       = true;
	    m_CdrFlag->m_fieldName     = m_startFieldName;
	    m_CdrFlag->m_errorCode     = INVALID_DATE_TIME;
		
		return false;	
	}
	
	iStartDuration = atoi(m_pFieldStartDu->m_FieldDesValue);
	iEndDuration   = atoi(m_pFieldEndDu->m_FieldDesValue);
	
	// 设置时间值为基础时间值	
	startDateTime = endDateTime = m_baseDateTime;
	
	startDateTime.addSecond(iStartDuration);
	endDateTime.addSecond(iEndDuration);
	
	// 根据开始时间和结束时间算出通话时长
	sprintf(cDuration,"%d",endDateTime.secondDiff(&startDateTime));
	
	// 插入字段
	m_pFieldStartDate->setDesValue(startDateTime.getDate());
	m_pFieldStartTime->setDesValue(startDateTime.getTime());
	m_pFieldEndDate->setDesValue(endDateTime.getDate());
	m_pFieldEndTime->setDesValue(endDateTime.getTime());
	m_pFieldDu->setDesValue(cDuration);    

    return true;
}



//##ModelId=4856198902FD
FieldConvertRule_10::~FieldConvertRule_10()
{
}

void FieldConvertRule_10::getFieldObj()
{
	m_pFieldDu      = m_pFieldInfos->getFieldInfoByName(DURATION);
	m_pFieldStartDu = m_pFieldInfos->getFieldInfoByName(m_startFieldName.c_str());
	m_pFieldEndDu   = m_pFieldInfos->getFieldInfoByName(m_endFieldName.c_str());	
	
	return FieldConvertRule::getFieldObj();	
}
