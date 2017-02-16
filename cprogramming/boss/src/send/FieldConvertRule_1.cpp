// ##########################################
// Source file : FieldConvertRule_1.cpp
// System      : Mobile Billing System
// Version     : 0.1.9
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010921
// Update      : 20011211
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

//############################################### 
//# 规则名称：rule_1                            #
//# 功能：    用于将原字段值转换为统一的代码    #
//###############################################

#include <vector>
#include <string.h>
#include "FieldConvertRule_1.h"

//##ModelId=3BAAEAB90073
bool FieldConvertRule_1::initialize(string ruleText)
{
	int len,ndx;
	string strTmp;
	vector<string> tmpStrs;

	// example:          rule_1;ALL;MSRN_FLAG;         05=91,06=A1,OTHER=81
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 4)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	m_callType = tmpStrs[1]; //跳过 'rule_1'
	m_srcFieldName = tmpStrs[2];
	
	// 转换 05=91,06=A1,OTHER=81
	strTmp = tmpStrs[3];
	splitStrByToken(strTmp,",",tmpStrs);
	
	for(int i=0;i<tmpStrs.size();i++)
	{
		len	= tmpStrs[i].size();
		if ( ( ndx = tmpStrs[i].find("=") ) < 0 )
			return false;
		
		m_srcFieldValues.push_back( lrTrim( tmpStrs[i].substr(0,ndx) ) );
		m_desFieldValues.push_back( lrTrim( tmpStrs[i].substr(ndx+1,len-ndx-1) ) );	
	}
	
	return true;
}

//##ModelId=3BAAEAE90145
bool FieldConvertRule_1::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
	int len;
	static string theCallType("CALL_TYPE");
	FieldInfo *srcFieldInfo,*callType;
	
	
	if ( (srcFieldInfo=getFieldMap(theFieldInfoMaps,m_srcFieldName)) == 0 )
		return false;
	if ( (callType=getFieldMap(theFieldInfoMaps,theCallType)) == 0 )
		return false;
	
	if( m_callType.compare("ALL")!=0 ) 
	{
	    if ( callType->m_FieldDesValue.compare(m_callType)!=0 )
	      return false;
	}
	
	len=m_srcFieldValues.size();
	for(int i=0;i<len;i++)
	{
		if( srcFieldInfo->m_FieldDesValue == m_srcFieldValues[i] ) 
		{
			srcFieldInfo->m_FieldDesValue=m_desFieldValues[i];
			break;
		} 
		else if ( m_srcFieldValues[i].compare("OTHER")==0 ) 
		{
		   srcFieldInfo->m_FieldDesValue=m_desFieldValues[i];
		}
	}

	return true;
} 
