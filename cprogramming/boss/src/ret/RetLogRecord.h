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
#ifndef RETLOGRECORD_H_INCLUDED_C4864891
#define RETLOGRECORD_H_INCLUDED_C4864891

#include "config-all.h"
#include "FormatConversion.h"
#include <string>
USING_NAMESPACE(std);

typedef struct _RET_INFO
{
  string error_code;
  string error_type;
  string ret_head;
  string ret_tail;
  string ret_name;
}RET_INFO;

class RetLogRecord
{
 public:
  RetLogRecord();
  ~RetLogRecord();
  int getLogInfo(string *);
  int initialize();
  RET_INFO m_retInfo;
  string *m_sRetLogRecord;
  
 protected:
 	virtual int getErrInfo();
 	//virtual int getRetName();
};

#endif /* RETLOGRECORD_H_INCLUDED_C4864891 */

