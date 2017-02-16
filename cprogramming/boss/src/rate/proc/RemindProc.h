#ifndef __REMINDPROC_H__
#define __REMINDPROC_H__

#include "config-all.h"

#include "Procedure.h"

#include "RateCdr.h"
#include "FieldsXX.h"

class RemindProc:public Procedure 
{
  public:
    virtual int execute();
};

#endif
