// ##########################################
// Source file : TokenFormat.cpp
// System      : Mobile Billing System
// Version     : 1.03
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010823
// Update      : 20020529
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include "TokenFormat.h"

//##ModelId=3B60229202FE
void TokenFormat::getContents()
{
}

//##ModelId=3B84FD0D02B8
int TokenFormat::getCdrSize(const char *theSign,const char *cdrTmp,int &aCdrOffset)
{
	return 0;
}

//##ModelId=3B9587360244
int TokenFormat::getValue(unsigned char * theSign,unsigned char *aValue,int position)
{
	return 0;
}

//##ModelId=3B96E4350222
void TokenFormat::initialize()
{
	;
}


//##ModelId=3C048D7702B8
TokenFormat::TokenFormat()
{
    m_FormatType=2;
}

