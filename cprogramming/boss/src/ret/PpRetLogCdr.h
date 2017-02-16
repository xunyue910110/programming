// Language:    C++
// OS Platform: UNIX
// Authors:     cheng haidong
// History:
// Copyright (C) Linkage. All Rights Reserved.
// Comments:
#ifndef PPRETLOGCDR_H_INCLUDED_C4864891
#define PPRETLOGCDR_H_INCLUDED_C4864891

#include "RetLogRecord.h"
#include "FormatConversion.h"

class PpRetLogCdr : public RetLogRecord
{
 public:
  PpRetLogCdr();
  ~PpRetLogCdr();
 
 protected:
  //virtual int getLogInfo(string *sRetLogCdr);
  virtual int getErrInfo();
  
 private:
  int getlength( const char *buf ,char split);
};

#endif /* PPRETLOGCDR_H_INCLUDED_C4353D06 */


