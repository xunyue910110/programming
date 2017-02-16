// ##########################################
// Source file : FieldConvertRule_11.cpp
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

#include <stdlib.h>

#include "FieldConvertRule_11.h"
#include "RepeatableFieldInfo.h"
#include "FieldInfos.h"


//##ModelId=48561A510251
//##提供gprs流量链表中的上下行流量和时长进行累计
bool FieldConvertRule_11::initialize(string ruleText)
{
    string strTmp;
	vector<string> tmpStrs,tmpStrs1;

    //RULE_11:CALL_TYPE=01;FIELD=changeTime;DATA_UP=sum(dataVolumeGPRSUp),DATA_DOWN=sum(dataVolumeGPRSDn),DURAT=difftime(changeTime)
	StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() < 2)
	{
		// 小于1个分号的,不符合规则的书写格式
		return false;	
	}
	
	// 提取规则中的信息
	if (tmpStrs.size() == 2)
	{
	    tmpStrs[0] = tmpStrs[0].substr(8);
	    if (tmpStrs[0].compare(0,5,"FIELD") == 0)
	    {
	        m_callType = "ALL";
	        
	        tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	        if (tmpStrs1.size() != 2)
	        {
	            return false;
	        }
	        else
	        {
	            m_sTimeFieldName = tmpStrs1[1];
	        }
	    }
	    else
	    {
	        return false;
	    }
	    
	    // 得到规则11中的具体操作部分:DATA_UP=sum(dataVolumeGPRSUp),DATA_DOWN=sum(dataVolumeGPRSDn),DURAT=difftime(changeTime)
	    tmpStrs1.clear();StringUtil::split(tmpStrs[1],",",tmpStrs1);
	    if( tmpStrs1.size() != 3 )
        {
            return false;
        }
	    if( !getFields(tmpStrs1[0],m_totalDtUpName,m_srcDtUpName)
	        || !getFields(tmpStrs1[1],m_totalDtDnName,m_srcDtDnName) 
	        || !getFields(tmpStrs1[2],m_strDuratName,strTmp)  )
	    {
		    return false;	
	    }
	    this->getFieldObj();
	    return true;
    }
    else if(tmpStrs.size()==3)
    {
        tmpStrs[0] = tmpStrs[0].substr(8);
        
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
            m_sTimeFieldName = tmpStrs1[1];
        }
        else
        {
            return false;
        }
        
        // 得到规则11中的具体操作部分:DATA_UP=sum(dataVolumeGPRSUp),DATA_DOWN=sum(dataVolumeGPRSDn),DURAT=difftime(changeTime)
	    tmpStrs1.clear();StringUtil::split(tmpStrs[2],",",tmpStrs1);
	    if( tmpStrs1.size() != 3 )
        {
            return false;
        }
	    if( !getFields(tmpStrs1[0],m_totalDtUpName,m_srcDtUpName)
	        || !getFields(tmpStrs1[1],m_totalDtDnName,m_srcDtDnName) 
	        || !getFields(tmpStrs1[2],m_strDuratName,strTmp)  )
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


//##ModelId=48561A570261
bool FieldConvertRule_11::convertField()
{
     char cDateTime[13];
	
	// 条件1：通话类型
    if(m_callType.compare(CONDITION_ALL)!=0) 
    {
        if( m_pFieldCallType->m_iDesValueLen == 0
        	||strcmp(m_pFieldCallType->m_FieldDesValue,m_callType.c_str())!=0 ) 
        {
            return false;
        }
    }

	// 从字段容器中提取时间、上下行流量、时长 的值
	if(  (m_pStartTimes->getNumCount() == m_pDataFlowUps->getNumCount())
	     && (m_pStartTimes->getNumCount() == m_pDataFlowDns->getNumCount())  )
	{
		// 流量、时长计数器清零
		int iDataUp1,iDataDown1,iDataUp2,iDataDown2;
		long iDuration1,iDuration2;
		iDataUp1=iDataDown1=iDataUp2=iDataDown2=0;
		iDuration1=iDuration2=0;
		
		// 根据时段统计流量和时长
		for( int i=0;i<m_pStartTimes->getNumCount();++i)
		{
			if(i ==0)
			{
				// 得到opendate,作为第一段流量的起始时间,用以判断流量发生在忙时还是闲时
				if( m_pFieldStartDate->m_iDesValueLen == 0
					|| m_pFieldStartTime->m_iDesValueLen == 0 )
				{
					return false;	
				}
				memcpy(cDateTime,m_pFieldStartDate->m_FieldDesValue+2,6);
				memcpy(cDateTime+6,m_pFieldStartTime->m_FieldDesValue,6);
			}
			else
			{
				memcpy(cDateTime,m_pStartTimes->m_fieldInfoVect[i]->m_FieldDesValue,6);
			}
			if( (strcmp(cDateTime+6,"230000") >= 0 && strcmp(cDateTime+6,"240000") <= 0 )
			  || (strcmp(cDateTime+6,"000000") >= 0 && strcmp(cDateTime+6,"065959") <= 0))
			{
				iDataUp2   += atoi( m_pDataFlowUps->m_fieldInfoVect[i]->m_FieldDesValue );
				iDataDown2 += atoi( m_pDataFlowDns->m_fieldInfoVect[i]->m_FieldDesValue );
				iDuration2 += diffSecond(string("20")+cDateTime,string("20")+m_pStartTimes->m_fieldInfoVect[i]->m_FieldDesValue);
			}   
			else
			{
				iDataUp1   += atoi( m_pDataFlowUps->m_fieldInfoVect[i]->m_FieldDesValue ); 
				iDataDown1 += atoi( m_pDataFlowDns->m_fieldInfoVect[i]->m_FieldDesValue );
				iDuration1 += diffSecond(string("20")+cDateTime,string("20")+m_pStartTimes->m_fieldInfoVect[i]->m_FieldDesValue);
			}
		}
		
		m_pFieldDateUp1->setDesValue( StringUtil::toString(iDataUp1).c_str() );
		m_pFieldDateUp2->setDesValue( StringUtil::toString(iDataUp2).c_str() );
		m_pFieldDateDn1->setDesValue( StringUtil::toString(iDataDown1).c_str() );
		m_pFieldDateDn2->setDesValue( StringUtil::toString(iDataDown2).c_str() );
		m_pFieldDurat1->setDesValue ( StringUtil::toString(iDuration1).c_str() );
		m_pFieldDurat1->setDesValue ( StringUtil::toString(iDuration2).c_str() );
		    
		if( iDataUp1==0 && iDataUp2 == 0 && iDataDown1 == 0 && iDataDown2 == 0)
		{
			m_CdrFlag->m_errorCode = GPRS_DATA_NULL;
			m_CdrFlag->m_isError   = true;
			m_CdrFlag->m_fieldName = m_totalDtUpName + "1";
		}
	}
	else
	{
		m_CdrFlag->m_errorCode = INVALID_DATE_TIME;
		m_CdrFlag->m_isError   = true;
		m_CdrFlag->m_fieldName = m_totalDtUpName + "1";		
	}
	
	return true;
}

