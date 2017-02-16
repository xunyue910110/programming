#include "SumBillMgr.h"
#include "CompositItemMgr.h"
#include "RateGlobalInfo.h"

SumBillMgr::SumBillMgr()
{
}  
SumBillMgr::~SumBillMgr()
{
  m_midBillInfo.clear();
}

//初始化，设置总量条件容器指针
void SumBillMgr::initialize(      RateGlobalInfo    * r_globalInfo,
                                  CompositItemMgr   * r_compIterMgr,
                            const vector<SumTpRule> * r_sumRuleList)
{
  m_globalInfo   = r_globalInfo;
  m_pRateInfo    =&(r_globalInfo->m_rateFeeInfo);
  m_pSumRuleList = r_sumRuleList;
  m_compItemMgr  = r_compIterMgr;
}

//根据当前批价信息和费用域，取该费用域之前的累积量(批价政策级初始化)
void SumBillMgr::getCurBillInfo(const CdrFeeInfo *r_cdrFeeInfo,const int &r_feeField)
{
  RateBillInfo  t_billInfo;
  m_midBillInfo.clear();

  t_billInfo.clear();
  t_billInfo.m_billId.m_billType      = BILL_TYPE_USER;
  t_billInfo.m_billId.m_userId        = m_globalInfo->m_billUserId;
  t_billInfo.m_billId.m_sumBeginTime  = m_globalInfo->m_beginDateTime.value();
  for(int i=1;i<r_feeField;++i)
  {
    const FeeInfo &t_feeInfo = r_cdrFeeInfo->m_feeInfo[i];
    for(int j=0;j<t_feeInfo.m_seg;++j)
    {
      const SegFeeInfo &t_segFeeInfo = t_feeInfo.m_segInfoList[j];
      for(vector<RateBillInfo>::const_iterator t_itr=t_segFeeInfo.m_billInfoList.begin();
          t_itr!=t_segFeeInfo.m_billInfoList.end();++t_itr)
      {
        //该处只取赠送费用和流量或费用的累计量，赠送时长的累计量不取。
        //便于支持赠送基本费和长途费100分钟的情况
        if( t_itr->m_objType == BILLOBJ_TIMEFEE ||
            t_itr->m_objType == BILLOBJ_DATAFEE ||
            t_itr->m_objType == BILLOBJ_DATA    ||
            t_itr->m_objType == BILLOBJ_FEELIMIT_T||
            t_itr->m_objType == BILLOBJ_FEELIMIT_D)
        {
          addRateBillInfo(*t_itr);
        }
      }
    }
    if(t_feeInfo.m_feeSum>0)
    {
      t_billInfo.m_billId.m_billId = m_pRateInfo->m_billIdList[i];
      t_billInfo.m_objType   = BILLOBJ_FEE;
      t_billInfo.m_unitValue = 1;
      t_billInfo.m_unitFee   = t_feeInfo.m_feeSum;
      t_billInfo.m_billValue = t_feeInfo.m_feeSum;
      addRateBillInfo(t_billInfo);
    }
  }
}

//判断是否满足总量条件
//r_feeBillInfo 为该计算中间的费用信息（增量）
//r_sumBillInfo 为该计算中间的累计量信息(增量)
//返回值: -1  Less,0 OK,1 MORE 
//Modified By Sunam 2009/8/6 17:18:18
//int  SumBillMgr::ifMatchRuleList(const RateBillInfo &r_feeBillInfo,
//                                 const RateBillInfo &r_sumBillInfo)
int  SumBillMgr::ifMatchRuleList(RateBillInfo &r_feeBillInfo, RateBillInfo &r_sumBillInfo)
{
  long  t_addValue,t_newValue;
  int   t_retValue = SUMRULEMATCH_OK;
  for(vector<SumTpRule>::const_iterator t_itr=m_pSumRuleList->begin();
      t_itr != m_pSumRuleList->end();++t_itr)
  {
    for(vector<SumRule>::const_iterator t_ruleItr = t_itr->m_ruleList.begin();
         t_ruleItr != t_itr->m_ruleList.end();++t_ruleItr)
    {
      //Added By Sunam 2009/12/29 10:33:38
      /*
         如果m_pSumRuleList->m_ruleList类似
         20091226000000 20091227000000 0 44005 3600
         20091227000000 20091228000000 0 44005 3000
         r_sumBillInfo.m_billId 为
         20091227000000 20091228000000 0 44005 0 应该只比较m_billId相同的
      */
       if( t_ruleItr->m_billId.m_billType == BILL_TYPE_SUM2  &&
          //下面这个判断可考虑封装在FeeInfo.h RateBillId类中
          t_ruleItr->m_billId.m_userId  == r_sumBillInfo.m_billId.m_userId  &&
          t_ruleItr->m_billId.m_tpObjId == r_sumBillInfo.m_billId.m_tpObjId &&
          t_ruleItr->m_billId.m_billId  == r_sumBillInfo.m_billId.m_billId  &&
          t_ruleItr->m_billId.m_sumBeginTime != r_sumBillInfo.m_billId.m_sumBeginTime)
	  {
	  	  continue;
	  }
      getMidValue(t_ruleItr->m_billId,r_feeBillInfo,r_sumBillInfo,t_addValue);
      t_newValue = t_ruleItr->m_curValue+t_addValue;
      //Added By Sunam 2009/8/6 16:32:25
      r_feeBillInfo.m_maxValue = t_ruleItr->m_maxValue;
      r_sumBillInfo.m_maxValue = t_ruleItr->m_maxValue;
      if(t_newValue >= t_ruleItr->m_maxValue)
      {
        t_retValue = SUMRULEMATCH_MORE;
        return t_retValue;
      }else if(t_newValue < t_ruleItr->m_minValue)
      {
        t_retValue = SUMRULEMATCH_LESS;
      }else continue;
    }
  }
  return t_retValue;
}

