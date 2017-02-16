// ##########################################
// Source file : FieldConvertRule_11.cpp
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

#include "FieldConvertRule_11.h"
#include <stdlib.h>

//##提供gprs流量链表中的上下行流量和时长,根据时段累计的功能
bool FieldConvertRule_11::initialize(string ruleText)
{
	string strTmp;
	vector<string> tmpStrs,tmpStrs1;

    //rule_11;ALL;changeTime;DATA_UP=sum(dataVolumeGPRSUp),DATA_DOWN=sum(dataVolumeGPRSDn),DURATION=sum(changeCondition),DURAT=difftime(changeTime)
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 4)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	
	// 提取规则中的信息
	m_callType = tmpStrs[1];       //跳过 'rule_11',get "ALL"
	m_sTimeFieldName = tmpStrs[2]; // get "changeTime"   
	
	// 得到规则11中的 具体操作部分:DATA_UP=sum(dataVolumeGPRSUp),DATA_DOWN=sum(dataVolumeGPRSDn),DURAT=difftime(changeTime)
	splitStrByToken(tmpStrs[3],",",tmpStrs1);
	if( tmpStrs1.size() != 3 )
    {
        return false;
    }
	if( !getFields(tmpStrs1[0],m_totalDtUpName,m_srcDtName)
	    || !getFields(tmpStrs1[1],m_totalDtDnName,m_srcDtDnName) 
	    || !getFields(tmpStrs1[2],m_strDuratName,strTmp)  )
	{
		return false;	
	}
/*	m_dateUp1Info.reset();
	m_dateUp1Info.m_IsFormat   = true;
	m_dateUp1Info.m_FieldName_ = m_totalDtUpName + "1";
	
	m_dateUp2Info.reset();
	m_dateUp2Info.m_IsFormat   = true;
	m_dateUp2Info.m_FieldName_ = m_totalDtUpName + "2";
	
	m_dateDn1Info.reset();
	m_dateDn1Info.m_IsFormat   = true;
	m_dateDn1Info.m_FieldName_ = m_totalDtDnName + "1";
	
	m_dateDn2Info.reset();
	m_dateDn2Info.m_IsFormat   = true;
	m_dateDn2Info.m_FieldName_ = m_totalDtDnName + "2";

	m_durat1Info.reset();
	m_durat1Info.m_IsFormat   = true;
	m_durat1Info.m_FieldName_ = strDuratName + "1";
	
	m_durat2Info.reset();
	m_durat2Info.m_IsFormat   = true;
	m_durat2Info.m_FieldName_ = strDuratName + "2";*/
	
	return true;
}

