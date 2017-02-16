#ifndef RENTPROC_H_INCLUDE_20090619_
#define RENTPROC_H_INCLUDE_20090619_ 

#include "Procedure.h"
#include "FeePolicyComp.h"
#include "EventFeePolicy.h"
#include "Price.h"
#include "CondBase.h"
#include "AccountCommon.h"
#include "RateGlobalInfo.h"
#include "Outputer.h"

class RentProc : public Procedure 
{
  public:
    virtual bool initialize();
    virtual int execute();  
    
    bool calcTotalCycRent();
    
    //是否需要计算固定费
    bool isNeedCalcRent();
    
  private:
    
    //获取计算的开始结束子帐期
    bool getCalcDateInfo(T_DATE_DAY& r_startDay, T_DATE_DAY& r_endDay,
                     map<T_SUBCYC_TYPE, int>& r_startSubCycIdMap,
                     map<T_SUBCYC_TYPE, int>& r_endSubCycIdMap);
    
    //计算子帐期费用                 
    bool calcSubFee(const RentEvent& rentEvent, 
                    const int subCycId,  //子帐期序列号
                    const int cycId,     //帐期
                    UserRentPriceLog& r_rentPricelog,
                    long& subfee, int& errCode);
                       
    //获取某个时间段内固定费资费         
    RateTpInfo* getUserSubCycRentTp(const T_BIZ_TYPE& r_evenTypeCode, 
                                        const T_DATE_TIME& r_startDateTime,
                                        const T_DATE_TIME& r_endDateTime);
    
    //用户是否有某个服务
   bool hasUserServ(const T_DATE_TIME& r_startDateTime,
                           const T_DATE_TIME& r_endDateTime, 
                           const T_SERVICE_ID r_iServ,
                           UserServ& userServ);
    
    //在某段时间内是否是有效用户
    bool isEffectiveUser(const T_DATE_TIME& startDate,
                         const T_DATE_TIME& endDate);
                         
    //用于将事件汇总成用户明细帐单
    bool mergeToUserBill();
    
    //获取用户服务状态
    //Added by Malone 2009-9-23 14:33 start
    /*
    bool getUserServState(const T_DATE_TIME& r_startDateTime,
                                const T_DATE_TIME& r_endDateTime,  
                                const T_SERVICE_ID r_iServ, 
                                UserServState& r_servState);
    */
    bool getUserServState(const T_DATE_TIME& r_startDateTime,
                                const T_DATE_TIME& r_endDateTime,  
                                const T_SERVICE_ID r_iServ, 
                                UserServState& r_servState,
                                T_DATE_TIME& r_servStateMinStartDateTime,
                                T_DATE_TIME& r_servStateMaxEndDateTime);
    //Added by Malone 2009-9-23 14:33 end
    
    /**
    * getSubCycIdFromDay:从某个日期、帐目，获取该帐目项子帐期
    * @param  r_day                 日期YYYYMMDD
    * @param  r_subCycTypeCode      子账期类型
    * @return 子账期标识, 0 表示 没获取
    */
    int getSubCycIdFromDay(const T_DATE_DAY& r_day ,const T_SUBCYC_TYPE& r_subCycTypeCode);


    //获取这个用户有可能发生的事件序列号（对应PM_Rate中m_vecRentEvent的下标）
    int getUserRentEvent(vector<int>& userRentEventIdxVec);
    
    void registerErrorUser(const T_USER_ID &r_id, 
                         const int r_errCode,
                         const T_FLAG &r_operTypeTag);
    
    vector<int>  m_userRentEventIdxVec;   //用户有可能触发的事件
    
    /*removed for rentfeeItem 2011-3-23
    vector<UserBill>   m_userRentBills;   //以事件编码作为billId的临时性用户帐单（用于转变成明细账单前的格式）
    */
    vector<RentOutBillCdr> m_rentBillInfoVec;//包含固定费用户账单（含事件）和固定费话单的容器
    char  m_billTrack[TRACK_INFO_MAXLEN+1]; //计费账单轨迹
    void setBillInfos(PCDR r_pcdr,const RentBillInfo  &r_rentBill);	       
    
    
};





#endif //RENTPROC_H_INCLUDE_20090619_
