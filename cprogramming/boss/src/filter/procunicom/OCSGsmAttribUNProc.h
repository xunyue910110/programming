#ifndef __GSMATTRIBUNPROC_H__
#define __GSMATTRIBUNPROC_H__

#include "config-all.h"
#include "AttribUNProc.h"

class OCSGsmAttribUNProc:public AttribUNProc
{
  public:
    virtual int execute();

  protected:
  	void initialattr();

};

#endif
