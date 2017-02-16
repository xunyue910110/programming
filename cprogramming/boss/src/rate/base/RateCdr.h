
#ifndef __RATECDR_H_INLCUDE_20080801_
#define __RATECDR_H_INLCUDE_20080801_

#include "cdr/CdrBase.h"


class RateCdr :public CdrBase
{
  public:
     RateCdr();
    virtual ~RateCdr();
  public:
    virtual bool reset();   //读取话单前初始化
    virtual bool canonicalize();//读取话单后规整
    virtual bool finalize();
  public:
    long    m_tatolFee;      //体现在清单中的总费用
    long    m_totalDuration; //体现在清单中的总时长
    long    m_totalData;     //流量
    long    m_baseFee;       //一次批价费用
};

#endif //__RATECDR_H_INLCUDE_20080801_

