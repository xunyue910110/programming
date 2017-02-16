// ##########################################
// Source file : FieldInfo.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "FieldInfo.h"
#include <memory.h>
#include <iostream>

//##ModelId=4850CE560157
FieldInfo::FieldInfo()
{
    m_errorCode     = 0;
    m_iSrcValueLen  = 0;
    m_iDesValueLen  = 0;
    m_iMaxSrcValueLen = 0;
	  m_iMaxDesValueLen =0;

    m_padding       = 0;
    m_order         = 0;
    m_FieldLength   = 0;      //added by zhonghai 2011/2/15 10:13:01

    m_FieldSrcValue = NULL;
    m_FieldDesValue = NULL;
    m_dataType      = "";
    
    //针对只出现tag码,且valueSize为0的情况 by zaiww 20090925
	m_tagAll		= false;
}

FieldInfo::FieldInfo(const FieldInfo &fieldInfo)
{
    m_iSrcValueLen = fieldInfo.m_iSrcValueLen;
    m_iDesValueLen = fieldInfo.m_iDesValueLen;
    
    if(m_iSrcValueLen==0)
    {
    	m_FieldSrcValue = NULL;
    }
    else
    {
    	m_FieldSrcValue = new unsigned char[m_iSrcValueLen];
    	memcpy(m_FieldSrcValue,fieldInfo.m_FieldSrcValue,m_iSrcValueLen); 	
    }
    if(m_iDesValueLen==0)
    {
    	m_FieldDesValue = NULL;
    }
    else
    {
    	m_FieldDesValue = new char[m_iDesValueLen];
    	memcpy(m_FieldDesValue,fieldInfo.m_FieldDesValue,m_iDesValueLen); 	
    }
   
    m_IsFormat   = fieldInfo.m_IsFormat;
    m_IsCheck    = fieldInfo.m_IsCheck;
    m_errorCode  = fieldInfo.m_errorCode;
    m_dataType   = fieldInfo.m_dataType;
    m_padding    = fieldInfo.m_padding;
    m_order      = fieldInfo.m_order;
	m_tagAll     = fieldInfo.m_tagAll;
}

//##ModelId=4850CE670251
FieldInfo::~FieldInfo()
{
    if(m_FieldSrcValue != NULL)
    {
    	delete m_FieldSrcValue;
    }
	
	if(m_FieldDesValue != NULL)
	{
		delete m_FieldDesValue;	
	}    
}

void FieldInfo::reset()
{
    m_errorCode     = 0;
    m_iSrcValueLen   = 0;
    m_iDesValueLen   = 0;

	memset(m_FieldSrcValue,0,m_iSrcValueLen);
	memset(m_FieldDesValue,0,m_iSrcValueLen);	
	return;
}

void FieldInfo::setDesValue(string &value)
{
	if(value.length()> m_iMaxDesValueLen)
	{
		if(m_FieldDesValue != NULL)
		{
			delete m_FieldDesValue;	
		}
		m_iMaxDesValueLen = value.length()+1;
		m_FieldDesValue = new char[m_iMaxDesValueLen];	
	}
	if(value.length()>0 )
	{
		strcpy(m_FieldDesValue,value.c_str());
	}
	m_iDesValueLen = value.length();
	
	return;	
}

void FieldInfo::setDesValue(const char *value,int iStrLength)
{
	if(iStrLength==-1)
	{
		iStrLength = strlen(value);	
	}
	
	if(iStrLength> m_iMaxDesValueLen)
	{
		if(m_FieldDesValue != NULL)
		{
			delete m_FieldDesValue;	
		}
		m_iMaxDesValueLen = iStrLength+1;
		m_FieldDesValue = new char[m_iMaxDesValueLen];	
		memset(m_FieldDesValue,0,m_iMaxDesValueLen);
	}
	memcpy(m_FieldDesValue,value,iStrLength);
	m_FieldDesValue[iStrLength] = 0;
	m_iDesValueLen = iStrLength;
	
	return;	
}


void FieldInfo::setSrcValue(const unsigned char *value,const int iLength)
{
	if(iLength > 0)
	{
		if(iLength> m_iMaxSrcValueLen)
		{
			if(m_FieldSrcValue != NULL)
			{
				delete m_FieldSrcValue;	
			}
			m_iMaxSrcValueLen = iLength+1;
			m_FieldSrcValue = new unsigned char[m_iMaxSrcValueLen];
				
		}
		memcpy(m_FieldSrcValue,value,iLength);
		m_iSrcValueLen = iLength;
	}
	return;		
}

void FieldInfo::reserveDesStr(int &iLength)
{
	if(iLength> m_iMaxDesValueLen)
	{
		if(m_FieldDesValue != NULL)
		{
			delete m_FieldDesValue;	
		}
		m_iMaxDesValueLen = iLength+1;
		m_FieldDesValue = new char[m_iMaxDesValueLen];
			
	}
	
	return;	
}
