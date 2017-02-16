// ############################################
// Source file : IndexMessage.h
// Version     : 0.1.0
// Language       : ANSI C++
// OS Platform : UNIX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20031028
// Update      : 20031028
// Copyright(C): 2003 by Yang Zhang, Linkage.
// ############################################     

#include "IndexMessage.h"  
#include "QueryUseExpression.h"

//##ModelId=3F9CC6C3017F
IndexMessage::IndexMessage()
{    
    m_queryUseExpression = 0;
}

IndexMessage::IndexMessage(const IndexMessage &indexMessage)
{
	m_order 				= indexMessage.m_order;
	m_fieldId   			= indexMessage.m_fieldId;
	m_isCompress			= indexMessage.m_isCompress;
	m_queryUseExpression	= indexMessage.m_queryUseExpression;
}


//##ModelId=3F9CC6C40393
IndexMessage::~IndexMessage()
{  
    m_queryUseExpression = 0;
}

