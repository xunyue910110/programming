#include "SpPriceProc_Unicom.h"

INSTALL_PROCEDURE_INSTANCE(SpPriceProc_Unicom);

//按天分割话单，形成话单天分割段列表
bool SpPriceProc_Unicom::getCdrSegments()
{ //考虑按天、忙闲时、忙闲时流量分段
  CdrSeg  &t_cdrSegment = m_pRateFeeInfo->m_cdrSegment;
  vector<CdrSeg>::iterator t_itr;
  vector<CdrSeg> &t_desCdrSegs = m_pRateFeeInfo->m_cdrSegList;
  
  //1.先按天分段
  if(PriceBaseProc::getCdrSegments(t_cdrSegment,t_desCdrSegs)==false)
  {
    return false;
  }

  //2.计算各段的流量
  long t_totalTime=m_pcdr[ATTRI_DURATION].value();
  long t_totalData=0,t_sData=0,t_dataRatio = 0;
  int  t_time;
  t_totalData = m_pcdr[ATTRI_SP_TOTALDATA].value();
  t_dataRatio = (t_totalTime > 0 )?(t_totalData/t_totalTime):0;
  for(t_itr=t_desCdrSegs.begin();t_itr!=t_desCdrSegs.end();++t_itr)
  {
    t_itr->m_startData = t_sData;
    if(t_itr == t_desCdrSegs.end()-1)
    {
      t_itr->m_data = t_totalData-t_sData;
    }else
    {
      t_itr->m_data = t_itr->m_duration*t_dataRatio;
    }
    t_sData += t_itr->m_data;
    t_itr->m_endData = t_itr->m_startData+t_itr->m_data;
  }
  t_cdrSegment.m_startData = 0;
  t_cdrSegment.m_endData   = t_totalData;
  t_cdrSegment.m_data      = t_totalData;
  /*
  #ifdef _DEBUG_
    cout << "＝＝＝ DAY SEGMENT INFO ＝＝＝" << endl;
    for (t_itr=t_desCdrSegs.begin();t_itr!=t_desCdrSegs.end();++t_itr)
    {
        cout << "  --------- "<<endl;
        cout << *t_itr <<endl;
    }
  #endif //_DEBUG_
  */
  return true;
}

