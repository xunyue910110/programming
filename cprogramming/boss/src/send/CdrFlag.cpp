// ##########################################
// Source file : CdrFlag.cpp
// System      : Mobile Billing System
// Version     : 0.1.4
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010928
// Update      : 20011122
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "CdrFlag.h"

//##ModelId=3BB429AB021A
CdrFlag *CdrFlag::m_cdrFlag=0;

//##ModelId=3BB42A560324
void CdrFlag::clear()
{
  m_errorCode	   = 0;
  m_errcode_pos  = 0; 
	m_callType[0]  = 0;
	m_isInvalid	   = false;
	m_isError	   = false;
	m_errorFlag    = false;
	m_cdrNo        = -1;
	//m_fieldName.clear();
	m_fieldName=""; //by chenmin 08-27 for hp
	m_srcCdr="";
	m_fileHeadSize = 0;
	m_totalFee=0;
}

//##ModelId=3BB42ADF02F9
CdrFlag *CdrFlag::getInstance()
{
	if ( m_cdrFlag == 0 )
		m_cdrFlag= new CdrFlag;

	return m_cdrFlag;
}

//##ModelId=3BFCACC90351
CdrFlag::CdrFlag()
{
   m_cdrNo = -1;
}

//##ModelId=3BFCAD1602EE
CdrFlag::~CdrFlag()
{
}

