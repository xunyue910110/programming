#include "TpRealRentMgr.h"
#include "RateGlobalInfo.h"
#include "EventFeePolicy.h"

void TpRealRentMgr::initialize(RateGlobalInfo *r_pRateGInfo)
{
  m_pRateGInfo = r_pRateGInfo;
  m_pCdr       = r_pRateGInfo->m_pCdr;
  for(int i=0;i<FEEFIELDS_MAX;++i)
  {
    m_nodeNum[i]=0;
    m_capacity[i]=0;
  }
}
void TpRealRentMgr::cdr_int()
{
  m_trackLen=0;m_freeTrack[0]=0; //初始化
  for(int i=0;i<FEEFIELDS_MAX;++i)
  {
    m_nodeNum[i]=0;
  }
  m_freeValues.clear();
}

void TpRealRentMgr::push_back(const int &r_feeField,const FeeInfo &r_feeInfo)
{
  if(m_capacity[r_feeField]>m_nodeNum[r_feeField])
  {
    m_rateTrackList[r_feeField][m_nodeNum[r_feeField]]=r_feeInfo;
  }else
  {
    m_rateTrackList[r_feeField].push_back(r_feeInfo);
    m_capacity[r_feeField] += 1;
  }
  m_nodeNum[r_feeField] += 1;
}
void TpRealRentMgr::setRateTrack(const int &r_feeField,const FeeInfo &r_feeInfo)
{
  if(r_feeInfo.m_pRateTpInfo == NULL || r_feeInfo.m_seg==0) return;
  push_back(r_feeField,r_feeInfo);
  for(int i=0;i<m_nodeNum[r_feeField]-1;++i)
  {
    RateTpInfo * t_pTpInfo = m_rateTrackList[r_feeField][i].m_pRateTpInfo;
    if((t_pTpInfo->m_pUserTp->m_tpObjId ==
        r_feeInfo.m_pRateTpInfo->m_pUserTp->m_tpObjId)&&
       (t_pTpInfo->m_rateTpId ==
        r_feeInfo.m_pRateTpInfo->m_rateTpId)&&
       (t_pTpInfo->m_pEventTp->m_id ==
        r_feeInfo.m_pRateTpInfo->m_pEventTp->m_id))
    {
      return; //该轨迹已重复
    }
  }
  if(m_trackLen>0) {sprintf(m_freeTrack+m_trackLen,";");++m_trackLen;}
  sprintf(m_freeTrack+m_trackLen,"%d|%ld|%ld|%ld|%ld",
          r_feeField,
          (long)r_feeInfo.m_pRateTpInfo->m_pUserTp->m_tpObjId,
          (long)r_feeInfo.m_pRateTpInfo->m_rateTpId,
          (long)r_feeInfo.m_pRateTpInfo->m_pEventTp->m_id,
          r_feeInfo.m_feeSum);
  m_trackLen = strlen(m_freeTrack);
}

const char * TpRealRentMgr::getRateTrackInfo()
{
  return m_freeTrack;
}
void TpRealRentMgr::getTpRentInfos(vector<UserSumBill1> *r_tpRentInfos,
                                   vector<UserBill>     *r_tpRentBills)
{
  r_tpRentInfos->clear();r_tpRentBills->clear();
  
  //1. 如果话单是上月的,则不扣取月租费
  if(m_pCdr[ATTRI_CDR_CYCLE_ORG] < m_pCdr[ATTRI_CYCLE_DATE])
  {
    return;
  }
  int  t_lastIdx = 0;
  for(int i=1;i<FEEFIELDS_MAX;++i)
  {
    t_lastIdx=m_nodeNum[i]-1;
    if(t_lastIdx <= 0) continue;
    FeeInfo &t_result = m_rateTrackList[i][t_lastIdx];
    if(t_result.m_rateTime == RATE_TIMES_1) continue;
    for(int j=0;j<t_result.m_seg;++j)
    {
      SegFeeInfo &t_segFeeInfo = t_result.m_segInfoList[j];
      //通过和前面政策比较,求取该段的优惠值
      //并将优惠信息存放在m_freeValues中
      getFreeValue(i,t_lastIdx,t_segFeeInfo.m_actMethod,t_segFeeInfo);
    }
  }
  //2.将所有的优惠信息排序
  ::sort(m_freeValues.begin(),m_freeValues.end());
  //3. 结合累计的情况
  TpFreeValue  t_tpFreeValue;
  for(vector<TpFreeValue>::const_iterator t_itr=m_freeValues.begin();
      t_itr!=m_freeValues.end();++t_itr)
  {
    if(t_itr==m_freeValues.begin())
    {
      t_tpFreeValue = *t_itr;
    }else if(t_tpFreeValue == *t_itr)
    {
      t_tpFreeValue.m_value += t_itr->m_value;
    }else
    { //处理该优惠信息t_tpFreeValue
      computeTpRent(t_tpFreeValue,r_tpRentInfos,r_tpRentBills);
      t_tpFreeValue =  *t_itr;
    }
  }
  computeTpRent(t_tpFreeValue,r_tpRentInfos,r_tpRentBills);
}

