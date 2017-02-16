#ifndef __LOCALIZEScpATTRIBPROC_010_H__
#define __LOCALIZEScpATTRIBPROC_010_H__

#include "config-all.h"
#include "FilterError.h"
#include "AttribUNProc.h"
#include "LocalConstDef_010.h"

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
