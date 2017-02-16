#ifndef _GSMCDRUNPROC_H_20080723_
#define _GSMCDRUNPROC_H_20080723_

#include "config-all.h"

#include "ProcessBase.h"
#include "CDRProc.h"

class GsmCDRUNProc:public CDRProc
{
  public:
    virtual int execute();

  protected:
  	//属性初始化
  	bool InitCdrProperty();
  	//trash求取
  	bool getTrash();
  	//分通道属性
  	bool getDisChannelProperty();

  	bool getNational();

  protected:

};

#endif //_GSMCDRUNPROC_H_20080723_
