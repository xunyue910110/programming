// ##########################################
// Source file : FieldConvertRule_1.cpp
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080925
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

//############################################### 
//# 规则名称：RULE_1                            #
//# 功能：用于将原字段值转换为统一的代码        #
//###############################################

#include <vector>
#include <string.h>
#include "FieldConvertRule_1.h"
#include "FieldInfos.h"

//##ModelId=485614B302CE
bool FieldConvertRule_1::initialize(string ruleText)
{
    int len,ndx;
	vector<string> tmpStrs, tmpStrs1;

	m_srcFieldValues.clear();
	m_desFieldValues.clear();
	
       tmpStrs.clear();
	StringUtil::split(ruleText,";",tmpStrs);
	if(tmpStrs.size() < 2)
	{
		//小于1个分号的,不符合规则的书写格式
		return false;
	}
	// example:RULE_1:FIELD=CALL_TYPE;00=01,01=02,02=03,OTHER=FF
    if(tmpStrs.size() == 2)
    {
	     tmpStrs[0] = tmpStrs[0].substr(7);
	     
	     if(tmpStrs[0].compare(0,strlen(DEAL_OBJ_NAME),DEAL_OBJ_NAME) == 0)
	     {
	     	tmpStrs1.clear();
	        StringUtil::split(tmpStrs[0],"=",tmpStrs1);
	        m_callType = CONDITION_ALL;
	        m_srcFieldName = tmpStrs1[1];
	     }
	     else
	     {
	        return false;
	     }
	     tmpStrs1.clear();
	     StringUtil::split(tmpStrs[1],",",tmpStrs1); 
	
	     for(int i=0;i<tmpStrs1.size();i++)
	     {
		 	len	= tmpStrs1[i].size();
		 	if ( ( ndx = tmpStrs1[i].find("=") ) < 0 )
		 	{
		 	    return false;
		 	}
			m_srcFieldValues.push_back( lrTrim( tmpStrs1[i].substr(0,ndx) ) );
	     	string tmpStr = lrTrim( tmpStrs1[i].substr(ndx+1,len-ndx-1) );
		 	if( tmpStr.compare(EMPTY) == 0 )
		 	{
		 		tmpStr = "";	
		 	}	
		 	m_desFieldValues.push_back( tmpStr );	
	     }
		 this->getFieldObj();
	     return true;
    }
    //example: RULE_1:CALL_TYPE=01;FIELD=msisdn_flag;05=91,06=A1,OTHER=81
    else if(tmpStrs.size() == 3)
    {
        tmpStrs[0] = tmpStrs[0].substr(7);
        tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
       
        if (tmpStrs1[0].compare(0,strlen(CALL_TYPE),CALL_TYPE) == 0)
        {
            m_callType = tmpStrs1[1];
        }
        else if(tmpStrs1[0].compare(0,strlen(DEAL_OBJ_NAME),DEAL_OBJ_NAME) == 0)
        {
            m_srcFieldName = tmpStrs1[1];
        }
        
        tmpStrs1.clear();StringUtil::split(tmpStrs[1],"=",tmpStrs1);
        if (tmpStrs1[0].compare(0,strlen(CALL_TYPE),CALL_TYPE) == 0)
        {
            m_callType = tmpStrs1[1];
        }
        else if(tmpStrs1[0].compare(0,strlen(DEAL_OBJ_NAME),DEAL_OBJ_NAME) == 0)
        {
            m_srcFieldName = tmpStrs1[1];
        }
   
        tmpStrs1.clear();StringUtil::split(tmpStrs[2],",",tmpStrs1);
	
        for(int i=0;i<tmpStrs1.size();i++)
        {
		    len	= tmpStrs1[i].size();
		    if ( ( ndx = tmpStrs1[i].find("=") ) < 0 )
		    {
			    return false;
			}
		    m_srcFieldValues.push_back( lrTrim( tmpStrs1[i].substr(0,ndx) ) );
		    string tmpStr = lrTrim( tmpStrs1[i].substr(ndx+1,len-ndx-1));
		    if( tmpStr.compare(EMPTY) == 0 )
		    {
		    	tmpStr = "";	
		    }
		    m_desFieldValues.push_back( lrTrim( tmpStr ));	
        }
        this->getFieldObj();
	    return true;
    }
    else
    {
        return false;
    }
}

//##ModelId=485614C00213
bool FieldConvertRule_1::convertField()
{
	if( m_pSrcFieldInfo->m_iDesValueLen == 0 )
	{
	    return false;
	}
	if ( m_pFieldCallType->m_iDesValueLen == 0 )
	{
	    return false;
	}
	
	if( m_callType.compare(CONDITION_ALL)!=0 )            //呼叫类型不为ALL
	{
	    if( strcmp(m_pFieldCallType->m_FieldDesValue,m_callType.c_str())!=0 )    //不符合呼叫类型条件
	    {
	        return false;
	    }
	}
	
	for(int i=0;i<m_srcFieldValues.size();++i)
	{
		if( strcmp(m_pSrcFieldInfo->m_FieldDesValue,m_srcFieldValues[i].c_str())==0 ) 
		{
		    m_pSrcFieldInfo->setDesValue(m_desFieldValues[i]);
			break;
		} 
		else if( m_srcFieldValues[i].compare(OTHER)==0 ) 
		{
            m_pSrcFieldInfo->setDesValue(m_desFieldValues[i]);
            break;
		}
	}

	return true;
}

//##ModelId=485614D20157
FieldConvertRule_1::~FieldConvertRule_1()
{
}

void FieldConvertRule_1::getFieldObj()
{
	m_pSrcFieldInfo = m_pFieldInfos->getFieldInfoByName(m_srcFieldName.c_str());
	
	return FieldConvertRule::getFieldObj();	
}