bool FieldConvertRule_11::getFields(const string &ruleStr,string &totalStr,string &srcStr)
{
	vector<string> tmpStrs;
	int ndx,len;	

	splitStrByToken(ruleStr,"=",tmpStrs); //get DATA_UP=sum(dataVolumeGPRSUp)
	if( tmpStrs.size() == 2 )
	{
		totalStr = tmpStrs[0];
		// length(msisdn),其实只有括号里的字段才有用,"sum"不起作用,实际配置时可以是任意值	
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
bool FieldConvertRule_11::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	int len,i;
	FieldInfo *tmpFieldInfo;
	static string callType("CALL_TYPE"),all("ALL");
	static string STARTDATE("START_DATE");
	static string STARTTIME("START_TIME");
	
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
	m_sTimes.clear();
	m_dataFlowUps.clear();
	m_dataFlowDns.clear();

	// 得到opendate,作为第一段流量的起始时间,用以判断流量发生在忙时还是闲时
	if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,STARTDATE))==0 )
		return false;
	m_sTimes.push_back( tmpFieldInfo->m_FieldDesValue.substr(2) );
	if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,STARTTIME))==0 )
		return false;
	m_sTimes[0] += tmpFieldInfo->m_FieldDesValue;
	
	// 从字段容器中提取 时间、上下行流量、时长 的值
	getMultiFieldValues(theFieldInfoMaps,m_sTimeFieldName,m_sTimes);
	getMultiFieldValues(theFieldInfoMaps,m_srcDtName,m_dataFlowUps);
	getMultiFieldValues(theFieldInfoMaps,m_srcDtDnName,m_dataFlowDns);
	
	if( ( (m_sTimes.size()-1)== m_dataFlowUps.size())
	     &&( (m_sTimes.size()-1)== m_dataFlowDns.size())  )
	{
		// 流量、时长计数器清零
		int iDataUp1,iDataDown1,iDataUp2,iDataDown2;
		long iDuration1,iDuration2;
		iDataUp1=iDataDown1=iDataUp2=iDataDown2=0;
		iDuration1=iDuration2=0;
		
		// 根据时段统计流量和时长
		for( i=0;i<(m_sTimes.size()-1);i++)
		{
			if((m_sTimes[i].compare(6,6,"230000") >= 0 && m_sTimes[i].compare(6,6,"240000") <= 0)
			  || (m_sTimes[i].compare(6,6,"000000") >= 0 && m_sTimes[i].compare(6,6,"065959") <= 0))
			{
				iDataUp2   += atoi( m_dataFlowUps[i].c_str() );
				iDataDown2 += atoi( m_dataFlowDns[i].c_str() );
				iDuration2 += diffSecond(string("20")+m_sTimes[i+1],string("20")+m_sTimes[i]);
			}   
			else
			{
				iDataUp1   += atoi( m_dataFlowUps[i].c_str() ); 
				iDataDown1 += atoi( m_dataFlowDns[i].c_str() );
				iDuration1 += diffSecond(string("20")+m_sTimes[i+1],string("20")+m_sTimes[i]);
			}
		}
		
		m_dateUp1Info = new FieldInfo(0);
		m_dateUp2Info = new FieldInfo(0);
		m_dateDn1Info = new FieldInfo(0);
		m_dateDn2Info = new FieldInfo(0);
		m_durat1Info  = new FieldInfo(0);
		m_durat2Info  = new FieldInfo(0);

//		m_dateUp1Info->m_IsFormat   = true;
		m_dateUp1Info->m_FieldName_ = m_totalDtUpName + "1";
//		m_dateUp2Info->m_IsFormat   = true;
		m_dateUp2Info->m_FieldName_ = m_totalDtUpName + "2";
//		m_dateDn1Info->m_IsFormat   = true;
		m_dateDn1Info->m_FieldName_ = m_totalDtDnName + "1";
//		m_dateDn2Info->m_IsFormat   = true;
		m_dateDn2Info->m_FieldName_ = m_totalDtDnName + "2";
//		m_durat1Info->m_IsFormat    = true;
		m_durat1Info->m_FieldName_  = m_strDuratName + "1";
//		m_durat2Info->m_IsFormat    = true;
		m_durat2Info->m_FieldName_  = m_strDuratName + "2";
		
		m_dateUp1Info->m_FieldDesValue = StringUtil::toString(iDataUp1);
		m_dateUp2Info->m_FieldDesValue = StringUtil::toString(iDataUp2);
		m_dateDn1Info->m_FieldDesValue = StringUtil::toString(iDataDown1);
		m_dateDn2Info->m_FieldDesValue = StringUtil::toString(iDataDown2);
		m_durat1Info ->m_FieldDesValue = StringUtil::toString(iDuration1);
		m_durat2Info ->m_FieldDesValue = StringUtil::toString(iDuration2);
				
		theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_dateUp1Info->m_FieldName_.c_str(),m_dateUp1Info) );
		theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_dateUp2Info->m_FieldName_.c_str(),m_dateUp2Info) );
		theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_dateDn1Info->m_FieldName_.c_str(),m_dateDn1Info) );
		theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_dateDn2Info->m_FieldName_.c_str(),m_dateDn2Info) );
		theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_durat1Info->m_FieldName_.c_str(),m_durat1Info) );
		theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(m_durat2Info->m_FieldName_.c_str(),m_durat2Info) );
		
		if( iDataUp1==0 && iDataUp2==0 
		    && iDataDown1==0 && iDataDown2==0 )
		{
			m_CdrFlag->m_errorCode = GPRS_DATA_NULL;
			m_CdrFlag->m_isError   = true;
			m_CdrFlag->m_fieldName = m_dateUp1Info->m_FieldName_;		
		}	
	}
	else
	{
		m_CdrFlag->m_errorCode = INVALID_DATE_TIME;
		m_CdrFlag->m_isError   = true;
		m_CdrFlag->m_fieldName = m_dateUp1Info->m_FieldName_;		
	}
	
	return true;
}

// 功能:获得两个时间之间的时间差，单位为 秒
//      两个时间的格式为'YYMMDDHHMMSS'
long FieldConvertRule_11::diffSecond(const string & dt1,const string & dt2)
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
