#ifndef _FILTERCDR_H_INLCUDE_20080821_
#define _FILTERCDR_H_INLCUDE_20080821_

#include "cdr/CdrBase.h"

class FilterCdr:public CdrBase
{
  public:
     FilterCdr();
    virtual ~FilterCdr();
  public:
    virtual bool reset();       //读取话单前初始化
    virtual bool canonicalize();//读取话单后规整
    virtual bool finalize();    //话单流程处理结束后规整
  private:
    T_ATTRI_VAL m_beginDate;
    T_ATTRI_VAL m_beginTime;
};

#endif // _FILTERCDR_H_INLCUDE_20080821_
