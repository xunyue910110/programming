#ifndef OUTCHANINFO_H_20080713
#define OUTCHANINFO_H_20080713

#include "base/PString.h"
#include "cdr/CDRJScript.h"

class  OutChanInfo
{
public:
       PString m_channelNo;
       PString m_desPath;
       PString m_prefix;
       PString m_expression;
       CDRJScript *m_outRule;
      OutChanInfo()
      {
         m_channelNo = "";
         m_desPath   = "";
         m_prefix    = "";
         m_expression= "";
      }
};
#endif // OUTCHANINFO_H_20080713
