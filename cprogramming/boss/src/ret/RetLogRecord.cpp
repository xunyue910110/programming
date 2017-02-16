// Language:    C++
//
// OS Platform: UNIX
//
// Authors:     cheng haidong
//
// History:
// Copyright (C) Linkage. All Rights Reserved.
//
// Comments:
//
#include "RetLogRecord.h"
USING_NAMESPACE(std);

RetLogRecord::RetLogRecord()
{
}

RetLogRecord::~RetLogRecord()
{}

int RetLogRecord::initialize()
{
  m_retInfo.error_code = "";
  m_retInfo.error_type = "";
  m_retInfo.ret_head = "";
  m_retInfo.ret_tail = "";
  m_retInfo.ret_name = "";
  
  return 1;
}

int RetLogRecord::getLogInfo(string *rec)
{
  	int iReturn=0;

  	m_sRetLogRecord = rec;
  	iReturn = getErrInfo();

  	return iReturn;
}

int RetLogRecord::getErrInfo()
{
  return 1;
}


/*
int RetLogRecord::getRetName()
{
  return 1;
}
*/
