#ifndef __SMSATTRIBPROC_H__
#define __SMSATTRIBPROC_H__

#include "config-all.h"

#include "AttribCMProc.h"

class SmsAttribProc:public AttribCMProc
{
  public:
    virtual int execute();
  protected:
  	bool getSmAHByMsisdn(); //获取A的归属
  	bool getSmBHByMsisdn(); //获取B的归属
};


#endif

