// ##########################################
// Source file : FieldConvertRule_3.cpp
// System      : Mobile Billing System
// Version     : 0.1.6
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080903
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

#include "FieldConvertRule_3.h"
#include "FieldInfos.h"


//##ModelId=4856163A02BF
bool FieldConvertRule_3::initialize(string ruleText)
{
	vector<string> tmpStrs,tmpStrs1;

	// example : RULE_3:CALL_TYPE=ALL;MSRN_FLAG,MSRN,1
	StringUtil::split(ruleText,";",tmpStrs);
	
	if(tmpStrs.size() != 2)
	{
		// 小于1个分号的,不符合规则的书写格式
		return false;	
	}
	// 提取规则中的信息
	tmpStrs[0] = tmpStrs[0].substr(7);
	
	if(tmpStrs[0].compare(0,strlen(CALL_TYPE),CALL_TYPE)==0)
	 {
         tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);
         m_callType = tmpStrs1[1];
	 }
	else
	 {
	    return false;
	 }
	
	
	// 提取 MSRN_FLAG,MSRN,1
	tmpStrs1.clear();StringUtil::split(tmpStrs[1],",",tmpStrs1);
	if( tmpStrs1.size() == 3)
	{
		m_phoneFlag = tmpStrs1[0];
		m_phone     = tmpStrs1[1];
		m_order     = atoi(tmpStrs1[2].c_str());
	}
	else if( tmpStrs1.size() == 2 )
	{
		m_phoneFlag = tmpStrs1[0];
		m_phone     = tmpStrs1[1];
		m_order     = 1;
	}	
	else
	{
		return false;	
	}
	this->getFieldObj();

	return true;
}

//##ModelId=4856164200BB
bool FieldConvertRule_3::convertField()
{
    int i,len;
	int zeroCount;
	char src[30],des[30];
	static string distanceCode;
	string tmpStr;
	
	if( m_pFieldCallType->m_iDesValueLen == 0 )
		return false;
	
	// 条件：通话类型，“包括：国内、国际、本地”
	if(m_callType.compare(CONDITION_ALL)!=0)
	{
	   if( strcmp(m_pFieldCallType->m_FieldDesValue,m_callType.c_str() ) != 0 ) 
	   {
	      return false;
	   }
	}
	
	if( m_pFieldPhoneFlag->m_iDesValueLen == 0 
		||m_pFieldPhone->m_iDesValueLen == 0) 
	{
		return false;
	}

	if( strcmp(m_pFieldPhoneFlag->m_FieldDesValue,"91")==0 )          // 国际电话
	{
	   distanceCode = "00";
	} 
	else if(strcmp(m_pFieldPhoneFlag->m_FieldDesValue,"A1")==0)   // 国内电话
	{
	   distanceCode = "0";
	} 
	else 
	{
	   return true;
	}

    // modified by chenm 2003-11-3 对国际长途标识的单独处理
    if( (strcmp(m_pFieldPhoneFlag->m_FieldDesValue,"91")==0) 
    	&& (m_pFieldPhone->m_iDesValueLen >= 2) )
    {
    	//  不是'00'开头的号码，加'00'
    	if( (m_pFieldPhone->m_iDesValueLen >= 2)
    		&& (strncmp(m_pFieldPhone->m_FieldDesValue,"00",2)!=0) )
    	{
    		tmpStr = string("00") + m_pFieldPhone->m_FieldDesValue;
    		m_pFieldPhone->setDesValue(tmpStr);  	  
    	}
    }
    else if(strcmp(m_pFieldPhoneFlag->m_FieldDesValue,"A1")==0)
    {
    	zeroCount = mvZero(m_pFieldPhone->m_FieldDesValue,des);       // 去掉开始的"0"
    	
		if( (strncmp(des,"13",2)==0)||(strncmp(des,"14",2)==0)||(strncmp(des,"15",2)==0)||(strncmp(des,"18",2)==0) ) 
		{
		   m_pFieldPhone->setDesValue(des);    	
		   return true;
		}
    	
		if( strncmp(des,"86",2)!=0 ) 
		{
			if( (zeroCount>0) && (zeroCount!=distanceCode.size()) )  
			{
			   if(m_order==1) 
			   {
                   m_pFieldPhone->setDesValue( (distanceCode + des).c_str() );
			   }
			} 
			else 
			{
			    m_pFieldPhone->setDesValue( (distanceCode + des).c_str() );
			}
		} 
		else 
		{
		   len = strlen(des) - 2;
		   if( len <= 0) 
		   {
		      return false;
		   }
		   memcpy(src,des+2,len);                          // 去掉"86"
		   src[len] = 0;
    	   mvZero(src,des);                                // 去掉开始的"0"
		   if( (strncmp(des,"13",2)!=0)&&(strncmp(des,"14",2)!=0)&&(strncmp(des,"15",2)!=0)&&(strncmp(des,"18",2)!=0) ) 
		   {
		      tmpStr = string("0") + des;
		      m_pFieldPhone->setDesValue( tmpStr.c_str() ); // is 国内长途
		   } 
		   else 
		   {
		      m_pFieldPhone->setDesValue(des);
		   }
		}
	}

	return true;
}

//##ModelId=4856164C0148
int FieldConvertRule_3::mvZero(string src, char *des)
{
    int i,len;
    char tmp[40];
    
    len=src.size();
    strcpy(tmp, src.c_str());
    for(i=0;i<len;i++)
    {
        if(tmp[i] != '0')
        {
            break;
        }
    }
    len -= i;
    memcpy(des,tmp+i,len);
    des[len]=0;
    
    return i;
}

void FieldConvertRule_3::getFieldObj()
{
    m_pFieldPhoneFlag = m_pFieldInfos->getFieldInfoByName(m_phoneFlag.c_str());  
    m_pFieldPhone     = m_pFieldInfos->getFieldInfoByName(m_phone.c_str());        	
	
	return FieldConvertRule::getFieldObj();		
}
