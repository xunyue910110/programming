#include "PriceProc.h"
#include "RateError.h"
#include "PMManager.h"
#include "EventFeePolicy.h"
#include "Price.h"
#include "NodeBase.h"
#include "base/exceptions.h"

INSTALL_PROCEDURE_INSTANCE(PriceProc);


//设置原始费用信息
void PriceProc::setOrgFeeInfo(const int  &r_feeField)
{
  SegFeeInfo t_segFeeInfo;
  FeeInfo &t_feeInfo = m_pRateFeeInfo->m_cdrFeeInfo.m_feeInfo[r_feeField];
  CdrSeg  &t_cdrSegment=m_pRateFeeInfo->m_cdrSegment;
  //设置费用，将费用结果存放在 t_feeInfo 中
  long  t_feeSum,t_unitTime,t_unitCount,t_remain,t_unitFee;
  t_feeSum   = m_pcdr[ATTRI_ORGFEE_FIELD1+r_feeField-1].value();
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
}

bool PriceProc::specialProcess()
{
    //Added By Sunam 2009/9/25 17:11:51 
    char buffer[100];
    memset(buffer,0,100);
    //in_cell|isnoneed_tp|roam_type|speicial_phone_group
    sprintf(buffer,"%s|%s|%s|%s",m_pcdr[ATTRI_CELL_FLAG].c_str(),
                                 m_pcdr[ATTRI_ISNONEED_TP].c_str(),
                                 m_pcdr[ATTRI_ROAM_TYPE].c_str(),
                                 m_pcdr[ATTRI_OTHERGROUP].c_str());
    m_pcdr[ATTRI_FIXVOICE_COMPATTR] = buffer;
   
  //1.  国际出访主叫费用拆分成漫游费(2.88)和长途费
  //湖南移动出访费用不拆分 海南移动出访费用不拆分 云南移动出访费用不拆
  if (!(m_pRateConfig->m_operatorType == OPERATOR_TYPE_CMCC &&
        (m_pRateConfig->m_provinceCode == "731" ||
         m_pRateConfig->m_provinceCode == "898" ||
  	     m_pRateConfig->m_provinceCode != "871")))
  {
  	if ((m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_OUT) && 
  		  (m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MOC))
  	{
      int t_times,t_dur;
      int t_cfee,t_lfee;
      CdrFeeInfo  &t_cdrFeeInfo=m_pRateFeeInfo->m_cdrFeeInfo;
      //按照2.88/分钟进行拆分
      t_dur = m_pcdr[ATTRI_DURATION].value();
      t_times = (t_dur + DEFAULT_CFEE_UNIT - 1)/DEFAULT_CFEE_UNIT;
      t_cfee  = t_times * 2880;
      t_lfee = t_cdrFeeInfo.m_feeInfo[ATTRI_DISCFEE_FIELD1].m_feeSum - t_cfee;
      
      if (t_lfee >= 0)
      {
      	t_cdrFeeInfo.m_feeInfo[ATTRI_DISCFEE_FIELD1].m_feeSum = t_cfee;
      	t_cdrFeeInfo.m_feeInfo[ATTRI_DISCFEE_FIELD3].m_feeSum = t_lfee;
      }
  	}
  }
  //长株潭等特殊资费要求  coding here later
  return true;
}


