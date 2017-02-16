// ##########################################
// Source file : FieldConvertRule_14.cpp
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

#include "FieldConvertRule_14.h"
#include "FieldInfos.h"
#include "RepeatableFieldInfo.h"
#include <stdlib.h>

//##ModelId=4856200D00DA
//##提供gprs流量链表中的上下行流量和时长,根据"service_Id"累计的功能
bool FieldConvertRule_14::initialize(string ruleText)
{
    string strDuratName,strTmp;
	vector<string> tmpStrs,tmpStrs1;

    //RULE_14:FIELD=changeTime;DATA_UP=sum(dataVolumeGPRSUp),DATA_DOWN=sum(dataVolumeGPRSDn),DURAT=difftime(changeTime)
	StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 2)
	{
		// 小于1个分号的,不符合规则的书写格式
		return false;	
	}
	
	// 提取规则中的信息
	tmpStrs[0] = tmpStrs[0].substr(8);
	if (tmpStrs[0].compare(0,5,"FIELD") != 0)
	{
	    return false;
	}
	else
	{
	    tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	    if (tmpStrs1.size() != 2)
	    {
	        return false;
	    }
	    else
	    {
	       m_callType = "ALL";
	       m_serviceID = tmpStrs1[1]; 
	    }
	}
	
	// 得到规则14中的具体操作部分:DATA_UP=sum(cbcUplinkByteCount),DATA_DOWN=sum(cbcDownlinkByteCount),DURAT=difftime(cbcDuration)
	tmpStrs1.clear();StringUtil::split(tmpStrs[1],",",tmpStrs1);
	if( tmpStrs1.size() != 3 )
    {
        return false;
    }
	if( !getFields(tmpStrs1[0],m_totalDtUpName,m_srcDtName)
	    || !getFields(tmpStrs1[1],m_totalDtDnName,m_srcDtDnName) 
	    || !getFields(tmpStrs1[2],m_totalDuratName,m_srcDuratName)  )
	{
		return false;	
	}
	this->getFieldObj();

	return true;
}