void TpRealRentMgr::computeTpRent(const TpFreeValue &r_rentInfo,
                               vector<UserSumBill1> *r_tpRentInfos,
                               vector<UserBill>     *r_tpRentBills)
{
  if(r_rentInfo.m_value <= 0) return;
  
  //获取对应资费实例的套餐月租信息
  long t_rentValue ;
  if(r_rentInfo.m_pTpInfo->m_pUserTp->m_subTpType == TP_TYPE_VPN&&
     r_rentInfo.m_pTpInfo->m_rateRoleCode == ROLECODE_SELF)
  { //不支持集团总量型套餐费实扣
    return;
  }
  if(r_rentInfo.m_pTpInfo->m_realRentFlag == FLAG_NO) return;  //modified by gaojf 2009-2-18 15:00
  
  //modified by gaojf 2009-2-18 15:00
   /*
  if(r_rentInfo.m_pTpInfo->m_pUserTp->m_halfInNetFlag=='1')
  { //半月
    t_rentValue = r_rentInfo.m_pTpInfo->m_halfRent;
  }else // 全月
  {
    t_rentValue = r_rentInfo.m_pTpInfo->m_monthRent;
  }
  */
  //Modified By Sunam 2009/11/10 11:28:11
  //t_rentValue = r_rentInfo.m_pTpInfo->getRentValue(
  //                  m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
  //                  m_pRateGInfo->m_firstCallTime,
  //                  m_pRateGInfo->m_cycDays);
  t_rentValue = r_rentInfo.m_pTpInfo->getRentValue(
                    m_pCdr[ATTRI_CURRENT_OPEN_FLAG][0],
                    m_pRateGInfo->m_firstCallTime,
                    m_pRateGInfo->m_cycDays,
                    m_pCdr[ATTRI_EARLIEST_OPEN_DATE].c_str());
  if(t_rentValue<=0) return; //非实扣套餐
  
  const UserSumBill1 *t_pSumBill1;  
  UserSumBill1  t_userSumBill1;
  UserBill      t_userBill;
  long t_rentValueOld=0;
  t_userSumBill1.m_userId = m_pRateGInfo->m_billUserId;
  t_userSumBill1.m_tpObjId= r_rentInfo.m_tpObjId;
  t_userSumBill1.m_billId = r_rentInfo.m_pTpInfo->m_sumBillId;
  t_userBill.m_userId = m_pRateGInfo->m_billUserId;
  t_userBill.m_billId = r_rentInfo.m_pTpInfo->m_rentBillId;

  if(m_pRateGInfo->getBillItem<UserSumBill1>(
           m_pRateGInfo->m_userSumBill1s_new,
           t_userSumBill1,t_pSumBill1)==true)
  {
    t_rentValueOld = t_pSumBill1->m_value;
  }
  
  if(t_rentValueOld >= t_rentValue)
  {
    return; //套餐费已经扣除完毕
  }else if(r_rentInfo.m_value+t_rentValueOld > t_rentValue)
  {
    t_userSumBill1.m_value = t_rentValue-t_rentValueOld;
  }else
  {
    t_userSumBill1.m_value = r_rentInfo.m_value;  
  }
  t_userBill.m_value       = t_userSumBill1.m_value;
  t_userBill.m_discntValue = t_userSumBill1.m_value;
  r_tpRentInfos->push_back(t_userSumBill1);
  r_tpRentBills->push_back(t_userBill);
}

