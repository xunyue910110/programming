#ifndef __GSMATTRIBPROC_H__
#define __GSMATTRIBPROC_H__

#include "config-all.h"
#include "AttribCMProc.h"

class GsmAttribProc:public AttribCMProc
{
  public:
    virtual int execute();

  protected:
  	void initialattr();

};

#endif
