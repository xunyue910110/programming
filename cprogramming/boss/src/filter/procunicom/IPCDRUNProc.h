#ifndef _IPCDRUNPROC_H_20081201_
#define _IPCDRUNPROC_H_20081201_

#include "config-all.h"

#include "ProcessBase.h"
#include "CDRProc.h"

class IPCDRUNProc:public CDRProc
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

};

#endif //_IPCDRUNPROC_H_20081201_
