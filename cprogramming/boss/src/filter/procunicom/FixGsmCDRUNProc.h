#ifndef _FIXGSMCDRUNPROC_H_20090325_
#define _FIXGSMCDRUNPROC_H_20090325_

#include "config-all.h"

#include "ProcessBase.h"
#include "CDRProc.h"

class FixGsmCDRUNProc:public CDRProc
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
  	//计费号码调整，加上区号
  	bool StandFixMsisdnA();

  protected:

};

#endif //_FIXGSMCDRUNPROC_H_20090325_
