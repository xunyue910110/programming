#ifndef _SPPRICEPROC_UNICOM_H_INCLUDE_20081210_
#define _SPPRICEPROC_UNICOM_H_INCLUDE_20081210_
#include "PriceBaseProc.h"

class SpPriceProc_Unicom: public PriceBaseProc
{
  protected:
    virtual void setOrgFeeInfo(const int &r_feeField);//设置原始费用信息
    virtual bool getCdrSegments();//按天分割话单，形成话单天分割段列表
  private:    
};
#endif //_SPPRICEPROC_UNICOM_H_INCLUDE_20081210_


