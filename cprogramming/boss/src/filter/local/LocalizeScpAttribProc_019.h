#ifndef __LOCALIZEScpATTRIBPROC_019_H__
#define __LOCALIZEScpATTRIBPROC_019_H__

#include "config-all.h"
#include "FilterError.h"
#include "AttribUNProc.h"
#include "LocalConstDef_019.h"

class ScpGsmAttribProc:public AttribUNProc
{
  public:
    virtual int execute();

  protected:
    void setScpRoamType(void);
    int setScpLongType() ;
    int setNoNeedTp();

};

#endif
