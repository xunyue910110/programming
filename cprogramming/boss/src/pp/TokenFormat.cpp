// ##########################################
// Source file : TokenFormat.cpp
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

#include "TokenFormat.h"

//##ModelId=4855C16A038A
TokenFormat::TokenFormat()
{
    m_FormatType = 2;
}

//##ModelId=4855C1770261
void TokenFormat::getContents()
{
}

//##ModelId=4855C18E0000
int TokenFormat::getCdrSize(const char *theSign, const char *cdrTmp, int &aCdrOffset)
{
    return 0;
}

//##ModelId=4855C1BC01C5
int TokenFormat::getValue(unsigned char * theSign, FieldInfo *pFieldInfo, int position, int theLength, bool &tagAll)
{
    return 0;
}

//##ModelId=4855C2160232
void TokenFormat::initialize()
{
    ;
}

