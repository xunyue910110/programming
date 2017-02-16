// ##########################################
// Source file : FieldValidateRule_7.cpp
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

#include "FieldValidateRule_7.h"
#include "FieldInfos.h"
#include "AlertCode.h"
#include <unistd.h>
#include <sys/stat.h>

//##########################################
//## 此校验规则:用于比较输入字段和当前处理时间的差值在阀值(以小时为单位)之内，否则判错单
//##########################################

//##ModelId=485647040232
bool FieldValidateRule_7::initialize(string ruleText)
{
    vector<string> tmpStrs,tmpStrs1;
    
    m_iMaxTime = 0;
    m_iMinTime = 0;    
    // example:	rule_7:VALID_TYPE=fields;DATE=START_DATE;TIME=START_TIME;-90,1
    // example: rule_7:VALID_TYPE=fields;DATE=START_DATE;TIME=START_TIME;LAST_7,1
    // example: rule_7:VALID_TYPE=fields;DATE=START_DATE;TIME=START_TIME;FOR_40,1
    //splitStrByToken(ruleText,";",tmpStrs);
	  StringUtil::split(ruleText,";",tmpStrs);

	if( tmpStrs.size() != 4 )
	{
		// 小于3个分号的,不符合规则的书写格式
		return false;	
	}
	
	      //splitStrByToken(tmpStrs[1],"=",tmpStrs1);
    tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);
    m_checkDate = tmpStrs1[1];                //  get "START_DATE"   
    //splitStrByToken(tmpStrs[2],"=",tmpStrs1); 
    tmpStrs1.clear();StringUtil::split(tmpStrs[2],"=",tmpStrs1);

    m_checkTime = tmpStrs1[1];               // get "START_TIME"
    
    //get the rest :"-90,1" 
    //splitStrByToken(tmpStrs[3],",",tmpStrs1);
    tmpStrs1.clear();StringUtil::split(tmpStrs[3],",",tmpStrs1);

    if( tmpStrs1.size() == 2 )
    {  
        if( (tmpStrs1[0].compare("LAST_7")==0) || (tmpStrs1[0].compare(0,3,"FOR")==0) )
        {
            m_Exp=tmpStrs1[0];
        }
        else
        {
           m_iMinTime = atoi(tmpStrs1[0].c_str());
           m_Exp = "NULL";  
        }
        m_iMaxTime = atoi(tmpStrs1[1].c_str());  	
    }
    else
    {
    	return false;	
    }
    
    return true;
}

//##ModelId=4856470B0196
int FieldValidateRule_7::validField()
{
    time_t tDealTime,tStarTime;
    struct tm tmStarTime,*tmpTime=0;
    string tmpStr;
    FieldInfo *aFieldInfo=0;
    long dTimeDiff;
    int day,hour,total, num;
    char cTmp[8+1],cTmp1[4+1];
    static string START_DATE("START_DATE"),DURATION("DURATION"),t1,t2;  
    
    //若时间出错,则直接统计
    if( m_cdrFlag->m_fieldName.compare(START_DATE) == 0 
    	|| m_cdrFlag->m_fieldName.compare(DURATION) == 0 )
    {
    	return INVALID_DATE_TIME;	
    }
    
    //if( (aFieldInfo=getFieldMap(theFieldInfoMaps,m_checkDate))==0 ) 
   	if((aFieldInfo =m_pFieldInfos->getFieldInfoByName( m_checkDate.c_str()))==0)
 
    {
	    return INVALID_DATE_TIME;
	}
	 	
	//取得年月日
	memset(cTmp,0,sizeof(cTmp) );
		aFieldInfo =m_pFieldInfos->getFieldInfoByName( m_checkDate.c_str());
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
	    return INVALID_DATE_TIME;
	}
	
	//取得时分秒 	
	memset(cTmp,0,sizeof(cTmp) );
		aFieldInfo =m_pFieldInfos->getFieldInfoByName( m_checkTime.c_str());
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
    
    if(m_Exp.compare("LAST_7")==0)
    {
        tmpTime = localtime(&tDealTime);
    
        day = tmpTime->tm_mday;
        hour = tmpTime->tm_hour;
    
        total = day*24+hour+7*24;    
        
        m_iMinTime = -total; 
    }
    else if (m_Exp.compare(0,3,"FOR")==0)
    {
        num=m_Exp.find("_");
        if (num==string::npos)
        {
            return false;
        }
        else
        {
            tmpStr=m_Exp.substr(num+1);
            num  = 24 * atoi(tmpStr.c_str());
            m_iMinTime = -num;    
        }
    }
    
    tStarTime = mktime(&tmStarTime) ;
    
    dTimeDiff = tStarTime - tDealTime;
    
    if( (dTimeDiff < (m_iMinTime*3600))
    	||(dTimeDiff > (m_iMaxTime*3600)) )
    {
        if (m_Exp.compare(0,3,"FOR") == 0)
        {
            theErrorMessages->insert(PP_ERROR_TIME_OUT,"Src CDR Time  Out  Of ("+tmpStr+") error!"); 
        }
		return INVALID_TIME_DELAY;
	}
    
    return 0;
}

