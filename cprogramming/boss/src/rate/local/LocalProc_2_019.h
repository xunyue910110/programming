#ifndef _LOCALPROC019_H_INCLUDE_20081020_
#define _LOCALPROC019_H_INCLUDE_20081020_

#include "Procedure.h"
#include "config-all.h"
#include "RateGlobalInfo.h"

//读取话单后的一些初始化处理
class RateCdrAttribInit:public Procedure
{
  public:
    virtual int execute();
};
//话单批价处理结束后的一些特殊处理
class RateCdrAttribEnd:public Procedure
{
  public:
    virtual int execute();
};

//判断是否需要参加套餐优惠,特殊处理
class IsNoNeedTPSpecialProc:public Procedure
{
  public:
    virtual int execute();
};

//GSM一些免费标记设置(比如3秒免费等)
class LocalizeGsmFreeFlagProc:public Procedure
{
  public:
    virtual int execute();
};
//设置那些费用项有原始费用
class SetOriginFeeFlag:public Procedure
{
  public:
	  virtual int execute();
};
//特殊合帐过程
class SpecialBillProc:public Procedure
{
  public:
	  virtual int execute();
};

#endif //_LOCALPROC019_H_INCLUDE_20081020_

