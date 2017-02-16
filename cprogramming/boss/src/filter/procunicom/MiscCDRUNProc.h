#ifndef _MISCCDRUNPROC_H_200800723_
#define _MISCCDRUNPROC_H_200800723_

#include "config-all.h"

#include "ProcessBase.h"
#include "CDRProc.h"

class MiscCDRUNProc:public CDRProc
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

  	bool getNationalMisc();

  protected:

};

#endif //_MISCCDRUNPROC_H_200800723_