void TpRealRentMgr::getFreeValue(const int &r_feeField,const int &r_lastIdx,
                                 const ACT_METHOD &r_actMethod,
                                 const SegFeeInfo &r_segFeeInfo)
{
  if(r_lastIdx <= 0) return;
  int t_stdIdx = r_lastIdx-1;
  TpFreeValue t_freeValue;
  vector<SegFeeInfo> t_segFeeInfoList;
  t_freeValue.m_tpObjId = (long)(r_segFeeInfo.m_tpObjId);
  t_freeValue.m_pTpInfo = r_segFeeInfo.m_pRateTpInfo;
  if(r_actMethod == ACT_METHOD_COVER) //覆盖式
  {
    //找到最后一个一次批价结果或第一个资费结果做比较
    for(t_stdIdx = r_lastIdx-1;t_stdIdx>=0;--t_stdIdx)
    {
      if(t_stdIdx == 0) break;
      if(m_rateTrackList[r_feeField][t_stdIdx].m_rateTime == RATE_TIMES_1)
      {
        break;
      }
    }
    //和m_rateTrackList[r_feeField][t_stdIdx]比较计算优惠额度
    t_freeValue.m_value = getSegFreeValue(r_segFeeInfo,
              m_rateTrackList[r_feeField][t_stdIdx],t_segFeeInfoList);
    if(t_freeValue.m_value != 0)
    {
      m_freeValues.push_back(t_freeValue);
    }
    return;
  }else //r_actMethod == ACT_METHOD_MULTI //叠加式
  {
    for(t_stdIdx = r_lastIdx-1;t_stdIdx>=0;--t_stdIdx)
    {
      if(t_stdIdx == 0) break;
      //找到第一个非本身政策的段政策
      if(isSameTpInfo(r_segFeeInfo,m_rateTrackList[r_feeField][t_stdIdx])==true)
      {
        continue;
      }else
      {
        break;
      }
    }
    t_freeValue.m_value = getSegFreeValue(r_segFeeInfo,
              m_rateTrackList[r_feeField][t_stdIdx],t_segFeeInfoList);
    if(t_freeValue.m_value != 0)
    {
      m_freeValues.push_back(t_freeValue);
    }
    //叠加方式，需要再获取前面套餐的优惠值
    for(vector<SegFeeInfo>::const_iterator t_itr = t_segFeeInfoList.begin();    
         t_itr != t_segFeeInfoList.end();++t_itr)
    {
      getFreeValue(r_feeField,t_stdIdx,t_itr->m_actMethod,*t_itr);
    }
    return ;
  }
}
bool TpRealRentMgr::isSameTpInfo(const SegFeeInfo &r_segFeeInfo,
                                 const FeeInfo    &r_feeInfo)
{
  for(int i=0;i<r_feeInfo.m_seg;++i)
  {
    if(r_segFeeInfo.m_startDateTime>=
       r_feeInfo.m_segInfoList[i].m_endDateTime)
    {
      continue;
    }
    return (r_segFeeInfo.m_tpObjId == r_feeInfo.m_segInfoList[i].m_tpObjId);
  }
  return false;
}
  
