// Language:    C++
// OS Platform: UNIX
// Authors:     cheng haidong
// History:
// Copyright (C) Linkage. All Rights Reserved.
// Comments:
#include "PpRetLogCdr.h"

PpRetLogCdr::PpRetLogCdr()
{
  initialize();	
}

PpRetLogCdr::~PpRetLogCdr()
{}

int PpRetLogCdr::getErrInfo()
{
  int location = 0;
  char cTemp[20];
  char separator = ',';
  string tmpstr;
  int cdrLength;
  int len = 0;
  //char rec[500];
  
  //rec = m_sRetLogRecord;
 // memset(rec,0,sizeof(rec));
  //strcpy(rec,m_sRetLogRecord->c_str());
  cdrLength = m_sRetLogRecord->length();
  
  //ret_head
  tmpstr = m_sRetLogRecord->substr(location,cdrLength - location);
  len = getlength(tmpstr.c_str(),separator);
  m_retInfo.ret_head = m_sRetLogRecord->substr(location,len);
  location += len;
  location += 1;
    
  //error_type
  tmpstr = m_sRetLogRecord->substr(location,cdrLength - location);
  len = getlength(tmpstr.c_str(),separator);
  m_retInfo.error_type = m_sRetLogRecord->substr(location,len);
  location += len;
  location += 1;
  
  //error_code
  tmpstr = m_sRetLogRecord->substr(location,cdrLength - location);
  len = getlength(tmpstr.c_str(),separator);
  m_retInfo.error_code = m_sRetLogRecord->substr(location,len);
  location += len;
  location += 1;

  //ret_name
  tmpstr = m_sRetLogRecord->substr(location,cdrLength - location);
  len = getlength(tmpstr.c_str(),separator);
  m_retInfo.ret_name = m_sRetLogRecord->substr(location,len);
  location += len;
  location += 1;

  //ret_tail
  tmpstr = m_sRetLogRecord->substr(location,sizeof(m_sRetLogRecord->c_str()) - location);
  len = getlength(tmpstr.c_str(),separator);
  m_retInfo.ret_tail = m_sRetLogRecord->substr(location,len);  
  
  #ifdef _DEBUG_
  cout<<"m_retInfo.ret_head:"<<m_retInfo.ret_head<<endl;
  cout<<"m_retInfo.error_type:"<<m_retInfo.error_type<<endl;
  cout<<"m_retInfo.error_code:"<<m_retInfo.error_code<<endl;
  cout<<"m_retInfo.ret_name:"<<m_retInfo.ret_name<<endl;  
  cout<<"m_retInfo.ret_tail:"<<m_retInfo.ret_tail<<endl;  
  #endif

  return 1;
}

int PpRetLogCdr::getlength( const char *buf ,char split)
{
	const char *pdest ;
	const char *phead = buf;
	int length;

  pdest = strchr( phead, split );
	length = pdest - phead;
    
  return length;
}


