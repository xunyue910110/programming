#include "GprsPriceProc_Unicom.h"

INSTALL_PROCEDURE_INSTANCE(GprsPriceProc_Unicom);

//按天分割话单，形成话单天分割段列表
bool GprsPriceProc_Unicom::getCdrSegments()
{ //考虑按天、忙闲时、忙闲时流量分段
  CdrSeg  &t_cdrSegment = m_pRateFeeInfo->m_cdrSegment;
  vector<CdrSeg> t_cdrSegs;
  vector<CdrSeg>::iterator t_itr;
  vector<CdrSeg> &t_desCdrSegs = m_pRateFeeInfo->m_cdrSegList;
  
  //1.先按天分段
  if(PriceBaseProc::getCdrSegments(t_cdrSegment,t_cdrSegs)==false)
  {
    return false;
  }
  //2.再按忙闲时分
  long     t_busiTime=0,t_idleTime=0,t_totalTime=0;
  DateTime t_tmp1_tp,t_tmp2_tp;
  for(t_itr=t_cdrSegs.begin();t_itr!=t_cdrSegs.end();++t_itr)
  {
    t_tmp1_tp.setDate(t_itr->m_startDateTime.getDateInteger());
    t_tmp2_tp.setDate(t_itr->m_startDateTime.getDateInteger());
    
    t_tmp1_tp.setTime(DATETIME_0);t_tmp2_tp.setTime(DATETIME_1);
    if(t_itr->split(t_tmp1_tp,t_tmp2_tp,t_cdrSegment)==true)
    {
      t_idleTime+=t_cdrSegment.m_duration;
      t_desCdrSegs.push_back(t_cdrSegment);
    }
    t_tmp1_tp.setTime(DATETIME_1);t_tmp2_tp.setTime(DATETIME_2);
    if(t_itr->split(t_tmp1_tp,t_tmp2_tp,t_cdrSegment)==true)
    {
      t_busiTime+=t_cdrSegment.m_duration;
      t_desCdrSegs.push_back(t_cdrSegment);
    }

    t_tmp1_tp.setTime(DATETIME_2);t_tmp2_tp.setTime(DATETIME_3);
    if(t_itr->split(t_tmp1_tp,t_tmp2_tp,t_cdrSegment)==true)
    {
      t_idleTime+=t_cdrSegment.m_duration;
      t_desCdrSegs.push_back(t_cdrSegment);
    }
  }
  t_totalTime = t_busiTime+t_idleTime;
  // 经江苏联通于永红确认,GPRS时长为0不加判
  if(t_totalTime <= 0) 
  {
    //setErrorCode(E_DATETIME_LONG);
    //return false;
    t_desCdrSegs=t_cdrSegs;
  }
  //3.计算各段的流量
  long t_busiData=0,t_idleData=0,t_totalData=0;
  long t_busiDataRatio = 0;
  long t_idleDataRatio = 0;
  int  t_time;
  t_busiData = m_pcdr[ATTRI_GPRS_DATA_UP1].value()+m_pcdr[ATTRI_GPRS_DATA_DOWN1].value();
  t_idleData = m_pcdr[ATTRI_GPRS_DATA_UP2].value()+m_pcdr[ATTRI_GPRS_DATA_DOWN2].value();
  m_pcdr[ATTRI_SP_TOTALDATA] = t_busiData+t_idleData;
  //t_busiDataRatio = (t_busiTime > 0 )?(t_busiData/t_busiTime):0;
  //t_idleDataRatio = (t_idleTime > 0 )?(t_idleData/t_idleTime):0;
  t_busiDataRatio = t_idleDataRatio = (t_totalTime>0)?
                  ((t_busiData+t_idleData)/t_totalTime):0;
  for(t_itr=t_desCdrSegs.begin();t_itr!=t_desCdrSegs.end();++t_itr)
  {
    t_itr->m_startData = t_totalData;
    if(t_itr == t_desCdrSegs.end()-1)
    {
      t_itr->m_data = t_busiData+t_idleData-t_totalData;
    }else
    {
      t_time = t_itr->m_startDateTime.getTimeInteger();
      if((t_time >= DATETIME_0 &&t_time <= DATETIME_1 ) ||
         (t_time >= DATETIME_2 &&t_time <= DATETIME_3 ))
      { //闲时
        t_itr->m_data = t_itr->m_duration*t_idleDataRatio;
      }else //忙时
      {
        t_itr->m_data = t_itr->m_duration*t_busiDataRatio;
      }
    }
    t_totalData += t_itr->m_data;
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
void GprsPriceProc_Unicom::setOrgFeeInfo(const int &r_feeField)
{
  SegFeeInfo t_segFeeInfo;
  FeeInfo &t_feeInfo = m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[r_feeField];
  CdrSeg  &t_cdrSegment=m_pRateFeeInfo->m_cdrSegment;
  //设置费用，将费用结果存放在 t_feeInfo 中
  long  t_feeSum,t_unitCount,t_remain,t_unitFee;
  long  t_data,t_unitData;
  //总费用
  t_feeSum   = m_pcdr[ATTRI_ORGFEE_FIELD1+r_feeField-1].value();
  //GPRS计费方式默认为流量计费
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
    t_segFeeInfo.m_unitData=t_unitData;
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
}



