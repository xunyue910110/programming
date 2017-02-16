// ##########################################
// Source file : FieldExtractRule.cpp
// System      : Mobile Billing System
// Version     : 0.1.24
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010813
// Update      : 20011128
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "FieldExtractRule.h"



//##ModelId=3B513F650258
void FieldExtractRule::getFmtValue()
{
}

//##ModelId=3B55105901E3
void FieldExtractRule::checkField()
{
}

//##ModelId=3B6215D8003B
FieldExtractRule::FieldExtractRule()
{
	m_SendLog_	          = SendLog::getInstance();
	m_padding	          = 0;
	m_order		          = 0;
	m_CompositeFieldSelf_ = 0;
	m_dataType.erase();
}

//##ModelId=3B6215D80199
FieldExtractRule::~FieldExtractRule()
{
}


//##ModelId=3B9DB40F0082
string FieldExtractRule::getFieldName() const
{
    return m_FieldName_;
}


//##ModelId=3B9DC7E1011F
string FieldExtractRule::getSectionName() const
{
    return m_SectionName_;
}


//##ModelId=3B9DC7E20329
int FieldExtractRule::getLaye() const
{
    return m_theLayer_;
}


//##ModelId=3B9DC8D7037B
void FieldExtractRule::setSectionName(string left)
{
    m_SectionName_ = left;
}

//##ModelId=3C048A91003A
FieldExtractRule * FieldExtractRule::getMySelf() const
{
    return m_CompositeFieldSelf_;
}

