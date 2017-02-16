#ifndef _MISCCDRPROC_H_200800723_
#define _MISCCDRPROC_H_200800723_

#include "config-all.h"

#include "ProcessBase.h"
#include "CDRProc.h"

class MiscCDRProc:public CDRProc
{
  public:
    virtual int execute();

  protected:

};

#endif //_MISCCDRPROC_H_200800723_
