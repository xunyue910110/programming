// ##########################################
// Source file : FieldValidateRule_7.cpp
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

#include "FieldValidateRule_7.h"

//##########################################
//## 此校验规则:用于比较输入字段和当前处理时间的差值在阀值(以小时为单位)之内，否则判错单
//##########################################

bool FieldValidateRule_7::initialize(string ruleText)
{
	vector<string> tmpStrs,tmpStrs1;
    
    m_iMaxTime = 0;
    m_iMinTime = 0;    
    // example:	rule_7;fields;START_DATE;START_TIME;-90,1
    splitStrByToken(ruleText,";",tmpStrs);

	if( tmpStrs.size() != 5 )
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	
    m_checkDate = tmpStrs[2];      // skip "rule_7" and "fields" is "START_DATE"     
    m_checkTime = tmpStrs[3];      // get "START_TIME"
    
    //get the rest :"-90,1" 
    splitStrByToken(tmpStrs[4],",",tmpStrs1);
    if( tmpStrs1.size() == 2 )
    {
	    m_iMinTime = atoi(tmpStrs1[0].c_str());
		m_iMaxTime = atoi(tmpStrs1[1].c_str());  	
    }
    else
    {
    	return false;	
    }
    
    return true;
}

int FieldValidateRule_7::validField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    time_t tDealTime,tStarTime;
    struct tm tmStarTime;
    FieldInfo *aFieldInfo=0;
    long dTimeDiff;
    char cTmp[8+1],cTmp1[4+1];
    static string START_DATE("START_DATE"),DURATION("DURATION");  
    
    //若时间出错,则直接统计
    if( m_cdrFlag->m_fieldName.compare(START_DATE) == 0 
    	|| m_cdrFlag->m_fieldName.compare(DURATION) == 0 )
    {
    	return INVALID_DATE_TIME;	
    }
    
    if( (aFieldInfo=getFieldMap(theFieldInfoMaps,m_checkDate))==0 )  
	 	return INVALID_DATE_TIME;
	 	
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
	 	return INVALID_DATE_TIME;
	
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
    
    tDealTime = m_RetLog_->getStartDateTime_tm();
    
    tStarTime = mktime(&tmStarTime) ;
    
    dTimeDiff = tStarTime - tDealTime;
    
    if( (dTimeDiff < (m_iMinTime*3600))
    	||(dTimeDiff > (m_iMaxTime*3600)) )
    {
		return INVALID_TIME_DELAY;
	}
    
    return 0;
}

