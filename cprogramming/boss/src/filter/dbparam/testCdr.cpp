#include "testCdr.h"
#include "FilterError.h"
#include "ConstDef.h"

#include "base/TimeUtil.h"
#include "base/StringUtil.h"

FilterCdr::FilterCdr()
{
}
FilterCdr::~FilterCdr()
{
}

//读取话单前一些属性初始化
bool FilterCdr::reset() 
{
  bool t_bRet = true;
  if(CdrBase::reset()==false)
  {
    t_bRet = false;
  }
  
  return t_bRet;
}
//读取话单后规整
bool FilterCdr::canonicalize()
{
  bool t_bRet = true;
  if(CdrBase::canonicalize()==false)
  {
    t_bRet = false;
  }

  return true;
}

bool FilterCdr::finalize()
{
  bool t_bRet = true;
  if(CdrBase::finalize()==false)
  {
    t_bRet = false;
  }
  return true;
}

