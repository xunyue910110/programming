#ifndef _GSMCDRPROC_H_200800723_
#define _GSMCDRPROC_H_200800723_

#include "config-all.h"

#include "ProcessBase.h"
#include "CDRProc.h"

class GsmCDRProc:public CDRProc
{
  public:
    virtual int execute();

  private:
  	//属性初始化
  	bool InitCdrProperty();
  	//trash求取
  	bool getTrash();
  	//分通道属性
  	bool getDisChannelProperty();
  	
  	bool getNational();

  private:

};

#endif //_GSMCDRPROC_H_200800723_
