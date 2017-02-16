#include "Price.h"
#include "PriceComp.h"
#include "NodeBase.h"
#include "RateGlobalInfo.h" 
#include "EventFeePolicy.h"
#include "base/exceptions.h"

PolicyInfo * Price::m_policyInfo = &NodeBase::m_policyInfo;

bool Price::execute() 
{
	m_policyInfo->m_price = this;
	if(m_priceComp != NULL)	
		return m_priceComp->execute();		
	else	
		return true;
}
//返回值：true 
bool Price::rateExecute(int &r_errCode)
{
  RateGlobalInfo *t_pGlobalInfo = NodeBase::m_globalInfo;
  RateInfo   &t_rateInfo   = t_pGlobalInfo->m_rateFeeInfo;
  int        &t_feeField   = t_rateInfo.m_curFeeField;
  CdrFeeInfo &t_cdrFeeInfo = t_rateInfo.m_cdrFeeInfos[t_rateInfo.m_curFeeIdx];
  FeeInfo    &t_curTpFeeInfo=t_rateInfo.m_curTpFeeInfo;
  char t_errmsg[128];
  t_cdrFeeInfo.m_noRateFlag[t_feeField]=true;
	t_cdrFeeInfo.m_feeInfo[t_feeField].clear();
	
	//初始化设置：t_rateInfo.m_curTpFeeInfo
	//强制作用的采用T0的作为基准。否则以T1作为基准
	FeeInfo  *t_pFeeInfo;
	if(t_rateInfo.m_pCurRateTpInfo->m_execMode == EXEC_METHOD_EXCLUDE||
	   t_rateInfo.m_pCurRateTpInfo->m_execMode == EXEC_METHOD_SUPEREXCLUDE)
  {
    if(t_rateInfo.m_cdrFeeInfo.m_noRateFlag[t_feeField]==true)
    {
      t_pFeeInfo = &(t_rateInfo.m_cdrFeeInfo2.m_feeInfo[t_feeField]);
    }else
    {
      t_pFeeInfo = &(t_rateInfo.m_cdrFeeInfo.m_feeInfo[t_feeField]);
    }
  }else 
  {
    t_pFeeInfo = &(t_rateInfo.m_cdrFeeInfo2.m_feeInfo[t_feeField]);
  }
  
  t_curTpFeeInfo = *t_pFeeInfo;

  if(m_priceComp != NULL)	
  {
      //考虑到用户资费有效期截止问题，需要对已过期的进行过滤 modified by gaojf 2009-10-12 21:58:16
      //m_priceComp->rateExecute(t_rateInfo.m_cdrSegList);
      if ((t_pGlobalInfo->m_pConfig->m_operatorType == OPERATOR_TYPE_UNICOM) &&
          (t_pGlobalInfo->m_pConfig->m_provinceCode == "022") &&
          (t_pGlobalInfo->m_bizType == BIZ_TYPE_NET))
      {
          m_priceComp->rateExecute(t_rateInfo.m_cdrSegList);
      }
      else
      {
          vector<CdrSeg> t_cdrSegs;
          CdrSeg t_tempSeg;
          t_cdrSegs.clear();
          DateTime t_sDateTime,t_eDateTime;
          t_sDateTime.set(t_rateInfo.m_pCurRateTpInfo->m_pUserTp->m_beginTime.substr(0,8),
                          t_rateInfo.m_pCurRateTpInfo->m_pUserTp->m_beginTime.substr(8,6));
          t_eDateTime.set(t_rateInfo.m_pCurRateTpInfo->m_pUserTp->m_endTime.substr(0,8),
                          t_rateInfo.m_pCurRateTpInfo->m_pUserTp->m_endTime.substr(8,6));
          for(vector<CdrSeg>::iterator t_itr=t_rateInfo.m_cdrSegList.begin();
              t_itr!=t_rateInfo.m_cdrSegList.end();++t_itr)
          {
              if (t_itr->split(t_sDateTime, t_eDateTime, t_tempSeg))
              {
              	  t_cdrSegs.push_back(t_tempSeg);
              }
          }
          /*Modified by Sunam 2009/10/27 16:40:43 
            如果t_rateInfo.m_cdrSegList只有一个零时长分段的话，在
            if (t_itr->split(t_sDateTime, t_eDateTime, t_tempSeg))中是无法保留这个分段的
            因此在这判断一下如果分割出来的分段为零的话，就用原始分段去计算
          */
          //m_priceComp->rateExecute(t_cdrSegs);
          if (t_cdrSegs.size() > 0)
          {
              m_priceComp->rateExecute(t_cdrSegs);
          }
          else
          {
              m_priceComp->rateExecute(t_rateInfo.m_cdrSegList);
          }
      }
	}else
	{
	  sprintf(t_errmsg,"批价政策:%d 没有定义!",m_id);
	  t_pGlobalInfo->setErrInfo(E_PRICEPARAM1,t_errmsg);
	  r_errCode = E_PRICEPARAM1;
	  #ifdef _DEBUG_
	    cout<<t_errmsg<<endl;
	  #endif
    THROW_C_P1(BusinessException, -1, "批价政策:%d 没有定义!",m_id);
	  return false;
	}
	/*
	#ifdef _DEBUG_
	  cout<<"整条政策批价后!"<<endl;
	  cout<<t_curTpFeeInfo<<endl;
	#endif
	*/
	t_curTpFeeInfo.ajustFeeInfoList(t_rateInfo.m_cdrSegment.m_endDateTime,
	                                t_rateInfo.m_cdrSegment.m_endData);
	if(t_curTpFeeInfo.m_coverFlag==false )
	{ //要求第一条政策必须覆盖整条话单
	  sprintf(t_errmsg,"批价政策:%d 资费不能覆盖整条话单!",m_id);
	  t_pGlobalInfo->setErrInfo(E_PRICEPARAM2,t_errmsg);
	  r_errCode = E_PRICEPARAM2;
	  #ifdef _DEBUG_
	    cout<<t_errmsg<<endl;
	  #endif
	  THROW_C_P1(BusinessException, -1, "批价政策:%d 资费不能覆盖整条话单!",m_id);
	  return false;
	}
	t_curTpFeeInfo.m_exeMethod=t_rateInfo.m_pCurRateTpInfo->m_execMode;
	t_curTpFeeInfo.m_rateTime =t_rateInfo.m_pCurEventPolicy->m_rateTime;  
	//更新批价结果
  t_curTpFeeInfo.computeFee(m_carryTag);
  t_cdrFeeInfo.m_feeInfo[t_feeField] = t_curTpFeeInfo;
  t_cdrFeeInfo.ComputeFeeInfo();
  //将自身生效t_curTpFeeInfo的赠送时长和费用更新到T1中
	t_pFeeInfo = &(t_rateInfo.m_cdrFeeInfo2.m_feeInfo[t_feeField]);
	t_pFeeInfo->spUpdateFeeInfo(t_curTpFeeInfo);
	
  return true;
}

