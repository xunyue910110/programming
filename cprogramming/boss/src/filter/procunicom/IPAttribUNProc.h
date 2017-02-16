#ifndef __IPAttribUNProc_H__
#define __IPAttribUNProc_H__

#include "config-all.h"
#include "AttribUNProc.h"

class IPAttribUNProc:public AttribUNProc
{
  public:
    virtual int execute();

  protected:
  	void initialattr();
    
    int m_channelNo; 
};

#endif
