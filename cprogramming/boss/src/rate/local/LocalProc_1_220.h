#ifndef _LOCALPROC731_H_INCLUDE_20081020_
#define _LOCALPROC731_H_INCLUDE_20081020_

#include "Procedure.h"
#include "config-all.h"
#include "TpInfo.h"
#include "RateGlobalInfo.h"

class IsNoNeedTPSpecialProc:public Procedure
{
  public:
    virtual int execute();
};

class LocalizeGsmFreeFlagProc:public Procedure
{
  public:
    virtual int execute();
};

class SetOriginFeeFlag:public Procedure
{
  public:
	  virtual int execute();
};

#endif //_LOCALPROC731_H_INCLUDE_20081020_

