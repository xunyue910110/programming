// ##########################################
// Source file : FieldConvertRule_15.cpp
// System      : Mobile Billing System
// Version     : 0.1.16
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20020526
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldConvertRule_15.h"
#include <stdlib.h>

//##提供gprs流量链表中的上下行流量和时长,根据"service_Id"累计的功能
bool FieldConvertRule_15::initialize(string ruleText)
{
	string strDuratName,strTmp;
	vector<string> tmpStrs,tmpStrs1;

    //rule_15;ALL;changeTime;DATA_UP=sum(dataVolumeGPRSUp),DATA_DOWN=sum(dataVolumeGPRSDn),DURATION=sum(changeCondition),DURAT=difftime(changeTime)
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 4)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	
	// 提取规则中的信息
	m_callType = tmpStrs[1];       //跳过 'rule_15',get "ALL"
	m_serviceID = tmpStrs[2]; // get "changeTime"   
	
	// 得到规则15中的 具体操作部分:DATA_UP=sum(cbcUplinkByteCount),DATA_DOWN=sum(cbcDownlinkByteCount),DURAT=difftime(cbcDuration)
	splitStrByToken(tmpStrs[3],",",tmpStrs1);
	if( tmpStrs1.size() != 3 )
    {
        return false;
    }
	if( !getFields(tmpStrs1[0],m_totalDtUpName,m_srcDtName)
	    || !getFields(tmpStrs1[1],m_totalDtDnName,m_srcDtDnName) 
	    || !getFields(tmpStrs1[2],m_totalDuratName,m_strDuratName)  )
	{
		return false;	
	}

	return true;
}

bool FieldConvertRule_15::getFields(const string &ruleStr,string &totalStr,string &srcStr)
{
	vector<string> tmpStrs;
	int ndx,len;	

	splitStrByToken(ruleStr,"=",tmpStrs); //get DATA_UP=sum(dataVolumeGPRSUp)
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


//##ModelId=3BAAEAFF02AF
bool FieldConvertRule_15::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	int len,i,j;
	FieldInfo *tmpFieldInfo;
	static string callType("CALL_TYPE"),all("ALL");
	bool isMatch;
	
	if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,callType))==0 )
		return false;
	
	// 条件1：通话类型
	if(m_callType.compare(all)!=0) 
	{
	   if(tmpFieldInfo->m_FieldDesValue.compare(m_callType)!=0) 
	   {
	      return false;
	   }
	}
	// 复位
	m_serviceIDs.clear();
	m_dataFlowUps.clear();
	m_dataFlowDns.clear();
	m_durations.clear();
	
	// 从字段容器中提取 时间、上下行流量、时长 的值
	getMultiFieldValues(theFieldInfoMaps,m_serviceID,m_serviceIDs);
	getMultiFieldValues(theFieldInfoMaps,m_srcDtName,m_dataFlowUps);
	getMultiFieldValues(theFieldInfoMaps,m_srcDtDnName,m_dataFlowDns);
	getMultiFieldValues(theFieldInfoMaps,m_strDuratName,m_durations);
	
	if(  (m_serviceIDs.size()>0)
	     &&( m_serviceIDs.size()== m_dataFlowUps.size())
	     &&( m_serviceIDs.size()== m_dataFlowDns.size())
	     &&( m_serviceIDs.size()== m_durations.size())  )
	{
		m_pCbcLists.clear();
		CbcListOfFlow *pCbcListOfFlow;
		
		// 查找唯一的serviceid,并合并
		pCbcListOfFlow = new CbcListOfFlow();
		pCbcListOfFlow->m_serviceId = m_serviceIDs[0];
		pCbcListOfFlow->m_dataUp    = atoi(m_dataFlowUps[0].c_str());
		pCbcListOfFlow->m_dataDn    = atoi(m_dataFlowDns[0].c_str());
		pCbcListOfFlow->m_duration  = atol(m_durations[0].c_str());
		m_pCbcLists.push_back(pCbcListOfFlow); 

		for( i=1;i<m_serviceIDs.size();++i)
		{
			isMatch = false;
			for(j=0;j<m_pCbcLists.size();++j)
			{
				if(m_serviceIDs[i] == m_pCbcLists[j]->m_serviceId )
				{
					m_pCbcLists[j]->m_dataUp   += atoi(m_dataFlowUps[i].c_str());
					m_pCbcLists[j]->m_dataDn   += atoi(m_dataFlowDns[i].c_str());
					m_pCbcLists[j]->m_duration += atol(m_durations[i].c_str());
					
					isMatch =  true;
					break;	
				}
			}
			if( !isMatch)
			{
				pCbcListOfFlow = new CbcListOfFlow();
				pCbcListOfFlow->m_serviceId = m_serviceIDs[i];
				pCbcListOfFlow->m_dataUp    = atoi(m_dataFlowUps[i].c_str());
				pCbcListOfFlow->m_dataDn    = atoi(m_dataFlowDns[i].c_str());
				pCbcListOfFlow->m_duration  = atol(m_durations[i].c_str());
				m_pCbcLists.push_back(pCbcListOfFlow);	
			}
		}
		// 输出到输出容器中
		for(i=0;i<m_pCbcLists.size();++i)
		{
			FieldInfo *pFieldInfo;
			string strSequence;
			//strSequence = StringUtil::toString(i+1);
			strSequence = "0";
			
			// 上行流量
			pFieldInfo = new FieldInfo(0);
			pFieldInfo->m_FieldName_    = m_totalDtUpName ;//+ strSequence;
			pFieldInfo->m_FieldDesValue = StringUtil::toString(m_pCbcLists[i]->m_dataUp);
			theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_totalDtUpName.c_str(),pFieldInfo) );
			
			// 下行流量
			pFieldInfo = new FieldInfo(0);
			pFieldInfo->m_FieldName_    = m_totalDtDnName ;//+ strSequence;
			pFieldInfo->m_FieldDesValue = StringUtil::toString(m_pCbcLists[i]->m_dataDn);
			theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_totalDtDnName.c_str(),pFieldInfo) );		

			// 时长
			pFieldInfo = new FieldInfo(0);
			pFieldInfo->m_FieldName_    = m_totalDuratName ;//+ strSequence;
			pFieldInfo->m_FieldDesValue = StringUtil::toString(m_pCbcLists[i]->m_duration);
			theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_totalDuratName.c_str(),pFieldInfo) );	

			// serviceID
			pFieldInfo = new FieldInfo(0);
			pFieldInfo->m_FieldName_    = m_serviceID + strSequence;
			pFieldInfo->m_FieldDesValue = m_pCbcLists[i]->m_serviceId;
			theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(pFieldInfo->m_FieldName_.c_str(),pFieldInfo) );				
			
			if( m_pCbcLists[i] != 0)
				delete m_pCbcLists[i];
			
		}	
	}
	else if( m_serviceIDs.size()!=0 )
	{
		m_CdrFlag->m_errorCode = INVALID_DATE_TIME;
		m_CdrFlag->m_isError   = true;	
	}
	
	return true;
}

// 功能:获得两个时间之间的时间差，单位为 秒
//      两个时间的格式为'YYMMDDHHMMSS'
long FieldConvertRule_15::diffSecond(const string & dt1,const string & dt2)
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
