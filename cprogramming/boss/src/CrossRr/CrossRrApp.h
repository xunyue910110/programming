#ifndef CROSSRRAPP_H_HEADER_INCLUDED_B7642005
#define CROSSRRAPP_H_HEADER_INCLUDED_B7642005

#include <sys/time.h>
#include <unistd.h>
#include "CrossRr.h"
#include "base/SysParam.h"

class CrossRrApp
{
  public:
    CrossRrApp(const int argc, char **argv);
    ~CrossRrApp();
    int run();
  private:
    void getCfgFileName();
    bool getSysParam();
    bool parseOpt();
    bool getParamValueByName(const string &name, string &value);
  private:
    int      m_argc;
    char     **m_argv;
    CrossRr  m_crossRr;
    DBINFO   m_paramDbInfo;
    DBINFO   m_srcDbInfo;
    DBINFO   m_desDbInfo;
    string   m_whereClause;
    SysParam *m_theSysParam;
    string   m_cfgFileName;
    int      m_channelNo;
    int      m_beginDate;
    int      m_endDate;
    int      m_month;
};

#endif
