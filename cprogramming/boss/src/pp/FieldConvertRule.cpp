// ##########################################
// Source file : FieldConvertRule.h
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080903
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldConvertRule.h"
#include "base/supdebug.h"
#include "FieldInfos.h"

//##ModelId=48561297034B
FieldConvertRule::FieldConvertRule(const int iSubChannelNum)
{
    m_PpLog   = PpLog::getInstance();
    m_CdrFlag = CdrFlag::getInstance();
    m_pFieldInfos = FieldInfos::getInstance(iSubChannelNum);
}

//##ModelId=4856129A032C
FieldConvertRule::~FieldConvertRule()
{
}

//##ModelId=485613D9006D
int FieldConvertRule::strnrps(const char *src, const char *token, const char *rps, char *des, int max)
{
    int l1;
	char *p=0;
	char *s3;
	char *s2=des;
	char *s1=(char *)src;

    l1 = strlen(token);
	if (!l1)
		return 0;

	while((p=strstr(s1,token))!='\0')
	{
		for (;s1<p;*s2++=*s1++,max--)
		{
			if (!max) 	
			{
				*s2='\0';
	         	return s2-des;
			}
		}
		p+=strlen(token);
		s1=p;
		for (s3=(char *)rps;*s3 !='\0';*s2++=*s3++,max--)
		{
			if (!max) 	
			{
				*s2='\0';
	         	return s2-des;
			}
		}
	}

	for(;(*s2=*s1)!='\0';s2++,s1++,max--)
	{
		if (!max) 	
		{
			*s2='\0';
		    return s2-des;
		}
	}

	return s2-des;  
}

//##ModelId=485614260261
//##用于根据开始时间和被修改的时长，重新计算出结束时间
void FieldConvertRule::editTime(int iDuration)
{
    char tmp[10];
    int iStartYear,iStartMonth,iStartDay,iStartHour,iStartMinute,iStartSecond;
    DateTime endDateTime;

    if( m_pFieldStartDate->m_iDesValueLen <8 )
		return ;

	if( m_pFieldStartTime->m_iDesValueLen<6 )
		return ;

    strncpy(tmp,m_pFieldStartDate->m_FieldDesValue,4);
    tmp[4]=0;
    iStartYear=atoi(tmp);
    strncpy(tmp,m_pFieldStartDate->m_FieldDesValue+4,2);
    tmp[2]=0;
    iStartMonth=atoi(tmp);
    strncpy(tmp,m_pFieldStartDate->m_FieldDesValue+6,2);
    tmp[2]=0;
    iStartDay=atoi(tmp);

    strncpy(tmp,m_pFieldStartTime->m_FieldDesValue,2);
    tmp[2]=0;
    iStartHour=atoi(tmp);
    strncpy(tmp,m_pFieldStartTime->m_FieldDesValue+2,2);
    tmp[2]=0;
    iStartMinute=atoi(tmp);
    strncpy(tmp,m_pFieldStartTime->m_FieldDesValue+4,2);
    tmp[2]=0;
    iStartSecond=atoi(tmp);

    if(!endDateTime.setDateTime(iStartYear,iStartMonth,iStartDay,iStartHour,iStartMinute,iStartSecond)) 
    {
		return ;
    }
	endDateTime.addSecond( iDuration );

    m_pFieldEndDate->setDesValue(endDateTime.getDate());
	m_pFieldEndTime->setDesValue(endDateTime.getTime());

	if( m_CdrFlag->m_errorCode == INVALID_DATE_TIME )
	{
		m_CdrFlag->clear();	
	} 
}

void FieldConvertRule::getFieldObj()
{
	m_pFieldCallType     = m_pFieldInfos->getFieldInfoByName(CALL_TYPE);
	m_pFieldStartDate    = m_pFieldInfos->getFieldInfoByName(START_DATE);
    m_pFieldStartTime    = m_pFieldInfos->getFieldInfoByName(START_TIME);
	m_pFieldEndDate      = m_pFieldInfos->getFieldInfoByName(END_DATE);
	m_pFieldEndTime      = m_pFieldInfos->getFieldInfoByName(END_TIME);
	
	return;
}

