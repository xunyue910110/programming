#ifndef __MISCATTRIBUNPROC_H__
#define __MISCATTRIBUNPROC_H__

#include "config-all.h"

#include "AttribUNProc.h"

class MiscAttribUNProc: public AttribUNProc
{
  public:
    virtual int execute();
  protected:
  	void initialattr();
};



#endif //__MISCATTRIBUNPROC_H__

