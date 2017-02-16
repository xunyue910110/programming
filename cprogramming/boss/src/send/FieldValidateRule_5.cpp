// ##########################################
// Source file : FieldValidateRule_5.cpp
// System      : Mobile Billing System
// Version     : 0.1
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : min chen
// E_mail      : chenm@lianchuang.com
// Create      : 20020602
// Update      : 20020602
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldValidateRule_5.h"

//##########################################
//##此校验规则用于统计交换机所下话单的时效性
//##########################################

bool FieldValidateRule_5::initialize(string ruleText)
{
	vector<string> tmpStrs,tmpStrs1;
    
    m_iRange1 = 0;
	m_iRange2 = 0;
	m_iRange3 = 0;
	m_iRange4 = 0;
	m_iRange5 = 0;
	m_iRange6 = 0;
    
    // example:	rule_5;fields;START_DATE;START_TIME;2,4,8,24,72,360
    splitStrByToken(ruleText,";",tmpStrs);

	if( tmpStrs.size() != 5 )
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}

    m_checkDate = tmpStrs[2];      // skip "rule_5" and "fields" is "START_DATE"     
    m_checkTime = tmpStrs[3];      // get "START_TIME"
    
    //get the rest :"2,4,8,24,72,360" 
    splitStrByToken(tmpStrs[4],",",tmpStrs1);
    if( tmpStrs1.size() == 6 )
    {
	    m_iRange1 = atoi(tmpStrs1[0].c_str());
		m_iRange2 = atoi(tmpStrs1[1].c_str());
		m_iRange3 = atoi(tmpStrs1[2].c_str());
		m_iRange4 = atoi(tmpStrs1[3].c_str());
		m_iRange5 = atoi(tmpStrs1[4].c_str());
		m_iRange6 = atoi(tmpStrs1[5].c_str());    	
    }
    else
    {
    	return false;	
    }
    
    return true;
}

int FieldValidateRule_5::validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    time_t tDealTime,tStarTime;
    struct tm tmStarTime;
    FieldInfo *aFieldInfo=0;
    long dTimeDiff;
    char cTmp[8+1],cTmp1[4+1];
    static string START_DATE("START_DATE"),DURATION("DURATION"),CALL_TYPE("CALL_TYPE");  
    
    //如果是trash话单，则不统计(包括‘305’,'999'错单)
    if( (aFieldInfo=getFieldMap(theFieldInfoMaps,CALL_TYPE))==0 )
    {
    	return 0;
    }
    else
    {
    	if( atoi(aFieldInfo->m_FieldDesValue.c_str()) == 0 )
    	{
    			return 0;	
    	} 	
    }
    
    
    //若时间出错,则直接统计
    if( m_cdrFlag->m_fieldName.compare(START_DATE) == 0 
    	|| m_cdrFlag->m_fieldName.compare(DURATION) == 0 )
    {
    	m_SendLog_->addDelaySum8();
    	return 0;	
    }
    
    if( (aFieldInfo=getFieldMap(theFieldInfoMaps,m_checkDate))==0 )  
	 	return 0;
	 	
	//取得年月日
	memset(cTmp,0,sizeof(cTmp) );
	strncpy( cTmp,(aFieldInfo->m_FieldDesValue).c_str(),8 );
	
	strncpy( cTmp1, cTmp ,4);
	cTmp1[4] = 0;
	tmStarTime.tm_year = atoi( cTmp1 ) - 1900;
	
	strncpy( cTmp1, cTmp+4 ,2);
	cTmp1[2] = 0;
	tmStarTime.tm_mon  = atoi( cTmp1 ) - 1;
	
	strncpy( cTmp1, cTmp+6 ,2);
	cTmp1[2] = 0;
	tmStarTime.tm_mday = atoi( cTmp1 );
		 	
	if( (aFieldInfo=getFieldMap(theFieldInfoMaps,m_checkTime))==0 )  
	 	return 0;
	
	//取得时分秒 	
	memset(cTmp,0,sizeof(cTmp) );
	strncpy( cTmp,(aFieldInfo->m_FieldDesValue).c_str(),6 );
	strncpy( cTmp1, cTmp ,2);
	cTmp1[2] = 0; 
	tmStarTime.tm_hour = atoi( cTmp1 );
	
	strncpy( cTmp1, cTmp+2 ,2);
	cTmp1[2] = 0; 
	tmStarTime.tm_min  = atoi( cTmp1 );
	
	strncpy( cTmp1, cTmp+4 ,2);
	cTmp1[2] = 0; 
	tmStarTime.tm_sec  = atoi( cTmp1 );	
    
    tDealTime = m_SendLog_->getStartDateTime_tm();
    
    tStarTime = mktime(&tmStarTime) ;
    
    dTimeDiff = tDealTime - tStarTime;
    
    if( dTimeDiff < 0 )
    {
		m_SendLog_->addDelaySum9();
	}
	else if( dTimeDiff/3600 < m_iRange1 )
	{
		m_SendLog_->addDelaySum1();	
	}
	else if( dTimeDiff/3600 < m_iRange2 )
	{
		m_SendLog_->addDelaySum2();	
	}
	else if( dTimeDiff/3600 < m_iRange3 )
	{
		m_SendLog_->addDelaySum3();	
	}
	else if( dTimeDiff/3600 < m_iRange4 )
	{
		m_SendLog_->addDelaySum4();	
	}
	else if( dTimeDiff/3600 < m_iRange5 )
	{
		m_SendLog_->addDelaySum5();	
	}
	else if( dTimeDiff/3600 < m_iRange6 )
	{
		m_SendLog_->addDelaySum6();	
	}
	else 
	{
		m_SendLog_->addDelaySum7();	
	}    
    
    return 0;
}