//将总量压入m_midBillInfo
void SumBillMgr::addRateBillInfo(const RateBillInfo &r_billInfo)
{
  for(vector<RateBillInfo>::iterator t_itr=m_midBillInfo.begin();
      t_itr != m_midBillInfo.end(); ++t_itr)
  {
    if(r_billInfo == *t_itr)
    {
      t_itr->m_billValue += r_billInfo.m_billValue;
      return;
    }
  }
  m_midBillInfo.push_back(r_billInfo);
}

//取中间量
void SumBillMgr::getMidValue(const RateBillId   &r_billId,
                             const RateBillInfo &r_feeBillInfo,
                             const RateBillInfo &r_sumBillInfo,
                                   long         &r_value) 
{
  r_value = 0;
  vector<RateBillInfo>::const_iterator t_itr;
  if(r_billId.m_billType == BILL_TYPE_USER )
  { //普通账单
    if(r_billId.m_type == 0) //组合帐目
    {
      IntMap & t_dtlItemMap = m_compItemMgr->find(r_billId.m_billId);
      for(map<int,int>::const_iterator t_mapItr = t_dtlItemMap.begin();
          t_mapItr!=t_dtlItemMap.end();++t_mapItr)
      {
        if(r_feeBillInfo.m_billId.m_billId == t_mapItr->first)
        {
          r_value += r_feeBillInfo.m_billValue;
        }
        for(t_itr=m_midBillInfo.begin();t_itr!=m_midBillInfo.end();++t_itr)
        {
          if(t_itr->m_billId.m_billId == t_mapItr->first)
          {
            r_value += t_itr->m_billValue;
          }
        }
      }
      return ;
    }else //明细帐目
    {
      if(r_billId == r_feeBillInfo.m_billId)
      {
        r_value += r_feeBillInfo.m_billValue;
      }
      for(t_itr=m_midBillInfo.begin();t_itr!=m_midBillInfo.end();++t_itr)
      {
        if(t_itr->m_billId == r_billId)
        {
          r_value += t_itr->m_billValue;
          return;
        }
      }
    }
  }else 
  { //累计量
    if(r_billId == r_sumBillInfo.m_billId)
    {
      r_value += r_sumBillInfo.m_billValue;
    }
    for(t_itr=m_midBillInfo.begin();t_itr!=m_midBillInfo.end();++t_itr)
    {
      if(t_itr->m_billId == r_billId)
      {
        r_value += t_itr->m_billValue;
        return;
      }
    }
  } 
}
//取当前批价政策中间累计量
void SumBillMgr::getCurTpBillInfo(const FeeInfo  &r_feeInfo,
                                  const DateTime &r_dateTime,
                                  const long     &r_data,
                                    RateBillInfo &r_feeBillInfo,
                                    RateBillInfo &r_sumBillInfo) const
{
  long t_duration,t_data,t_unitCount;
  vector<RateBillInfo>::const_iterator t_itr;
  r_feeBillInfo.m_billValue = 0;
  r_sumBillInfo.m_billValue = 0;
  for(int i=0;i<r_feeInfo.m_seg;++i)
  {
    const SegFeeInfo &t_segFeeInfo = r_feeInfo.m_segInfoList[i];
    if(t_segFeeInfo.m_startDateTime > r_dateTime ||
       t_segFeeInfo.m_startDateTime == r_dateTime &&
       t_segFeeInfo.m_startData     >= r_data)
    {
      break;
    }else if(t_segFeeInfo.m_endDateTime < r_dateTime ||
             t_segFeeInfo.m_endDateTime == r_dateTime &&
             t_segFeeInfo.m_endData     <= r_data)
    {
      for(t_itr  = t_segFeeInfo.m_billInfoList.begin();
          t_itr != t_segFeeInfo.m_billInfoList.end();++t_itr)
      {
        if(r_sumBillInfo == *t_itr)
        {
          r_sumBillInfo.m_billValue += t_itr->m_billValue;
          break;
        }
      }
      r_feeBillInfo.m_billValue += t_segFeeInfo.m_value;
    }else 
    //if(t_segFeeInfo.m_startDateTime <  r_dateTime ||
    //    t_segFeeInfo.m_startDateTime == r_dateTime &&
    //    t_segFeeInfo.m_startData     < r_data) &&
    //   (t_segFeeInfo.m_endDateTime >  r_dateTime ||
    //    t_segFeeInfo.m_endDateTime == r_dateTime &&
    //    t_segFeeInfo.m_endData   > r_data) 
    {
      t_duration = r_dateTime-t_segFeeInfo.m_startDateTime;
      t_data     = r_data - t_segFeeInfo.m_startData;
      
      if(t_segFeeInfo.m_rateMethod == RATE_METHOD_DATA)
      {
        t_unitCount= (t_data+t_segFeeInfo.m_unitData-1)
                      /t_segFeeInfo.m_unitData;
      }else
      {
        t_unitCount= (t_duration+t_segFeeInfo.m_unitTime-1)
                      /t_segFeeInfo.m_unitTime;
      }
      if(t_unitCount > t_segFeeInfo.m_unitCount) 
         t_unitCount=t_segFeeInfo.m_unitCount;
         
      for(t_itr  = t_segFeeInfo.m_billInfoList.begin();
          t_itr != t_segFeeInfo.m_billInfoList.end();++t_itr)
      { //累计量
        if(r_sumBillInfo == *t_itr)
        {
          if(t_itr->m_objType == BILLOBJ_DURATION)
          {
            r_sumBillInfo.m_billValue += t_segFeeInfo.m_unitTime*t_unitCount;
          }else if(t_itr->m_objType == BILLOBJ_TIMEFEE && t_itr->m_unitValue > 0)
          {
            
            r_sumBillInfo.m_billValue += t_itr->m_unitFee*t_unitCount*
                                         t_segFeeInfo.m_unitTime/t_itr->m_unitValue;
          }else if(t_itr->m_objType == BILLOBJ_DATAFEE && t_itr->m_unitValue > 0)
          {
            r_sumBillInfo.m_billValue += t_itr->m_unitFee*t_unitCount*
                                         t_segFeeInfo.m_unitData/t_itr->m_unitValue;
          }else if(t_itr->m_objType == BILLOBJ_DATA)
          { //coding here later 流量
            r_sumBillInfo.m_billValue += t_segFeeInfo.m_unitData*t_unitCount;
          }
        }
      }
      r_feeBillInfo.m_billValue += t_segFeeInfo.m_unitFee*t_unitCount;
    }
  }
}
void SumBillMgr::dumpMidBillInfo() const
{
  for(vector<RateBillInfo>::const_iterator t_itr=m_midBillInfo.begin();
      t_itr!=m_midBillInfo.end();++t_itr)
  {
    cout<<*t_itr<<endl;
  }
}

