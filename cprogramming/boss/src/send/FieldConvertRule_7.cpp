// ##########################################
// Source file : FieldConvertRule_7.cpp
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

#include "FieldConvertRule_7.h"

//##ModelId=3CE364420011
bool FieldConvertRule_7::initialize(string ruleText)
{
    char cTmp[100];
	vector<string> tmpStrs,tmpStrs1;

    //rule_7;01;ALL;hour,minute,second
	splitStrByToken(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 4)
	{
		// 小于4个分号的,不符合规则的书写格式
		return false;	
	}

	// 提取规则中的信息
	m_callType = tmpStrs[1]; //跳过 'rule_7',get "01"
	
    // 规则7中除呼叫类型外的条件初始化:MSRN_FLAG=91或 ALL
    if(strcmp(tmpStrs[2].c_str(),"ALL")==0) 
    {
    	m_conditionFieldName = "ALL";
    } 
    else 
    {
        // 得到条件中的运算符
        strcpy(cTmp,tmpStrs[2].c_str());
        if(strstr(cTmp,"!=")!=NULL) 
        {
            m_conditionExp = "!=";
        } 
        else if(strstr(cTmp,"=")!=NULL) 
        {
            m_conditionExp = "=";
        } 
        else 
        {
            return false;
        }
        // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
        splitStrByToken(tmpStrs[2],m_conditionExp.c_str(),tmpStrs1);
        if(tmpStrs1.size()!=2)
        {
        	return false;	
        }
        else
        {
        	m_conditionFieldName  = tmpStrs1[0];
        	m_conditionFieldValue = tmpStrs1[1];	
        }	
	}
	
	// 提取 hour,minute,second
	splitStrByToken(tmpStrs[3],",",tmpStrs1);
	if( tmpStrs1.size() == 3 )
	{
		m_hour   = tmpStrs1[0];
		m_minute = tmpStrs1[1];
		m_second = tmpStrs1[2];
	}
	else
	{
		return false;	
	}
    return true;
}

//##ModelId=3CE3645003C8
bool FieldConvertRule_7::convertField(multimap<MyString,FieldInfo *> &theFieldInfoMaps)
{
    char tmp[10];
    int hour,minute,second;
    FieldInfo *pFieldInfo;
    FieldInfo *tmpFieldInfo;
    static string callType("CALL_TYPE"),all("ALL"),du("DURATION");

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

    // 条件2：其他字段值
	if(m_conditionFieldName.compare(all)!=0) 
	{                                               // not is "ALL"
	    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_conditionFieldName))==0 )
		    return false;

		if(m_conditionFieldValue.compare(all)!=0) 
		{                                          // 字段值不是"ALL"
		    if(m_conditionExp.compare("!=")==0) {
		        if(tmpFieldInfo->m_FieldDesValue.compare(m_conditionFieldValue)==0) 
		        {
                    return false;
                }
		    } 
		    else if(m_conditionExp.compare("=")==0) 
		    {
		        if(tmpFieldInfo->m_FieldDesValue.compare(m_conditionFieldValue)!=0) 
		        {
                    return false;
                }
		    } 
		    else 
		    {
		        return false;
		    }

        } 
        else 
        {                                                   // 字段值是"ALL"
            if(m_conditionExp.compare("!=")==0) 
            {                                               // 如果字段没有值(!=ALL),即不为空
                if(tmpFieldInfo->m_FieldDesValue.size()>0)
                    return false;                           // 有值，不满足条件
            } 
            else if(m_conditionExp.compare("=")==0) 
            {                                               // 如果字段有值(=ALL),即为空
                if(tmpFieldInfo->m_FieldDesValue.size()<=0)
                    return false;                           // 没有值，不满足条件
            } 
            else 
            {
                return false;
            }
        }
    }

    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_hour))==0 )
		hour = 0;
    else
        hour = atoi( tmpFieldInfo->m_FieldDesValue.c_str() );
    tmpFieldInfo = 0;

    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_minute))==0 )
		minute = 0;
	else
        minute = atoi( tmpFieldInfo->m_FieldDesValue.c_str() );
    tmpFieldInfo = 0;

    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,m_second))==0 )
		second = 0;
	else
        second = atoi( tmpFieldInfo->m_FieldDesValue.c_str() );
    tmpFieldInfo = 0;

    second += hour * 3600;
    second += minute * 60;

    sprintf(tmp,"%d",second);

    if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,du))==0 ) 
    {
	    pFieldInfo = new FieldInfo(0);
	    pFieldInfo->m_FieldName_		= du;
	    pFieldInfo->m_FieldLength	    = 0;
//	    pFieldInfo->m_IsFormat			= true;
//	    pFieldInfo->m_IsCheck 			= true;
	    pFieldInfo->m_FieldDesValue   = tmp;

	    theFieldInfoMaps.insert( multimap<MyString,FieldInfo *>::value_type(du.c_str(),pFieldInfo) );
    } 
    else 
    {
        tmpFieldInfo->m_FieldDesValue	= tmp;
    }
    editTime( second,theFieldInfoMaps );

    return true;
}

