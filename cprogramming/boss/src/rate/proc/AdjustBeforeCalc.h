#ifndef ADJUSTBEFORECALC_H_HEADER_INCLUDED_B6F82ADA
#define ADJUSTBEFORECALC_H_HEADER_INCLUDED_B6F82ADA
#include "AccountCommon.h"
#include "RateGlobalInfo.h"
#include "ObjectBill.h"
#include "Mdb_AdjustBefore.h"
// 计算（用户，用户群（虚拟用户：如客户，集团等），账户）帐前调帐
//##ModelId=48F94C3F01CE
class AdjustBeforeCalc
{
  public:
    //##ModelId=48F94C3F0282
    ~AdjustBeforeCalc();

    //##ModelId=48F94C3F028B
    AdjustBeforeCalc();

    //##ModelId=48F94C3F03B0
    bool dealAdjust(const Discount &r_discount);

    // 对象初始化
    //##ModelId=48FD532A0189
    void initial(const T_SERVER_ID &r_id,
                 ID_TYPE r_idType,
                 PM_Account *r_pAccountParam, 
                 const CycParam   &r_cycParam, 
                 RateGlobalInfo *r_pRateInfo,
                 vector<AdjustBefore> *r_adjustBeforeVector
                 );

  private:
    // 
    //    * getAddupInfo:获取用户上个帐期的累计信息
    //    * @param  r_adjustInsId          调帐实例标识.
    //    * @param  r_beginTime            累计起始时间
    //    * @param  r_addupItem            累计帐目.
    //    * @param  r_addupValue           累计值.
    //    * @return true:存在 false:不存在
    //   
    //##ModelId=48F94C3F02D1
    bool getAddupInfo(const long &r_adjustInsId, const long &r_beginTime, const int &r_addupItem, long &r_addupValue);

  
    // 
    //    * getAdjustBeforeById:根据调账编码获取调账信息
    //    * @param  adjustInsId     调账实例标识.
    //    * @param  adjustBefore    帐前调整信息
    //    * @return true:存在 false:不存在
    //   
    //##ModelId=48F94C3F0326
    bool getAdjustBeforeById(const long &r_adjustInsId, AdjustBefore &r_adjustBefore);

    // 
    //    * getAdjustBeforeFee:根据调账信息获取调账应优惠金额
    //    * @param  r_adjustBefore      帐前调帐信息.
    //    * @param  r_discount          优惠信息
    //    * @param  r_nfee              优惠后费用.
    //    * @param  r_effectFee         应优惠金额  正值:减免 负值:补收
    //    * @param  r_usedfee           使用费用.
    //    * @return true:成功 false:失败
    //   
    //##ModelId=48F94C3F032C
    bool getAdjustBeforeFee(const AdjustBefore &r_adjustBefore,const Discount &r_discount, long &r_nfee, long &r_effectFee,long &r_usedFee);

    /**
     * getReferItemFee:根据参考帐目标识获取明细帐目费用
     * @param  r_referItemCode     参考帐目编码.
     * @param  r_factFee           作用帐目上实际费用.
     * @return 无
     */
    void getReferItemFee(int r_referItemCode,long &r_factFee);
    
    //    * updateAdjustBeforeFee:根据调账更新账单
    //    * @param  r_adjustBefore    调账信息.
    //    * @param  nfee              优惠后费用.
    //    * @param  effectFee         调账金额.     
    //    * @param  discount          优惠信息
    //    * @return 无
    //   
    //##ModelId=48F94C3F036D
    void updateAdjustBeforeFee(AdjustBefore &r_adjustBefore,  const long &r_nfee, long &r_effectFee, const Discount &r_discount);

    // 
    //    * updateAddupInfo:更新普通累计信息
    //    * @param  r_adjustInsId        调帐实例ID.
    //    * @param  r_addupItem          累计帐目标识.
    //    * @param  r_begintTime         累计起始时间.
    //    * @param  r_endTime            累计终止时间.   
    //    * @param  r_effectFee          金额.
    //    * @return 无
    //   
    //##ModelId=48F94C3F0377
    void updateAddupInfo(const long &r_adjustInsId, const int &r_addupItem,const long &r_beginTime,const long &r_endTime, const long &r_effectFee);
    
/**
 * updateEffectItemFee:   根据作用帐目更新账单
 * @param  r_effectItemCode    作用帐目编码.
 * @param  r_nsumfee           作用账目实际费用.
 * @param  r_effectFee         帐前调整费用.
 * @param  r_discountMode      帐前调整方式.
 * @param  r_discountType      帐前调整类型.  
   * @param  r_discount          帐前调整信息
 * @param  r_id                    三户标记
  * @param  r_idType          三户类型
  * @param  r_adjustInsId          调账流水
 * @return 无
*/ 
void updateEffectItemFee(const int &r_effectItemCode,
    	                         const long &r_nsumfee, 
    	                         long &r_effectFee,
    	                         const char &r_discountMode,
    	                         const char &r_discountType, 
    	                         const Discount &r_discount,
    	                         const T_SERVER_ID &r_id,
    	                         const char  r_idType,
    	                         const long      r_adjustInsId );
 private:   	                         

     //当前处理的三户标识
    T_SERVER_ID m_id;
    
    ID_TYPE m_idTypeCode;
    //##ModelId=48F94C3F0246
    //帐前调帐信息
    vector<AdjustBefore> *m_adjustBeforeVector;

    //帐务资料管理对象指针
    //##ModelId=48F94C3F026D
    PM_Account *m_pAccountParam;
    //帐期信息对象
    //##ModelId=48F94C3F026E
    CycParam    m_cycParam;
  
    //帐务处理子过程共享对象
    ComBill     *m_pComBill;
    
    //批价全局信息
    RateGlobalInfo *m_pRateInfo;
    
};



#endif /* ADJUSTBEFORECALC_H_HEADER_INCLUDED_B6F82ADA */
