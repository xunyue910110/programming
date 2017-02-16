#ifndef __FIXSCPGSMATTRIBUNPROC_H__
#define __FIXSCPGSMATTRIBUNPROC_H__

#include "config-all.h"
#include "FixGsmAttribUNProc.h"

class FixScpGsmAttribUNProc:public FixGsmAttribUNProc
{
  public:
    virtual int execute();

  protected:
    bool getScpHomeAreaCodeA();    //固网求A归属
};

#endif //__FIXSCPGSMATTRIBUNPROC_H__
