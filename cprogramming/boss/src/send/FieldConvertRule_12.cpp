// ##########################################
// Source file : FieldConvertRule_12.h
// System      : Mobile Billing System
// Authors     : Chen Min
// E_mail      : chenm@lianchuang.com
// Create      : 20020704
// Update      : 
// Copyright(C): 2002 by Chen Min, Linkage.
// ##########################################

#include "FieldConvertRule_12.h"

//***********功能****************************
//针对nortel,将呼转原因为非无条件和不可及(1,4)的呼转话单压入容器中,
//当找到相应的mtc话单中的lac时,再输出
//********************************************

bool FieldConvertRule_12::initialize(string ruleText)
{
	m_Map03.clear();
	m_MapFW.clear();
	m_StrKey.clear();
	return true;
}

bool FieldConvertRule_12::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	FieldInfo *tmpFieldInfo,*CallTypeField;
	static FieldInfo aFieldInfo;
	map< string,vector<FieldInfo> >::iterator iteor_03;
	map< string,string >          ::iterator iteor_FW;
	string tmpLac;
	char tmp[20];
	static string callType("CALL_TYPE"),start_date("START_DATE"),start_time("START_TIME"),
	              imis("IMSI"),call_duration("CALL_DURATION"),lac("lac"),forward_cause("forward_cause");
	
	if( ( CallTypeField = getFieldMap(theFieldInfoMaps,callType ) )==0 )
		return false;
	
	// 只处理通话类型为03和FW的正确话单
	if(CallTypeField->m_FieldDesValue.compare("03")!=0 && 
	   CallTypeField->m_FieldDesValue.compare("FW")!=0) 
	{
		return false;
	}
	tmpFieldInfo = getFieldMap(theFieldInfoMaps,forward_cause);	
	
	if( m_CdrFlag->m_errorCode !=0 || 
	    tmpFieldInfo->m_FieldDesValue.compare("1")==0 ||
	    tmpFieldInfo->m_FieldDesValue.compare("4")==0   ) 
	{
		return false;
	}
	if( CallTypeField->m_FieldDesValue.compare("03")==0 ) //nortel呼转话单中的moc
	{
		// 根据开始时间,imis和通话时长拚出key值
		m_StrKey.clear();
		tmpFieldInfo = getFieldMap(theFieldInfoMaps,start_date);
		m_StrKey     = tmpFieldInfo->m_FieldDesValue;
		tmpFieldInfo = getFieldMap(theFieldInfoMaps,start_time);
		m_StrKey    += tmpFieldInfo->m_FieldDesValue;
		tmpFieldInfo = getFieldMap(theFieldInfoMaps,imis);
		m_StrKey    += tmpFieldInfo->m_FieldDesValue;
		tmpFieldInfo = getFieldMap(theFieldInfoMaps,call_duration);
		m_StrKey    += tmpFieldInfo->m_FieldDesValue;

		iteor_FW = m_MapFW.find( m_StrKey );
		
		if( iteor_FW != m_MapFW.end())
		{
			if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,lac))==0 ) 
            {
//		   	 	aFieldInfo.m_IsFormat 	= true;
		   		aFieldInfo.m_FieldName_ 	= lac;
		   		theFieldInfoMaps.push_back(aFieldInfo);

		   		tmpFieldInfo = getFieldMap(theFieldInfoMaps,lac);
	  		}
			tmpFieldInfo->m_FieldDesValue.clear();
			tmpFieldInfo->m_FieldDesValue = iteor_FW->second;
						
			m_MapFW.erase( iteor_FW );
		}
		else
		{
			m_Map03.insert( map< string,vector<FieldInfo> >::value_type( m_StrKey,theFieldInfoMaps) );
			//没有找到对应的lac将此moc话单作为无效话单输出
			CallTypeField->m_FieldDesValue = "FA";	
		}
	}
	else  //nortel呼转话单中的mtc
	{
		m_StrKey.clear();
		tmpFieldInfo = getFieldMap(theFieldInfoMaps,start_date);
		m_StrKey      = tmpFieldInfo->m_FieldDesValue;
		tmpFieldInfo = getFieldMap(theFieldInfoMaps,start_time);
		m_StrKey     += tmpFieldInfo->m_FieldDesValue;
		tmpFieldInfo = getFieldMap(theFieldInfoMaps,imis);
		m_StrKey     += tmpFieldInfo->m_FieldDesValue;
		tmpFieldInfo = getFieldMap(theFieldInfoMaps,call_duration);
		m_StrKey     += tmpFieldInfo->m_FieldDesValue;

		if( ( tmpFieldInfo = getFieldMap(theFieldInfoMaps,lac) ) == 0 )
		{
			return false;
		}
		tmpLac      = tmpFieldInfo->m_FieldDesValue;
		
		iteor_03 = m_Map03.find( m_StrKey );
		
		//在moc容器中找到匹配,则用moc话单代替mtc在标准文件中输出
		if( iteor_03 != m_Map03.end())
		{
			theFieldInfoMaps.clear();
			theFieldInfoMaps = iteor_03->second;
			if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,lac))==0 ) 
            {
//		   	 	aFieldInfo.m_IsFormat 	= true;
		   		aFieldInfo.m_FieldName_ 	= lac;
		   		theFieldInfoMaps.push_back(aFieldInfo);

		   		tmpFieldInfo = getFieldMap(theFieldInfoMaps,lac);
	  		}
			tmpFieldInfo->m_FieldDesValue = tmpLac;
			
			m_Map03.erase( iteor_03 );
		}
		else
		{
			m_MapFW.insert( map< string,string >::value_type( m_StrKey,tmpLac) );
		}
	}
	
	if( m_MapFW.size()>10 )
	{
		m_MapFW.erase( m_MapFW.begin() );	
	}
	if( m_Map03.size()>10 )
	{
		m_Map03.erase( m_Map03.begin() );	
	}	
	return true;
}

