/**
*   套餐月租实扣引擎.
*   提供套餐实扣管理
*   @author 高将飞 <gaojf@lianchuang.com>
*   @version 1.0
*   @date 2008-11-14
*/
#ifndef _TpRealRentMgr_H_INCLUDE_20081114_
#define _TpRealRentMgr_H_INCLUDE_20081114_
#include "RateConstDef.h"
#include "FeeInfo.h"
#include "RateCdr.h"

class RateGlobalInfo;
class UserSumBill1;
class UserBill;
class TpFreeValue
{
  public:
    long        m_tpObjId;  //资费实例
    long        m_value;    //优惠的费用
    const RateTpInfo *m_pTpInfo;  //资费信息
  public:
    TpFreeValue(){m_value=0;}
    friend bool operator<(const TpFreeValue &r_left,const TpFreeValue &r_right)
    {
      return (r_left.m_tpObjId<r_right.m_tpObjId) ;
    }
    friend bool operator==(const TpFreeValue &r_left,const TpFreeValue &r_right)
    {
      return (r_left.m_tpObjId==r_right.m_tpObjId) ;
    }
};

class TpRealRentMgr
{
  public:
    void initialize(RateGlobalInfo *r_pRateGInfo);
    void cdr_int();
    const char * getRateTrackInfo();
    void getTpRentInfos(vector<UserSumBill1> *r_tpRentInfos,
                        vector<UserBill>     *r_tpRentBills);
    void setRateTrack(const int &r_feeField,const FeeInfo &r_feeInfo);
  protected:
    void push_back(const int &r_feeField,const FeeInfo &r_feeInfo);
    void computeTpRent(const TpFreeValue &r_rentInfo,vector<UserSumBill1> *r_tpRentInfos,
                       vector<UserBill> *r_tpRentBills);
    void getFreeValue(const int &r_feeField,const int &r_lastIdx,
                      const ACT_METHOD &r_actMethod,
                      const SegFeeInfo &r_segFeeInfo);
    bool isSameTpInfo(const SegFeeInfo &r_segFeeInfo,const FeeInfo &r_feeInfo);
    long getSegFreeValue(const SegFeeInfo &r_segFeeInfo,
                         const FeeInfo    &r_feeInfo,
                       vector<SegFeeInfo> &r_segFeeInfoList);
  protected:
    RateGlobalInfo  *m_pRateGInfo;
    PCDR             m_pCdr;
    vector<FeeInfo>  m_rateTrackList[FEEFIELDS_MAX]; //套餐作用轨迹信息
    int              m_nodeNum[FEEFIELDS_MAX];
    int              m_capacity[FEEFIELDS_MAX];
    //优惠轨迹信息为 {费用域|资费实例|资费标识|费用}
    // 多个信息间 以 分号 分割
    char             m_freeTrack[TRACK_INFO_MAXLEN+1]; //优惠轨迹信息
    int              m_trackLen;            //优惠轨迹长度
  protected:
    vector<TpFreeValue> m_freeValues;
};
#endif //_TpRealRentMgr_H_INCLUDE_20081114_

/*****************************************************
 *        套餐月租实扣规则说明
 *1. 该月以前的话单,不扣除套餐月租费
 *2. 作为一次批价政策的套餐，不扣去套餐月租费
 *3. 无套餐实扣参数的套餐，不扣去套餐月租费
 *4. 套餐月租的实扣，以该话单最后的政策为准,最后参加作用的政策才扣取
 *5. 套餐扣取的额度，以该套餐作用时所产生的优惠额度为准
 *6. 如果该套餐总优惠额度<0，则该套餐不实扣
*****************************************************/
