// ##########################################
// Source file : RepeatableFieldInfo.cpp
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

#include "RepeatableFieldInfo.h"
#include <memory.h>
#include <iostream>

//##ModelId=4850CE560157
RepeatableFieldInfo::RepeatableFieldInfo()
{
	m_iNum       = 0;
	m_fieldInfoVect.clear();
}

//##ModelId=4850CE670251
RepeatableFieldInfo::~RepeatableFieldInfo()
{
	m_iNum       = 0;
    for(int i=0;i<m_fieldInfoVect.size();++i)
    {
    	delete m_fieldInfoVect[i];	
    }	
	m_fieldInfoVect.clear();
}

void RepeatableFieldInfo::reset()
{
    for(int i=0;i<m_iNum;++i)
    {
    	m_fieldInfoVect[i]->reset();	
    }
    m_iNum     = 0;
    
	return;
}

void RepeatableFieldInfo::setDesValue(string &value)
{
	if(m_iNum >= m_fieldInfoVect.size())
	{
		FieldInfo *pFieldInfo ;
		pFieldInfo = new FieldInfo();
	    pFieldInfo->m_padding      = m_padding;
	    pFieldInfo->m_order        = m_order;   
	    pFieldInfo->m_dataType     = m_dataType;  
		m_fieldInfoVect.push_back(pFieldInfo);
	}
	m_fieldInfoVect[m_iNum]->setDesValue(value);
	++m_iNum;
	
	return;	
}

void RepeatableFieldInfo::setDesValue(const char *value)
{
	if(m_iNum >= m_fieldInfoVect.size())
	{
		FieldInfo *pFieldInfo ;
		pFieldInfo = new FieldInfo();
	    pFieldInfo->m_padding      = m_padding;
	    pFieldInfo->m_order        = m_order;  
	    pFieldInfo->m_dataType     = m_dataType;     
		m_fieldInfoVect.push_back(pFieldInfo);
	}
	m_fieldInfoVect[m_iNum]->setDesValue(value);
	++m_iNum;
	
	return;	
}

void RepeatableFieldInfo::setSrcValue(const unsigned char *value,const int iLength)
{
	if(m_iNum >= m_fieldInfoVect.size())
	{
		FieldInfo *pFieldInfo ; 
		pFieldInfo = new FieldInfo();
	    pFieldInfo->m_padding      = m_padding;
	    pFieldInfo->m_order        = m_order;   
	    pFieldInfo->m_dataType     = m_dataType;  
		m_fieldInfoVect.push_back(pFieldInfo);
	}
	m_fieldInfoVect[m_iNum]->setSrcValue(value,iLength);
	++m_iNum;

	return;		
}

int RepeatableFieldInfo::getNumCount()
{
	return m_iNum;	
}
