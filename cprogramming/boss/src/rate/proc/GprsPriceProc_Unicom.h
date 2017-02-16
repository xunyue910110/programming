/**********支持GPRS流量单条实时方式计费************************
 **********支持GPRS时长按分业务时长累计方式计费****************
 **************************************************************
 **/
#ifndef _GPRSPRICEPROC_UNICOM_H_INCLUDE_20081121_
#define _GPRSPRICEPROC_UNICOM_H_INCLUDE_20081121_
#include "PriceBaseProc.h"

class GprsPriceProc_Unicom: public PriceBaseProc
{
  public:
    enum DATE_SEPPOINT
    {
      DATETIME_0 = 0,
      DATETIME_1 = 70000,
      DATETIME_2 = 230000,
      DATETIME_3 = 240000
    };
  protected:
    virtual void setOrgFeeInfo(const int &r_feeField);//设置原始费用信息
    virtual bool getCdrSegments();//按天分割话单，形成话单天分割段列表
  private:    
};
#endif //_GPRSPRICEPROC_H_INCLUDE_20081121_


