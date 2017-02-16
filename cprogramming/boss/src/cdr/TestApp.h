#ifndef _TESTAPP_H_INCLUDE_20080731_
#define _TESTAPP_H_INCLUDE_20080731_

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "CdrFmtConfig.h"
#include "CdrBase.h"

class TestApp: public Application 
{
  public:
    TestApp();
   ~TestApp();
  protected:
    virtual bool initialization();
    virtual bool beforeLoop();
    virtual bool loopProcess();
    virtual bool end();

    bool   testFunc();
  protected:
    CdrFmtConfig  m_config; ///<话单格式配置信息
    CdrBase       m_cdr;
};
#endif //_TESTAPP_H_INCLUDE_20080731_

