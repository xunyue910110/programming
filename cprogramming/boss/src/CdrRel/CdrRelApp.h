#ifndef __CDRRELAPP_H__20110411_
#define __CDRRELAPP_H__20110411_

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "dbi/oradbi.h"
#include "base/Directory.h"
#include <map>
#include <vector>
#include "CdrRelCommon.h"


class CdrRelApp: public Application 
{
  public:
    CdrRelApp();
    virtual ~CdrRelApp();
    bool CdrRelRun();
    bool getCdr();
    bool cdrIndb();
    bool execProc(const FileType &fileType);
    bool outputCdr(const FileType &fileType);
  protected:
    virtual bool initialization();
    virtual bool beforeLoop();
    virtual bool loopProcess();
    virtual bool end();
  private:
    CdrRelConfig   m_config;
    DbInterface    m_dbinterface;
    vector<string> m_cdr;
    //目录管理
    Directory      m_dir;
    string         m_fullFileName;
    string         m_fileName;
};

#endif //__CDRRELAPP_H__20110411_