//设置原始费用信息
void SpPriceProc_Unicom::setOrgFeeInfo(const int &r_feeField)
{
  SegFeeInfo t_segFeeInfo;
  FeeInfo &t_feeInfo = m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[r_feeField];
  CdrSeg  &t_cdrSegment=m_pRateFeeInfo->m_cdrSegment;
  //设置费用，将费用结果存放在 t_feeInfo 中
  long  t_feeSum,t_unitCount,t_remain,t_unitFee;
  long  t_data,t_unitData;
  long  t_unitTime;

  //总费用
  t_feeSum   = m_pcdr[ATTRI_ORGFEE_FIELD1+r_feeField-1].value();
  if(m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_DATA ||
     m_pcdr[ATTRI_SP_RATEUNIT] == SP_RATE_UNIT_CLDATA)
  {
    //流量计费
    t_data     = m_pRateFeeInfo->m_cdrSegment.m_endData;
    t_unitData = m_pcdr[ATTRI_FEEUNIT_1+r_feeField-1].value();
    t_unitCount= (t_data+t_unitData-1)/t_unitData;
    if(t_unitCount>0)
    {
      t_remain   = t_feeSum % t_unitCount;
      t_unitFee  = (t_feeSum-t_remain)/t_unitCount;
    }
    for(int i=0;i<m_pRateFeeInfo->m_cdrSegList.size();++i)
    {
      t_segFeeInfo = m_pRateFeeInfo->m_cdrSegList[i];
      t_segFeeInfo.m_unitData= t_unitData;
      t_segFeeInfo.m_unitFee = t_unitFee;
      t_segFeeInfo.m_rateMethod = RATE_METHOD_DATA;
      t_segFeeInfo.m_segType =FEE_SEG;
      t_feeInfo.addSegFeeInfo(t_segFeeInfo);
    }
    t_feeInfo.ajustFeeInfoList(t_cdrSegment.m_endDateTime,t_cdrSegment.m_endData);
    if(t_remain>0)
    {
      SegFeeInfo& t_segFeeInfo0 = t_feeInfo.m_segInfoList[t_feeInfo.m_seg-1];
      if(t_segFeeInfo0.m_unitCount > 1)
      {
        t_segFeeInfo0.m_duration     = (t_segFeeInfo0.m_unitCount-1)*t_unitData*t_segFeeInfo0.m_duration/t_segFeeInfo0.m_data;
        t_segFeeInfo0.m_unitCount   -= 1;
        t_segFeeInfo0.m_value       -= t_unitFee;
        t_segFeeInfo0.m_data         = t_unitData*t_segFeeInfo0.m_unitCount;
        t_segFeeInfo0.m_endData     -= t_unitData;
        t_segFeeInfo0.m_endSecond    = t_segFeeInfo0.m_startSecond+t_segFeeInfo0.m_duration;
        t_segFeeInfo0.m_endDateTime  = t_segFeeInfo0.m_startDateTime+
                                       t_segFeeInfo0.m_duration;
        
        t_segFeeInfo.m_startDateTime= t_segFeeInfo0.m_endDateTime;  
        t_segFeeInfo.m_endDateTime  = t_cdrSegment.m_endDateTime;
        t_segFeeInfo.m_startSecond  = t_segFeeInfo0.m_endSecond;
        t_segFeeInfo.m_endSecond    = t_cdrSegment.m_endSecond;
        t_segFeeInfo.m_duration     = t_segFeeInfo.m_endSecond-t_segFeeInfo.m_startSecond;
        t_segFeeInfo.m_unitFee      = t_unitFee + t_remain;
        t_segFeeInfo.m_value        = t_unitFee + t_remain;
        t_segFeeInfo.m_startData    = t_segFeeInfo0.m_endData;
        t_segFeeInfo.m_unitData     = t_unitData;
        t_feeInfo.addSegFeeInfo(t_segFeeInfo);
        t_feeInfo.ajustFeeInfoList(t_cdrSegment.m_endDateTime,t_cdrSegment.m_endData);
      }else
      {
        t_segFeeInfo.m_unitFee += t_remain;
        t_segFeeInfo.m_value   += t_remain;
      }
    }
    t_feeInfo.m_rateTime = RATE_TIMES_1;
    t_feeInfo.m_exeMethod= EXEC_METHOD_ALL;
    t_feeInfo.computeFee();  
    return;
  }else //其他作为时长计费方式
  {
    //设置费用，将费用结果存放在 t_feeInfo 中
    t_unitTime = m_pcdr[ATTRI_FEEUNIT_1+r_feeField-1].value();
    t_unitCount= (m_pcdr[ATTRI_DURATION].value()+t_unitTime-1)/t_unitTime;
    if(t_unitCount>0)
    {
      t_remain   = t_feeSum % t_unitCount;
      t_unitFee  = (t_feeSum-t_remain)/t_unitCount;
    }
    for(int i=0;i<m_pRateFeeInfo->m_cdrSegList.size();++i)
    {
      t_segFeeInfo = m_pRateFeeInfo->m_cdrSegList[i];
      t_segFeeInfo.m_unitTime=t_unitTime;
      t_segFeeInfo.m_unitFee =t_unitFee;
      t_segFeeInfo.m_segType =FEE_SEG;
      t_segFeeInfo.m_rateMethod=RATE_METHOD_TIME;
      t_feeInfo.addSegFeeInfo(t_segFeeInfo);
    }
    t_feeInfo.ajustFeeInfoList(m_pRateFeeInfo->m_cdrSegment.m_endDateTime,
                               m_pRateFeeInfo->m_cdrSegment.m_endData);
    if(t_remain>0)
    {
      SegFeeInfo& t_segFeeInfo0 = t_feeInfo.m_segInfoList[t_feeInfo.m_seg-1];
      if(t_segFeeInfo0.m_unitCount > 1)
      {
        t_segFeeInfo0.m_unitCount   -= 1;
        t_segFeeInfo0.m_duration     = t_unitTime*t_segFeeInfo0.m_unitCount;
        t_segFeeInfo0.m_endSecond   -= t_unitTime;
        t_segFeeInfo0.m_endDateTime  = t_segFeeInfo0.m_startDateTime+
                                       t_segFeeInfo0.m_duration;
        
        t_segFeeInfo.m_startDateTime= t_segFeeInfo0.m_endDateTime;  
        t_segFeeInfo.m_endDateTime  = t_cdrSegment.m_endDateTime;
        t_segFeeInfo.m_startSecond  = t_segFeeInfo0.m_endSecond ;
        t_segFeeInfo.m_endSecond    = m_pcdr[ATTRI_DURATION].value();
        t_segFeeInfo.m_duration     = t_segFeeInfo.m_endDateTime-t_segFeeInfo.m_startDateTime;
        t_segFeeInfo.m_unitFee      = t_unitFee + t_remain;
        t_segFeeInfo.m_unitTime     = t_unitTime;
        t_feeInfo.addSegFeeInfo(t_segFeeInfo);
        t_feeInfo.ajustFeeInfoList(m_pRateFeeInfo->m_cdrSegment.m_endDateTime,
                                   m_pRateFeeInfo->m_cdrSegment.m_endData);
      }else
      {
        t_segFeeInfo.m_unitFee += t_remain;
        t_segFeeInfo.m_value   += t_remain;
      }
    }
    t_feeInfo.m_rateTime = RATE_TIMES_1;
    t_feeInfo.m_exeMethod= EXEC_METHOD_ALL;
    t_feeInfo.computeFee();    
    return;
  }
}



