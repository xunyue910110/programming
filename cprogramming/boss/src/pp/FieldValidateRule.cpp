// ##########################################
// Source file : FieldValidateRule.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080921
// Update      : 20080922
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldValidateRule.h"
#include "FieldInfos.h"


//##ModelId=4856436500FA
FieldValidateRule::FieldValidateRule(const int iSubChannelNum)
{
    m_PpLog_  = PpLog::getInstance();
    m_cdrFlag = CdrFlag::getInstance();
    m_pFieldInfos = FieldInfos::getInstance(iSubChannelNum);
}

//##ModelId=48564407006D
void FieldValidateRule::substr(string &srcString, string &desString)
{
	int len,size;
	
	size=srcString.size();
	
	if(m_subLength==0) 
	{
		if(size>m_substart)
		{
		   len=size - m_substart;
		}
		else
		{
		   return ;
		}
	} 
	else 
	{
		len=m_subLength;
	}
	
	desString=srcString.substr(m_substart,len);
}

//##ModelId=48564425034B
void FieldValidateRule::substr(char * src, char *des)
{
    int len;
	
	if(m_subLength==0)
	{
	   len=strlen(src)-m_substart;
	}
	else
	{
	   len=m_substart;
	}
	
	if(len<=0)
	{
	    return ;
	}
	memcpy(des,src+m_substart,len);
	des[len]=0;
}

//##ModelId=4856443A033C
bool FieldValidateRule::setSubstr(string &src)
{
	string tmp;
	int len,ndx1,ndx2;
	
	// example :   substr(1,8)
	if ( ( ndx1  = src.find("(") ) < 0 )
		return false;
	if ( ( ndx2  = src.find(")") ) < 0 )
		return false;
	
	tmp=lrTrim( src.substr(ndx1+1,ndx2-ndx1-1) );
	
	if ( ( ndx1  = tmp.find(",") ) < 0 )
		return false;
	
	m_substart  = atoi(tmp.substr(0,ndx1).c_str())-1;
	m_subLength = atoi(tmp.substr(ndx1+1,tmp.size()-ndx1-1).c_str());
	
	if(m_substart<0 || m_subLength<0) 
	{
		return false;
	}
	
	return true;
}

void FieldValidateRule::getFieldObj()
{
	m_pFieldCallType     = m_pFieldInfos->getFieldInfoByName(CALL_TYPE);
	m_pFieldCondition    = m_pFieldInfos->getFieldInfoByName(m_conditionFieldName.c_str());
	
	return;
}

bool FieldValidateRule::generalCondJudge()								
{
    // 条件1：通话类型
    if(m_callType.compare(CONDITION_ALL)!=0) 
    {
        if( m_pFieldCallType->m_iDesValueLen == 0
        	||strcmp(m_pFieldCallType->m_FieldDesValue,m_callType.c_str())!=0 ) 
        {
            return false;
        }
    }

    // 条件2：其他字段值
	if(m_conditionFieldName.compare(CONDITION_ALL)!=0) 
	{      // not is "ALL"
	    if( m_pFieldCondition->m_iDesValueLen==0 )
	    {
		    return false;
		}
		if(m_conditionFieldValue.compare(CONDITION_ALL)!=0) 
		{
            if(strcmp(m_pFieldCondition->m_FieldDesValue,m_conditionFieldValue.c_str())!=0) 
            {
               return false;
            }
        }
    }
    return true;	
}

//##ModelId=4856444D005D
string  FieldValidateRule::lrTrim(string theString)
{
    int len,size;

	size	= theString.size();
	len	= theString.find_first_not_of(" \t");
	if(len>0)
		theString	= theString.substr(len,size-len);	// 去掉头部空格和Tab
	len	= theString.find_last_not_of(" \t");
	if(len>0)
		theString	= theString.substr(0,len+1);		   // 去掉尾部空格和Tab

	return theString;
}
