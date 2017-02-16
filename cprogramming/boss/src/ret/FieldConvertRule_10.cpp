// ##########################################
// Source file : FieldConvertRule_10.cpp
// System      : Mobile Billing System
// Version     : 0.1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20020516
// Update      : 20020516
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "FieldConvertRule_10.h"

//##ModelId=3CE364420011
////## 此转换用于把以以秒为单位的时间转换为时分秒表示的时间值。
bool FieldConvertRule_10::initialize(string ruleText)
{
    vector<string> tmpStrs,tmpStrs1;
	
    //rule_8;ALL;19940101;startSend,endSend
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 4)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}

	// 提取规则中的信息
	m_callType = tmpStrs[1]; //跳过 'rule_10',get 呼叫类型 "ALL"
	
	// 得到基准时间:19940101
    if( tmpStrs[2].size() >= 8)
    {
	    int iYear,iMonth,iDay;
	    iYear=iMonth=iDay=0;
	    iYear  = atoi(tmpStrs[2].substr(0,4).c_str());
	    iMonth = atoi(tmpStrs[2].substr(4,2).c_str());
	    iDay   = atoi(tmpStrs[2].substr(6,2).c_str());
	    m_baseDateTime.setDateTime(iYear,iMonth,iDay,0,0,0);
	}
	else
	{
		// 基准时间值非法
		return false;	
	}    
	
	// 得到需要转换的时间值
	splitStrByToken(tmpStrs[3],",",tmpStrs1); 	
	if( tmpStrs1.size() == 2 )
	{
		m_startFieldName = tmpStrs1[0];
		m_endFieldName   = tmpStrs1[1];
	}
	else
	{
		return false;	
	}

    return true;
}

//##ModelId=3CE3645003C8
bool FieldConvertRule_10::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    int startDuration,endDuration;
    FieldInfo *startFieldInfo,*endFieldInfo,*tmpFieldInfo;
    DateTime startDateTime,endDateTime;
    char cDuration[6];
    static string callType("CALL_TYPE"),all("ALL"),du("DURATION"),
                startDate("START_DATE"),startTime("START_TIME"),endDate("END_DATE"),endTime("END_TIME");

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

    if( (startFieldInfo=getFieldMap(theFieldInfoMaps,m_startFieldName))==0 
    	||(endFieldInfo=getFieldMap(theFieldInfoMaps,m_endFieldName))==0  )
	{
		m_CdrFlag->m_isError       = true;
	    m_CdrFlag->m_fieldName     = m_startFieldName;
	    m_CdrFlag->m_errorCode     = INVALID_DATE_TIME;
		
		return false;	
	}
	
	startDuration = atoi(startFieldInfo->m_FieldDesValue.c_str());
	endDuration   = atoi(endFieldInfo->m_FieldDesValue.c_str());
	// 设置时间值为基础时间值	
	startDateTime = endDateTime = m_baseDateTime;
	
	startDateTime.addSecond(startDuration);
	endDateTime.addSecond(endDuration);
	// 根据开始时间和结束时间算出通话时长
	sprintf(cDuration,"%d",endDateTime.secondDiff(&startDateTime));
	
	// 插入字段
	addField(theFieldInfoMaps,startDate,startDateTime.getDate(),startDateTime.getDate().size());
    addField(theFieldInfoMaps,startTime,startDateTime.getTime(),startDateTime.getTime().size());
    addField(theFieldInfoMaps,endDate,endDateTime.getDate(),endDateTime.getDate().size());
    addField(theFieldInfoMaps,endTime,endDateTime.getTime(),endDateTime.getTime().size());
    addField(theFieldInfoMaps,du,cDuration,strlen(cDuration));

    return true;
}

void FieldConvertRule_10::addField(multimap<MyString,FieldInfo *> &theFieldInfoMaps,const string &fieldName,
					const string &fieldValue,const int filenLength)
{
	FieldInfo *pFieldInfo;

	pFieldInfo = new FieldInfo(filenLength+1);
	pFieldInfo->m_FieldName_	= fieldName;
	pFieldInfo->m_FieldDesValue	= fieldValue;
	pFieldInfo->m_FieldLength	= filenLength;
//	pFieldInfo->m_IsFormat		= true;
//	pFieldInfo->m_IsCheck 		= true;	

	theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(fieldName.c_str(),pFieldInfo) );
}

