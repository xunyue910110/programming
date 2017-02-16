#ifndef _EFFECTFEECOUNT_H_
#define _EFFECTFEECOUNT_H_

#include "EffectBase.h"
#include "AddupCycleRule.h"

class ObjectBase;
class EffectAddup;

const int RATE_OBJECT_TIME = 0;
const int RATE_OBJECT_DATA = 1;
class EffectFeeCount :public EffectBase
{
  public:
  	EffectFeeCount();
  	virtual ~EffectFeeCount();
  	
  	bool operator < (const EffectFeeCount &right) const
  	{
  		return (m_id < right.m_id);
  	}
  
  	bool operator == (const EffectFeeCount &right) const
  	{
  		return (m_id==right.m_id);
  	}
  public:	
  	virtual bool execute() ;
  	virtual void dump(char *);
  public:
    /* 清单式作用，对话单进行批价，并将结果存在
     * RateInfo.m_curFeeInfoList[m_curBranchIdx]中
     *@param r_pCdrSegList 要计算话单段
     *@return true 成功 false 失败
     */
    virtual bool rateExecute(const vector<CdrSeg> &r_pCdrSegList);
     
    bool computeFee(const RateBillId &r_rateBillId,
                    const vector<CdrSeg> &r_pCdrSegList,
                    FeeInfo &r_feeInfo) const;  
    //总量政策方式处理
    bool sumTpProcess(FeeInfo      &r_feeInfo,
                      RateBillInfo &r_sendBillInfo,
                      FeeInfo      &r_desFeeInfo);
                      
    //add for new version begin                  
    //从td_b_feecount表unit_ratio字段获取明细金额
    bool getRentUnitRatio(long &r_feeRatio) const;
    
    //从td_b_feecount表base_unit字段，作为规整费用金额	
    bool getSumToIntFee(long &r_sumToIntFee) const;
    
    //获取归整标志
    char getSumToIntTag();
    
    //获取display调整标志
    char getDisplayMode();
    	
    //add for new version end
  protected:
    //判断各个子帐期费用是否相等(有可能是第几个子帐期到第几个子帐期费用相等与否)
    bool isSubFeeEqual(int startSubId, int endSubId) const;
    //简单式费用计算,即x元/分钟方式或固定费用
    bool computeFee_simp(const vector<CdrSeg> &r_pCdrSegList,FeeInfo &r_feeInfo) const; 
    //复杂方式费用计算，必须有基准费率
    bool computeFee_comp(const RateBillId &r_rateBillId,
                         const vector<CdrSeg> &r_pCdrSegList,
                               FeeInfo &r_feeInfo) const;  
    //从基准资费中获取对应段的资费信息
    //和r_pCdrSegList相关可能需要重批的,段类型置为TMPSEG
    bool getStdFeeInfo(const vector<CdrSeg> &r_pCdrSegList,
                             FeeInfo &r_feeInfo) const;
    //取费率信息
    bool getFeeRatio(long &r_feeRatio) const;

    //根据资费信息和费用作用的累计信息取对应的累计ID信息
    //bool getRateBillIdInfo(const RateTpInfo &r_rateTpInfo,RateBillId &r_rateBillId) const;
    bool getRateBillIdInfo(const RateTpInfo &r_rateTpInfo,RateBillId &r_rateBillId,const T_DATE_TIME &r_cdrStartTime) const;
    
    //取当前段前r_duration（时长）的费用和赠送量信息
    void getAddBillInfo(const SegFeeInfo &r_segFeeInfo,
                        const long       &r_unitCount,
                              long       &r_addFeeValue,
                              long       &r_addSendValue) const;
    //费用封顶处理
    void computeFee_Limit(const RateBillId &r_rateBillId,
                          const vector<CdrSeg> &r_pCdrSegList,
                                FeeInfo &r_feeInfo) const;  
    //按当月总流量进行计费
    void computeFee_totalData(const RateBillId &r_rateBillId,
                              const vector<CdrSeg> &r_pCdrSegList,
                                    FeeInfo &r_feeInfo) const;
    //根据资费执行方式,获取中间执行方式
    int  getExeFlag(const RateTpInfo * r_rateTpInfo) const;
  public:	
  	int             m_computeMethod;               //计算方式
  	long            m_baseUnit;                    //基准单元
  	//char            m_unitRatio[STR_LENGTH+1];   //单元费率 支持表达式
  	char            m_unitRatio[100+1];   //单元费率
  	char            m_timesIntTag;                 //跳次规整标记
  	int             m_object;                      //计算引用对象指针
  	EffectAddup    *m_addupRule;                   //累计处理规则
  	//add for GPRS 0.X厘费率 2009,0904 费用精度
    int m_pricision ;
};

#endif
