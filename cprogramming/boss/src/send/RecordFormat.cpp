// ##########################################
// Source file : RecordFormat.cpp
// System      : Mobile Billing System
// Version     : 0.1.5
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010823
// Update      : 20011024
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "RecordFormat.h"

vector<RecordFormat *> RecordFormat::m_RecordFormatPointer;

//##ModelId=3B94E33C02BA
RecordFormat::RecordFormat()
{
    m_SendLog_	= SendLog::getInstance();
	m_FormatType = 0;
}

//##ModelId=3B94E33D0077
RecordFormat::~RecordFormat()
{
}

//##ModelId=3B94E0D00345
void RecordFormat::reset(const unsigned char *aCdrBuffer,int cdrSize,
						 int cdrOffset,int flag)
{
   	m_subOffset		= 0;
   	m_cdrSize	 	= cdrSize;
   	m_CdrBuffer 	= aCdrBuffer;
   	m_bufferOffset = cdrOffset;
}


//##ModelId=3B96FAF00374
RecordFormat *RecordFormat::getNextPointer()
{
    int len;

    len=m_RecordFormatPointer.size();
	for ( int i=0;i<len ; i++ )
	{
		if( !m_RecordFormatPointer[i]->isUsed  )
			return m_RecordFormatPointer[i];
	}
	return 0;

}
//##ModelId=3C048D2F0387
int RecordFormat::getFormatType() const
{
    return m_FormatType;
}