void FieldConvertRule::getFieldSequnceByName(const vector<string> &fieldNames
    											,vector<int> &fieldSequnces)
{
    int i;
    fieldSequnces.clear();
    for(i=0;i<fieldNames.size();++i)
    {
    	fieldSequnces.push_back(m_pFieldInfos->getSequnceByName(fieldNames[i].c_str()));
    }	
	;
}

bool FieldConvertRule::generalCondJudge()							
{
    // 条件1：通话类型
    int l_field,l_value;
    
    if(m_callType.compare(CONDITION_ALL)!=0) 
    {
        if( m_pFieldCallType->m_iDesValueLen == 0
        	||strcmp(m_pFieldCallType->m_FieldDesValue,m_callType.c_str())!=0 ) 
        {
            return false;
        }
    }

    // 条件2：其他字段值
	if(m_conditionFieldName.compare(CONDITION_ALL)!=0)                           // not is "ALL"   
	{           
	    if( m_pFieldCondition->m_iDesValueLen==0 )
	    {
			return false;
		}
		//字段值是"TAG_ALL" added by zaiww 20090925
		//针对只出现tag码,且valueSize为0的情况判断,区别于("=ALL"表示该字段有值且valueSize不为0)
		if(m_conditionFieldValue.compare(TAG_ALL)==0)
		{
			//TAG_ALL关键字前必须配置等号(例：=TAG_ALL)
			if(m_conditionExp.compare("=")!=0)
			{
				theErrorMessages->insert(PP_ERROR_CONFIG_FILE, "convert rule_5 error! ");
				return false;
			}
			if(m_pFieldCondition->m_tagAll==false)
			{
				return false;
			}
		}
		else
		{
			if(m_conditionFieldValue.compare(CONDITION_ALL)!=0) // 字段值不是"ALL"
			{                                                 
			    if(m_conditionExp.compare("!=")==0) 
			    {
			        if( strcmp(m_pFieldCondition->m_FieldDesValue,m_conditionFieldValue.c_str())==0 ) 
			        {
    		            return false;
    		        }
			    } 
			    else if(m_conditionExp.compare("=")==0) 
			    {
			        if( strcmp(m_pFieldCondition->m_FieldDesValue,m_conditionFieldValue.c_str())!=0 ) 
			        {
    		            return false;
    		        }
			    }
			    else if(m_conditionExp.compare(">=") == 0)
			    {
			        l_field=atoi( m_pFieldCondition->m_FieldDesValue );
			        l_value=atoi( m_conditionFieldValue.c_str() );
			        if( l_field < l_value ) 
			        {
			            return false;
			        }			        
			    } 
			    else if(m_conditionExp.compare("<=") == 0)
			    {
			        l_field=atoi( m_pFieldCondition->m_FieldDesValue );
			        l_value=atoi( m_conditionFieldValue.c_str() );
			        if( l_field > l_value ) 
			        {
			            return false;
			        }			        
			    }			     
			    else 
			    {
			        return false;
			    }
    		} 
    		else                                                    // 字段值是"ALL"
    		{                                                    
    		    if(m_conditionExp.compare("!=")==0)         // 如果字段没有值(!=ALL),即为空
    		    {                                               
    		        if(m_pFieldCondition->m_iDesValueLen>0)      // 有值，不满足条件
    		        {
    		            return false;                              
    		        }                          
    		    } 
    		    else if(m_conditionExp.compare("=")==0)     // 如果字段有值(=ALL),即不为空
    		    {                
    		        if(m_pFieldCondition->m_iDesValueLen<=0)
    		        {
    		            return false;
    		        }                            // 没有值，不满足条件
    		    } 
    		    else 
    		    {
    		        return false;
    		    }
    		}
    	}
    }
    return true;	
}

string FieldConvertRule::lrTrim(string theString)
{
    int len,size;

	size	= theString.size();
	len		= theString.find_first_not_of(" \t");
	if(len>0)
		theString	= theString.substr(len,size-len);	// 去掉头部空格和Tab

	len		= theString.find_last_not_of(" \t");
	if(len>0)
		theString	= theString.substr(0,len+1);		   // 去掉尾部空格和Tab

	return theString;
}