long TpRealRentMgr::getSegFreeValue(const SegFeeInfo &r_segFeeInfo,
                                    const FeeInfo    &r_feeInfo, 
                                  vector<SegFeeInfo> &r_segFeeInfoList)
{
  long  t_value = 0;
  r_segFeeInfoList.clear();
  SegFeeInfo t_partSegInfo;
  for(int i=0;i<r_feeInfo.m_seg;++i)
  {
    const SegFeeInfo &t_segFeeInfo = r_feeInfo.m_segInfoList[i];
    if(r_segFeeInfo.m_startDateTime>t_segFeeInfo.m_endDateTime ||
       r_segFeeInfo.m_startDateTime==t_segFeeInfo.m_endDateTime &&
       r_segFeeInfo.m_startData    >=t_segFeeInfo.m_endData)
    {
      continue;
    }else if(r_segFeeInfo.m_endDateTime< t_segFeeInfo.m_startDateTime ||
             r_segFeeInfo.m_endDateTime==t_segFeeInfo.m_startDateTime &&
             r_segFeeInfo.m_endData    <=t_segFeeInfo.m_startData )
    {
      break;
    }else if(r_segFeeInfo.m_startDateTime <  t_segFeeInfo.m_startDateTime ||
             r_segFeeInfo.m_startDateTime == t_segFeeInfo.m_startDateTime &&
             r_segFeeInfo.m_startData     <= t_segFeeInfo.m_startData)
    {
      if(r_segFeeInfo.m_endDateTime >  t_segFeeInfo.m_endDateTime ||
         r_segFeeInfo.m_endDateTime == t_segFeeInfo.m_endDateTime &&
         r_segFeeInfo.m_endData     == t_segFeeInfo.m_endData )
      { //该段全算
        r_segFeeInfoList.push_back(t_segFeeInfo);
        t_value += t_segFeeInfo.m_value;
      }else //(r_segFeeInfo.m_endDateTime < t_segFeeInfo.m_endDateTime||
            // r_segFeeInfo.m_endDateTime == t_segFeeInfo.m_endDateTime &&
            // r_segFeeInfo.m_endData     == t_segFeeInfo.m_endData)
      { //该段前部分算
        t_partSegInfo = t_segFeeInfo;
        t_partSegInfo.m_endDateTime = r_segFeeInfo.m_endDateTime;
        t_partSegInfo.m_endSecond   = r_segFeeInfo.m_endSecond;
        t_partSegInfo.m_duration    = t_partSegInfo.m_endSecond -
                                      t_partSegInfo.m_startSecond;
        t_partSegInfo.m_endData   = r_segFeeInfo.m_endData;
        t_partSegInfo.m_data      = r_segFeeInfo.m_endData -
                                    r_segFeeInfo.m_startData;
        t_partSegInfo.compute();
        r_segFeeInfoList.push_back(t_partSegInfo);
        t_value += t_partSegInfo.m_value;
        break;
      } 
    }else //if(r_segFeeInfo.m_startDateTime > t_segFeeInfo.m_startDateTime ||
          //   r_segFeeInfo.m_startDateTime == t_segFeeInfo.m_startDateTime &&
          //   r_segFeeInfo.m_startData     > t_segFeeInfo.m_startData )
    { 
      t_partSegInfo = t_segFeeInfo;
      t_partSegInfo.m_startDateTime = r_segFeeInfo.m_startDateTime;
      t_partSegInfo.m_startSecond   = r_segFeeInfo.m_startSecond;
      t_partSegInfo.m_startData     = r_segFeeInfo.m_startData;
      if(r_segFeeInfo.m_endDateTime > t_segFeeInfo.m_endDateTime ||
         r_segFeeInfo.m_endDateTime == t_segFeeInfo.m_endDateTime &&
         r_segFeeInfo.m_endData     >= t_segFeeInfo.m_endData)
      {//该段后半部分算
        t_partSegInfo.m_duration    = t_partSegInfo.m_endDateTime -
                                      t_partSegInfo.m_startDateTime;
        t_partSegInfo.m_data        = t_partSegInfo.m_endData -
                                      t_partSegInfo.m_startData;
        t_partSegInfo.compute();
        r_segFeeInfoList.push_back(t_partSegInfo);
        t_value += t_partSegInfo.m_value;
        continue;
      }else
      { //中间部分算
        t_partSegInfo.m_endDateTime = r_segFeeInfo.m_endDateTime;
        t_partSegInfo.m_endSecond   = r_segFeeInfo.m_endSecond;
        t_partSegInfo.m_duration    = t_partSegInfo.m_endDateTime -
                                      t_partSegInfo.m_startDateTime;
        t_partSegInfo.m_endData   = r_segFeeInfo.m_endData;
        t_partSegInfo.m_data        = t_partSegInfo.m_endData -
                                      t_partSegInfo.m_startData;
        t_partSegInfo.compute();
        r_segFeeInfoList.push_back(t_partSegInfo);
        t_value += t_partSegInfo.m_value;
        break;
      }
    }
  }
  t_value -= r_segFeeInfo.m_value;
  return t_value;
}