void Price::dump(char *prefix)
{
	char myPrefix[256] = "  ";
	strcat(myPrefix, prefix);

	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"---- Price : " <<  m_id <<" ----" <<endl;	
	cout <<myPrefix<<"m_id          = "<< m_id <<endl;                        
	//cout <<myPrefix<<"m_execMode    = "<< m_execMode<<endl;
	cout <<myPrefix<<"m_carryTag    = "<< m_carryTag<<endl;
	cout <<myPrefix<<"m_sumToIntFee	= "<< m_sumToIntFee<<endl; 
	cout <<myPrefix<<"m_sumToIntType= "<< m_sumToIntType<<endl; 

	if(m_priceComp != NULL)                      
	{
		cout <<myPrefix<<"m_priceComp   != NULL"<<endl;
		m_priceComp->dump(myPrefix);
	}
	else
		cout <<myPrefix<<"m_priceComp   = NULL"<<endl;
				
	if(m_priceExtend != NULL)	
	{
		int order = 0;
		cout <<myPrefix<<"m_priceExtend != NULL"<<endl;
		cout <<myPrefix<<"  {" <<endl; 
		m_priceExtend->dump(myPrefix, order);
		cout <<myPrefix<<"  }" <<endl;
	}	
	else
		cout <<myPrefix<<"m_priceExtend = NULL"<<endl;
	cout <<myPrefix<<"}" <<endl;
}

