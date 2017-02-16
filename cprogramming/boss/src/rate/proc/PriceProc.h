#ifndef _PRICEPROC_H_INCLUDE_20081007_
#define _PRICEPROC_H_INCLUDE_20081007_

#include "PriceBaseProc.h"

class PriceProc : public PriceBaseProc
{  
  protected:
    virtual void setOrgFeeInfo(const int &r_feeField);//设置原始费用信息
  private:    
    
    //一些批价后的特殊处理
    bool specialProcess();
  protected:
    
};

#endif // _PRICEPROC_H_INCLUDE_20081007_