//##ModelId=4856201601D4
bool FieldConvertRule_14::convertField()
{
    int i,j;
	bool isMatch;
	
	// 条件1：通话类型
    if(m_callType.compare(CONDITION_ALL)!=0) 
    {
        if( m_pFieldCallType->m_iDesValueLen == 0
        	||strcmp(m_pFieldCallType->m_FieldDesValue,m_callType.c_str())!=0 ) 
        {
            return false;
        }
    }
	
	if(  (m_pSrcServiceID->getNumCount()>0)
	     &&( m_pSrcServiceID->getNumCount()== m_pSrcDataFlowUps->getNumCount())
	     &&( m_pSrcServiceID->getNumCount()== m_pSrcDataFlowDns->getNumCount())
	     &&( m_pSrcServiceID->getNumCount()== m_pSrcCbcDu->getNumCount()  )
	   )
	{
		m_pCbcLists.clear();
		CbcListOfFlow *pCbcListOfFlow;
		
		// 查找唯一的serviceid,并合并
		pCbcListOfFlow = new CbcListOfFlow();
		pCbcListOfFlow->m_serviceId = m_pSrcServiceID->m_fieldInfoVect[0]->m_FieldDesValue;
		pCbcListOfFlow->m_dataUp    = atoi(m_pSrcDataFlowUps->m_fieldInfoVect[0]->m_FieldDesValue);
		pCbcListOfFlow->m_dataDn    = atoi(m_pSrcDataFlowDns->m_fieldInfoVect[0]->m_FieldDesValue);
		pCbcListOfFlow->m_duration  = atol(m_pSrcCbcDu->m_fieldInfoVect[0]->m_FieldDesValue);
		m_pCbcLists.push_back(pCbcListOfFlow); 

		for( i=1;i<m_pSrcServiceID->getNumCount();++i)
		{
			isMatch = false;
			for(j=0;j<m_pCbcLists.size();++j)
			{
				if( strcmp(m_pSrcServiceID->m_fieldInfoVect[i]->m_FieldDesValue,m_pCbcLists[j]->m_serviceId.c_str())==0 )
				{
					m_pCbcLists[j]->m_dataUp   += atoi(m_pSrcDataFlowUps->m_fieldInfoVect[i]->m_FieldDesValue);
					m_pCbcLists[j]->m_dataDn   += atoi(m_pSrcDataFlowDns->m_fieldInfoVect[i]->m_FieldDesValue);
					m_pCbcLists[j]->m_duration += atol(m_pSrcCbcDu->m_fieldInfoVect[i]->m_FieldDesValue);
					
					isMatch =  true;
					break;	
				}
			}
			if( !isMatch)
			{
				pCbcListOfFlow = new CbcListOfFlow();
				pCbcListOfFlow->m_serviceId = m_pSrcServiceID->m_fieldInfoVect[i]->m_FieldDesValue;
				pCbcListOfFlow->m_dataUp    = atoi(m_pSrcDataFlowUps->m_fieldInfoVect[i]->m_FieldDesValue);
				pCbcListOfFlow->m_dataDn    = atoi(m_pSrcDataFlowDns->m_fieldInfoVect[i]->m_FieldDesValue);
				pCbcListOfFlow->m_duration  = atol(m_pSrcCbcDu->m_fieldInfoVect[i]->m_FieldDesValue);
				m_pCbcLists.push_back(pCbcListOfFlow);	
			}
		}
		// 输出到输出容器中
		for(i=0;i<m_pCbcLists.size();++i)
		{
			// 上行流量
			m_pDesDataFlowUps->setDesValue( StringUtil::toString(m_pCbcLists[i]->m_dataUp).c_str() );
			
			// 下行流量
			m_pDesDataFlowDns->setDesValue( StringUtil::toString(m_pCbcLists[i]->m_dataDn).c_str() );
			
			// 时长
			m_pDesCbcDu->setDesValue( StringUtil::toString(m_pCbcLists[i]->m_duration).c_str() );
			
			// serviceID
			m_pDesServiceID->setDesValue( m_pCbcLists[i]->m_serviceId );
			
			if( m_pCbcLists[i] != 0)
				delete m_pCbcLists[i];
			
		}	
	}
	else if( m_pSrcServiceID->getNumCount()!=0 )
	{
		m_CdrFlag->m_errorCode = INVALID_DATE_TIME;
		m_CdrFlag->m_isError   = true;	
	}
	
	return true;
}

bool FieldConvertRule_14::getFields(const string &ruleStr,string &totalStr,string &srcStr)
{
	vector<string> tmpStrs;
	int ndx,len;	

	StringUtil::split(ruleStr,"=",tmpStrs); //get DATA_UP=sum(dataVolumeGPRSUp)
	if( tmpStrs.size() == 2 )
	{
		totalStr = tmpStrs[0];
		// sum(msisdn),其实只有括号里的字段才有用,"sum"不起作用,实际配置时可以是任意值	
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

// 功能:获得两个时间之间的时间差，单位为秒
//      两个时间的格式为'YYMMDDHHMMSS'
long FieldConvertRule_14::diffSecond(const string & dt1,const string & dt2)
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

void FieldConvertRule_14::getFieldObj()
{
    m_pSrcServiceID   = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_serviceID.c_str()) );  
	m_pSrcDataFlowUps = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_srcDtName.c_str()) );  
	m_pSrcDataFlowDns = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_srcDtDnName.c_str()) );  
	m_pSrcCbcDu       = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_srcDuratName.c_str()) );  

    m_pDesServiceID   = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName((m_serviceID+"0").c_str()) );  
	m_pDesDataFlowUps = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_totalDtUpName.c_str()) );  
	m_pDesDataFlowDns = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_totalDtDnName.c_str()) );  
	m_pDesCbcDu       = dynamic_cast<RepeatableFieldInfo*>( m_pFieldInfos->getRepeatableFieldInfoByName(m_totalDuratName.c_str()) );  
	
	return FieldConvertRule::getFieldObj();		
}