//取该话单作用前的累计量
long SumBillMgr::getSumValueOld(const RateBillId &r_billId)
{
  long t_value = 0;
  if(r_billId.m_billType == BILL_TYPE_SUM1)
  {
    UserSumBill1 t_billItem1;
    const UserSumBill1 *t_pBill1=NULL;
    t_billItem1.m_userId  = r_billId.m_userId;
    t_billItem1.m_tpObjId = r_billId.m_tpObjId;
    t_billItem1.m_billId  = r_billId.m_billId;
    t_billItem1.m_value   = 0;
    if(m_globalInfo->getBillItem<UserSumBill1>(m_globalInfo->m_userSumBill1s_new,
              t_billItem1,t_pBill1)==true)
    {
      t_value = t_pBill1->m_value;
    }
  }else if(r_billId.m_billType == BILL_TYPE_SUM2)
  {
    UserSumBill2 t_billItem2;
    const UserSumBill2 *t_pBill2=NULL;
    t_billItem2.m_userId   = r_billId.m_userId;
    t_billItem2.m_tpObjId  = r_billId.m_tpObjId;
    t_billItem2.m_beginTime= r_billId.m_sumBeginTime;
    t_billItem2.m_endTime  = r_billId.m_sumEndTime;
    t_billItem2.m_cycleId  = 0;
    t_billItem2.m_billId   = r_billId.m_billId;
    t_billItem2.m_value    = 0;
    if(m_globalInfo->getBillItem<UserSumBill2>(m_globalInfo->m_userSumBill2s_new,
              t_billItem2,t_pBill2)==true)
    {
      t_value = t_pBill2->m_value;
    }
  }
  return t_value;
}
//取该累计量的中间信息
long SumBillMgr::getSumMidValue(const RateBillId   &r_billId)
{
  long t_value = 0;
  for(vector<RateBillInfo>::const_iterator t_itr=m_midBillInfo.begin();
      t_itr!=m_midBillInfo.end();++t_itr)
  {
    if(t_itr->m_billId == r_billId)
    {
      t_value += t_itr->m_billValue;
      break;
    }
  }
  return t_value;
}