//##ModelId=48561A620222
FieldConvertRule_11::~FieldConvertRule_11()
{
}

//##ModelId=48561A72003E
bool FieldConvertRule_11::getFields(const string &ruleStr, string &totalStr, string &srcStr)
{
    vector<string> tmpStrs;
    int ndx, len;
    
    StringUtil::split(ruleStr,"=",tmpStrs);
    if( tmpStrs.size() == 2 )
    {
        totalStr = tmpStrs[0];
        len = tmpStrs[1].size();
        if( ( ndx = tmpStrs[1].find("(") ) > 0 )
		{
			len = tmpStrs[1].size();
			tmpStrs[1] = lrTrim( tmpStrs[1].substr(ndx+1,len-ndx-1) );
			len = tmpStrs[1].size();
			if ( ( ndx = tmpStrs[1].find(")") ) > 0 )
			{
				srcStr = lrTrim( tmpStrs[1].substr(0,ndx) );
			}
			else
				return false;
		}
		else
		{
				return false;
		}	
	}
	return true;
}

//##ModelId=48561A9D03B9
// 功能：获得两个时间之间的时间差，单位为秒
// 两个时间的格式为'YYMMDDHHMMSS'
long FieldConvertRule_11::diffSecond(const string &dt1, const string &dt2)
{
    struct tm t;
    time_t t1,t2;
    char tmp[10];
    
    memcpy(tmp,dt1.c_str(),4);
    tmp[4]=0;
    t.tm_year=atoi(tmp)-1900;
    
    memcpy(tmp,dt1.c_str()+4,2);
    tmp[2]=0;
    t.tm_mon=atoi(tmp)-1;
    
    memcpy(tmp,dt1.c_str()+6,2);
    tmp[2]=0;
    t.tm_mday=atoi(tmp);
    
    memcpy(tmp,dt1.c_str()+8,2);
    tmp[2]=0;
    t.tm_hour=atoi(tmp);
    
    memcpy(tmp,dt1.c_str()+10,2);
    tmp[2]=0;
    t.tm_min=atoi(tmp);
    
    memcpy(tmp,dt1.c_str()+12,2);
    tmp[2]=0;
    t.tm_sec=atoi(tmp);
    
    t.tm_isdst=0;
    t1=mktime(&t);
    
    memcpy(tmp,dt2.c_str(),4);
    tmp[4]=0;
    t.tm_year=atoi(tmp)-1900;
    
    memcpy(tmp,dt2.c_str()+4,2);
    tmp[2]=0;
    t.tm_mon=atoi(tmp)-1;
    
    memcpy(tmp,dt2.c_str()+6,2);
    tmp[2]=0;
    t.tm_mday=atoi(tmp);
    
    memcpy(tmp,dt2.c_str()+8,2);
    tmp[2]=0;
    t.tm_hour=atoi(tmp);
    
    memcpy(tmp,dt2.c_str()+10,2);
    tmp[2]=0;
    t.tm_min=atoi(tmp);
    
    memcpy(tmp,dt2.c_str()+12,2);
    tmp[2]=0;
    t.tm_sec=atoi(tmp);
    
    t2=mktime(&t);
    
    return (long)difftime(t1,t2);
}

void FieldConvertRule_11::getFieldObj()
{
    m_pStartTimes  = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_sTimeFieldName.c_str()) );  
	m_pDataFlowUps = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_srcDtUpName.c_str()) );  
	m_pDataFlowDns = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_srcDtDnName.c_str()));  

    m_pFieldDateUp1 = m_pFieldInfos->getFieldInfoByName((m_totalDtUpName + "1").c_str());
    m_pFieldDateUp2 = m_pFieldInfos->getFieldInfoByName((m_totalDtUpName + "2").c_str());
    m_pFieldDateDn1 = m_pFieldInfos->getFieldInfoByName((m_totalDtDnName + "1").c_str());
    m_pFieldDateDn2 = m_pFieldInfos->getFieldInfoByName((m_totalDtDnName + "2").c_str());
    m_pFieldDurat1  = m_pFieldInfos->getFieldInfoByName((m_strDuratName + "1" ).c_str());
    m_pFieldDurat2  = m_pFieldInfos->getFieldInfoByName((m_strDuratName + "2" ).c_str());
	    
	return FieldConvertRule::getFieldObj();		
}
