#ifndef _SUMBILLMGR_H_INCLUDE_20081103_
#define _SUMBILLMGR_H_INCLUDE_20081103_

#include "FeeInfo.h"


const int  SUMRULEMATCH_LESS  = -1;
const int  SUMRULEMATCH_OK    = 0 ;
const int  SUMRULEMATCH_MORE  = 1 ;
const int  SUMRULEMATCH_NOT   = 2 ;

class RateGlobalInfo;
class CompositItemMgr;

class SumBillMgr
{
  public:
  	SumBillMgr();
  	virtual ~SumBillMgr();
  public:
    //初始化，设置总量条件容器指针
    void initialize(      RateGlobalInfo    * r_globalInfo,
                          CompositItemMgr   * r_compIterMgr,
                    const vector<SumTpRule> * r_sumRuleList);
    
    //根据当前批价信息和费用域，取该费用域之前的累积量(批价政策级初始化)
    void getCurBillInfo(const CdrFeeInfo *r_cdrFeeInfo,const int &r_feeField);
	  //取当前批价政策中间累计量
	  void getCurTpBillInfo(const FeeInfo  &r_feeInfo,
	                        const DateTime &r_dateTime,
	                        const long     &r_data,
	                          RateBillInfo &r_feeBillInfo,
	                          RateBillInfo &r_sumBillInfo) const;                          
	  //判断是否满足总量条件
	  //r_feeBillInfo 为该计算中间的费用信息（增量）
	  //r_sumBillInfo 为该计算中间的累计量信息(增量)
	  //返回值: -1  Less,0 OK,1 MORE ,2 不满足
	  //Modified By Sunam 2009/8/6 17:18:18
	  //int  ifMatchRuleList(const RateBillInfo &r_feeBillInfo,
	  //                     const RateBillInfo &r_sumBillInfo);
	  int  ifMatchRuleList(RateBillInfo &r_feeBillInfo, RateBillInfo &r_sumBillInfo);
  public: //以下为封顶方式而加
    //取该话单作用前的累计量
    long getSumValueOld(const RateBillId   &r_billId) ;
    //取该累计量的中间信息
    long getSumMidValue(const RateBillId   &r_billId) ;
  public:
    void dumpMidBillInfo() const;
  protected:
  
    //将总量压入m_midBillInfo
    void addRateBillInfo(const RateBillInfo &r_billInfo);
    //取中间量
    void getMidValue(const RateBillId   &r_billId,
                     const RateBillInfo &r_feeBillInfo,
                     const RateBillInfo &r_sumBillInfo, 
                           long         &r_value) ;
  private:
          RateGlobalInfo      *m_globalInfo;   //全局参数信息
    const RateInfo            *m_pRateInfo;    //批价信息
	  const vector<SumTpRule>   *m_pSumRuleList; //总量条件
	  vector<RateBillInfo>       m_midBillInfo;  //该费用域之前的累积量
	        CompositItemMgr     *m_compItemMgr;  //组合账单参数管理器
  
};

#endif //_SUMBILLMGR_H_INCLUDE_20081103_
