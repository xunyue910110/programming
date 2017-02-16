#ifndef _SCPSMCDRPROC_H_200800723_
#define _SCPSMCDRPROC_H_200800723_

#include "config-all.h"

#include "ProcessBase.h"
#include "CDRProc.h"

class ScpSmCDRProc:public CDRProc
{
  public:
    virtual int execute();

  protected:

};

#endif //_SCPSMCDRPROC_H_200800723_
