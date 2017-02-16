// ##########################################
// Source file : CdrFlag.cpp
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

#include "CdrFlag.h"

CdrFlag *CdrFlag::m_cdrFlag = 0;

//##ModelId=4855C48703D8
void CdrFlag::clear()
{
    m_errorCode	   = 0;
	m_callType[0]  = 0;
	m_isInvalid	   = false;
	m_isError	   = false;
	m_errorFlag    = false;
	//disabled by zaiww 20090819
	//m_cdrNo        = -1;
	m_fieldName    = "";
	m_cdrFee       = 0;
	m_FHT_errorcode = "";
}

//##ModelId=4855C4990109
CdrFlag *CdrFlag::getInstance()
{
	if ( m_cdrFlag == 0)
	{
		m_cdrFlag = new CdrFlag;
	}

	return m_cdrFlag;
}

//##ModelId=4855C46F00CB
CdrFlag::CdrFlag()
{
    m_cdrNo = -1;
}
                  
//added by zaiww 20090819
void CdrFlag::resetCdrNo()
{
    m_cdrNo = -1;
}

