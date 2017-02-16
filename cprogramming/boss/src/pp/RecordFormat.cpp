// ##########################################
// Source file : RecordFormat.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "RecordFormat.h"

vector<RecordFormat *> RecordFormat::m_RecordFormatPointer;

//##ModelId=4851C3AD03C8
RecordFormat::RecordFormat()
{
    m_PpLog_	 = PpLog::getInstance();
    m_FormatType = 0;
}

//##ModelId=4851C3B400EA
RecordFormat::~RecordFormat()
{
}

//##ModelId=4851C4650157
void RecordFormat::reset(const unsigned char *aCdrBuffer, int cdrSize, int cdrOffset, int flag)
{
    m_subOffset		= 0;
   	m_cdrSize	 	= cdrSize;
   	m_CdrBuffer     = aCdrBuffer;
   	m_bufferOffset  = cdrOffset;
}

//##ModelId=4851C4F1006D
int RecordFormat::getFormatType() const
{
    return m_FormatType;
}

//##ModelId=4851C54C03C8
RecordFormat *RecordFormat::getNextPointer()
{
    int len;

    len = m_RecordFormatPointer.size();
	for (int i = 0; i < len; i++)
	{
		if(!m_RecordFormatPointer[i]->isUsed)
		{
			return m_RecordFormatPointer[i];
		}
	}
	return 0;
}

