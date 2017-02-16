// ##########################################
// Source file : FieldValidateRule_5.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080924
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################


//##########################################
//##此校验规则用于统计交换机所下话单的时效性
//##########################################

#include "FieldValidateRule_5.h"
#include "FieldInfos.h"

//##ModelId=4856465D008C
bool FieldValidateRule_5::initialize(string ruleText)
{
    vector<string> tmpStrs,tmpStrs1;
    
    m_iRange1 = 0;
	m_iRange2 = 0;
	m_iRange3 = 0;
	m_iRange4 = 0;
	m_iRange5 = 0;
	m_iRange6 = 0;
    
    // example:	rule_5:VALID_TYPE=fields;DATE=START_DATE;TIME=START_TIME;2,4,8,24,72,360
  //  splitStrByToken(ruleText,";",tmpStrs);
	StringUtil::split(ruleText,";",tmpStrs);
		
	if( tmpStrs.size() != 4)
	{
		// 小于4个分号的,不符合规则的书写格式
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
        m_checkDate = tmpStrs1[1];        //get "START_ DATE"
    }
    //splitStrByToken(tmpStrs[2],"=",tmpStrs1);
    tmpStrs1.clear();StringUtil::split(tmpStrs[2],"=",tmpStrs1);

    if (tmpStrs1.size() != 2)
    {
        return false;
    }
    else
    {
        m_checkTime = tmpStrs1[1];         //get "START_TIME"
    }
 
    //get the rest :"2,4,8,24,72,360" 
    //splitStrByToken(tmpStrs[3],",",tmpStrs1);
    tmpStrs1.clear();StringUtil::split(tmpStrs[3],",",tmpStrs1);

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

//##ModelId=48564664000F
int FieldValidateRule_5::validField()
{
    time_t tDealTime,tStarTime;
    struct tm tmStarTime;
    FieldInfo *aFieldInfo=0;
    long dTimeDiff;
    char cTmp[8+1],cTmp1[4+1];
    static string START_DATE("START_DATE"),DURATION("DURATION"),CALL_TYPE("CALL_TYPE"),t1,t2,t3;  
    
    //如果是trash话单，则不统计(包括‘305’,'999'错单)
   //	aFieldInfo =m_pFieldInfos->getFieldInfoByName(CALL_TYPE );
   // if( (aFieldInfo=getFieldMap(theFieldInfoMaps,CALL_TYPE))==0 )
    if((aFieldInfo=m_pFieldInfos->getFieldInfoByName(CALL_TYPE.c_str()))==0)
    {
    	return 0;
    }
    else
    {   
    	t3=aFieldInfo->m_FieldDesValue;
    	if( atoi(t3.c_str()) == 0 )
    	{
    			return 0;	
    	} 	
    }
    
    
    //若时间出错,则直接统计
    if( m_cdrFlag->m_fieldName.compare(START_DATE) == 0 
    	|| m_cdrFlag->m_fieldName.compare(DURATION) == 0 )
    {
    	m_PpLog_->addDelaySum8();
    	return 0;	
    }
    
    //if( (aFieldInfo=getFieldMap(theFieldInfoMaps,m_checkDate))==0 )
   	if((aFieldInfo =m_pFieldInfos->getFieldInfoByName( m_checkDate.c_str()))==0)

    {  
	    return 0;
	}
	 	
	//取得年月日
	memset(cTmp,0,sizeof(cTmp) );
	//aFieldInfo =m_pFieldInfos->getFieldInfoByName( m_checkDate.c_str());
	t1=aFieldInfo->m_FieldDesValue;
	strncpy( cTmp,t1.c_str(),8 );
	
	strncpy( cTmp1, cTmp ,4);
	cTmp1[4] = 0;
	tmStarTime.tm_year = atoi( cTmp1 ) - 1900;
	
	strncpy( cTmp1, cTmp+4 ,2);
	cTmp1[2] = 0;
	tmStarTime.tm_mon  = atoi( cTmp1 ) - 1;
	
	strncpy( cTmp1, cTmp+6 ,2);
	cTmp1[2] = 0;
	tmStarTime.tm_mday = atoi( cTmp1 );
		 	
	//if( (aFieldInfo=getFieldMap(theFieldInfoMaps,m_checkTime))==0 ) 
	if((aFieldInfo =m_pFieldInfos->getFieldInfoByName( m_checkTime.c_str()))==0)
 
	{
	    return 0;
	}
	
	//取得时分秒 	
	memset(cTmp,0,sizeof(cTmp) );
	//aFieldInfo =m_pFieldInfos->getFieldInfoByName( m_checkTime.c_str());
	t2=aFieldInfo->m_FieldDesValue;
	strncpy( cTmp,t2.c_str(),6 );
	strncpy( cTmp1, cTmp ,2);
	cTmp1[2] = 0; 
	tmStarTime.tm_hour = atoi( cTmp1 );
	
	strncpy( cTmp1, cTmp+2 ,2);
	cTmp1[2] = 0; 
	tmStarTime.tm_min  = atoi( cTmp1 );
	
	strncpy( cTmp1, cTmp+4 ,2);
	cTmp1[2] = 0; 
	tmStarTime.tm_sec  = atoi( cTmp1 );	
    
    tDealTime = m_PpLog_->getStartDateTime_tm();
    
    tStarTime = mktime(&tmStarTime) ;
    
    dTimeDiff = tDealTime - tStarTime;
    
    if( dTimeDiff < 0 )
    {
		m_PpLog_->addDelaySum9();
	}
	else if( dTimeDiff/3600 < m_iRange1 )
	{
		m_PpLog_->addDelaySum1();	
	}
	else if( dTimeDiff/3600 < m_iRange2 )
	{
		m_PpLog_->addDelaySum2();	
	}
	else if( dTimeDiff/3600 < m_iRange3 )
	{
		m_PpLog_->addDelaySum3();	
	}
	else if( dTimeDiff/3600 < m_iRange4 )
	{
		m_PpLog_->addDelaySum4();	
	}
	else if( dTimeDiff/3600 < m_iRange5 )
	{
		m_PpLog_->addDelaySum5();	
	}
	else if( dTimeDiff/3600 < m_iRange6 )
	{
		m_PpLog_->addDelaySum6();	
	}
	else 
	{
		m_PpLog_->addDelaySum7();	
	}    
    
    return 0;
}

