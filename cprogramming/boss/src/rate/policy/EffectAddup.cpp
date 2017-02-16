#include "EffectAddup.h"
#include "AddupItem.h"
#include "PolicyInfo.h"
#include "base/exceptions.h"
#include "RateGlobalInfo.h"
#include "Price.h"

EffectAddup::EffectAddup(){}
EffectAddup::~EffectAddup(){}

bool EffectAddup::execute()
{
	//只做系统的累计
	if (m_addupItem != NULL)
	{
		//获取累计开始时间
		long sumBeginTime,sumEndTime;
		
		//Modified By Sunam 2009/12/29 10:33:38
		//if (!m_addupItem->getCyleTime(m_globalInfo,m_policyInfo.m_rateTpInfo,sumBeginTime,sumEndTime))
  		if (!m_addupItem->getCyleTime(m_globalInfo,m_policyInfo.m_rateTpInfo,m_globalInfo->m_beginDateTime,sumBeginTime,sumEndTime))
  		{
  			THROW_C_P1(BusinessException, -1, "getCyleTime error!"
    	        "addup_id=%d",m_addupItem->m_id);
  		}
		
  		
  		//获取当月的累积值
  		long curValue;
  		if (m_addupItem->getAcctCurValue(m_globalInfo, curValue) )
  		{
  			//进行与资费无关的累计
  			m_addupItem->updateAddupValue(m_globalInfo,
  			   m_policyInfo.m_rateTpInfo, sumBeginTime, sumEndTime, curValue,'0');
  		}
  		else
  		{  			
			THROW_C_P2(BusinessException, -1, "累计%d的元素值来源=%c 未定义!",
    	            m_addupItem->m_id,m_addupItem->m_elemValueSrc);
  		}		
	}
	else
	{
		THROW_C_P3(BusinessException, -1, "累计作用错误,累计账目指针为空："
                    "effect_id=%d,m_addupItemCode=%d,m_addupItemCode=%d", 
                    (long)m_id,m_addupItemCode, m_addupItemCode);
	}
	
	if (m_next != NULL)
	{
		m_next->execute();	
	}
	return true;
}

void EffectAddup::commitAddupBillInfo(const RateBillInfo &r_billInfo)
{
	if(r_billInfo.m_billValue == 0) return;
	RateGlobalInfo *t_pGlobalInfo = NodeBase::m_globalInfo;
  RateInfo       &t_rateInfo    = t_pGlobalInfo->m_rateFeeInfo;
  vector<RateBillInfo> &t_addupInfos = t_rateInfo.m_addupInfos;
  bool t_flag = true;;
  for(vector<RateBillInfo>::const_iterator t_curItr = t_addupInfos.begin();t_curItr!=t_addupInfos.end();++t_curItr)
  {
    //t_flag = true;
    if(r_billInfo == *t_curItr)//有相同累计
    {
      t_flag = false;
      break;
    }
  }
  if(t_flag)
  {
    t_addupInfos.push_back(r_billInfo);
  }
  return;
}

bool EffectAddup::rateExecute(const vector<CdrSeg> &r_pCdrSegList)
{
	
  RateGlobalInfo *t_pGlobalInfo = NodeBase::m_globalInfo;
  RateInfo       &t_rateInfo    = t_pGlobalInfo->m_rateFeeInfo;
  RateTpInfo     *t_rateTpInfo  = t_rateInfo.m_pCurRateTpInfo;
  FeeInfo        &t_curTpFeeInfo= t_rateInfo.m_curTpFeeInfo;
  AddupInfo      &t_curAddupInfo     = t_rateInfo.m_curAddupInfo;
  //Price          *t_curPrice = t_rateInfo.m_pCurPrice;
  //累计处理不进行费用计算，将m_curTpFeeInfo费用置成T1费用
  //int             t_feeField    = t_rateInfo.m_curFeeField;
  //FeeInfo        *t_pFeeInfo = &(t_rateInfo.m_cdrFeeInfo2.m_feeInfo[t_feeField]);
  //t_curTpFeeInfo.spUpdateFeeInfo(*t_pFeeInfo);
  //t_curTpFeeInfo = *t_pFeeInfo;
  //t_curTpFeeInfo.m_addupFlag = true;
  /*
  for(int i=0;i<t_curTpFeeInfo.m_seg;++i)
  {
    SegFeeInfo &t_segInfo = t_curTpFeeInfo.m_segInfoList[i];
    t_segInfo.m_tpObjId = t_rateTpInfo.m_pUserTp->m_tpObjId;
    t_segInfo.m_feePolicyId = t_rateTpInfo.m_rateTpId;
    //t_segInfo.m_tpId = (t_rateInfo.m_pCurPrice)->m_id;
    t_segInfo.m_tpId = t_curPrice->m_id;
  }*/
  t_curAddupInfo.clear();
  if(m_addupItem == NULL)
  {
  	THROW_C_P2(BusinessException, -1, "累计作用错误,累计账目指针为空:addup_id=%d,m_addupItemCode=%d",(long)m_id,m_addupItemCode);
  }
  if(m_addupItem->m_elemType == ADDUP_ELEM_TYPE_EVENT) //事件元素累计
  {
  	T_DATE_TIME r_cdrStartDateTime;
  	//DateTime t_tmpDateTime;
  	RateBillInfo t_addupInfo;
  	SegAddupInfo t_segAddupInfo;
  	
  	for(int seg=0; seg < r_pCdrSegList.size(); ++seg)
    {
      t_segAddupInfo = r_pCdrSegList[seg];
      if(m_addupItem->m_elemId == RATE_METHOD_TIME) //时长累计
      {
        t_segAddupInfo.m_objType = RATE_METHOD_TIME;
        t_segAddupInfo.m_unitValue = 60;
      }
      else
      {
        t_segAddupInfo.m_objType = RATE_METHOD_DATA;
        t_segAddupInfo.m_unitValue = 1024;
      }
      t_curAddupInfo.addSegAddupInfo(t_segAddupInfo);
    }
    t_curAddupInfo.ajustAddupInfoList();
    t_curAddupInfo.compute();
  	//

  	for(int seg=0; seg < t_curAddupInfo.m_seg; ++seg)
  	{
  		SegAddupInfo &t_segInfo = t_curAddupInfo.m_segInfoList[seg];
  		#ifdef _DEBUG_
  		cout << "累计段" << seg << endl;
  		cout << t_segInfo << endl;
  		cout << "---------END----------" << endl;
  		#endif
  		//t_tmpDateTime = t_segInfo.m_startDateTime;
  		r_cdrStartDateTime = t_segInfo.m_startDateTime.getDateTime();
  		//获取累计ID信息
  		if(!m_addupItem->getRateBillIdInfo(t_pGlobalInfo,
				                                 t_rateTpInfo,
				                                 r_cdrStartDateTime,
				                                 &t_segInfo.m_billId))
			{
				THROW_C_P2(BusinessException, -1, "累计量指针调用getRateBillIdInfo()失败："
						   		"addup_id=%d,addupItemId=%d", 
						   		(long)m_id,m_addupItem->m_id);
			}
			t_addupInfo = t_segInfo;
			//提交
			commitAddupBillInfo(t_addupInfo);
  	}
  }
  else
  {
  }
  if(m_next != NULL)
  {
  	m_next->rateExecute(r_pCdrSegList);
  }
  return true;
}

bool EffectAddup::acctExecute(long &sumBeginTime, long &sumEndTime)
{
	if (m_addupItem != NULL)
	{
		//获取累计开始时间
		if (sumBeginTime==0) 
		{	
			//Modified By Sunam 2009/12/29 10:33:38
  			//if (!m_addupItem->getCyleTime(m_globalInfo,m_policyInfo.m_rateTpInfo,sumBeginTime,sumEndTime))
  			if (!m_addupItem->getCyleTime(m_globalInfo,m_policyInfo.m_rateTpInfo,m_globalInfo->m_beginDateTime,sumBeginTime,sumEndTime))
  			{
  				THROW_C_P1(BusinessException, -1, "getCyleTime error!"
    	            "addup_id=%d",m_addupItem->m_id);
  			}
  		}
  		
  		//获取当月的累积值
  		long curValue;
  		if (m_addupItem->getAcctCurValue(m_globalInfo, curValue) )
  		{
  			m_addupItem->updateAddupValue(m_globalInfo, 
  			    m_policyInfo.m_rateTpInfo, sumBeginTime, sumEndTime, curValue,'1');
  		}
  		else
  		{  			
			THROW_C_P2(BusinessException, -1, "累计%d的元素值来源=%c 未定义!",
    	            m_addupItem->m_id,m_addupItem->m_elemValueSrc);
  		}		
	}
	else
	{
		THROW_C_P3(BusinessException, -1, "累计作用错误,累计账目指针为空："
                    "effect_id=%d,m_addupItemCode=%d,m_addupItemCode=%d", 
                    (long)m_id,m_addupItemCode, m_addupItemCode);
	}
	
	if (m_next != NULL)
	{
		m_next->acctExecute(sumBeginTime, sumEndTime);	
	}
	return true;
}

bool EffectAddup::getAcctMargin(const int &itemCode,long &marginFee,
                                long &sumBeginTime, long &sumEndTime)
{
	if (m_addupItem != NULL)
	{
		if (m_addupItem->m_elemType==ADDUP_ELEM_TYPE_ITEM &&
			m_addupItem->m_elemValueSrc==ADDUP_ELEM_VALUESRC_DISCNT &&
			m_addupItem->m_elemId==itemCode)
		{			
			if (sumBeginTime==0)
			{
				//Modified By Sunam 2009/12/29 10:33:38
  				//if (!m_addupItem->getCyleTime(m_globalInfo,m_policyInfo.m_rateTpInfo,sumBeginTime,sumEndTime))
				if (!m_addupItem->getCyleTime(m_globalInfo,m_policyInfo.m_rateTpInfo,m_globalInfo->m_beginDateTime,sumBeginTime,sumEndTime))
  				{
  					THROW_C_P1(BusinessException, -1, "getCyleTime error!"
    	    	        "addup_id=%d",m_addupItem->m_id);
  				}
  			}
  			
			//取已经使用的累积量
			m_addupItem->getAddupValue(m_globalInfo, m_policyInfo.m_rateTpInfo, 
                          sumBeginTime, sumEndTime, marginFee);
		
			long addupUpper;
			//获取累计上限
			if (m_addupUpper[0] == '?') //需要替换参数
			{
				T_TP_ID r_paramId;
				const char *paramValue;
				r_paramId = &m_addupUpper[1];
				paramValue = m_policyInfo.m_rateTpInfo->getParamValue(r_paramId);
				if (paramValue == NULL)
				{
					THROW_C_P2(BusinessException, -1, "累计配置错误,实例化的参数值为空："
    		                "addup_id=%d,order_no=%d", (long)m_id, m_orderNo);
				}
				addupUpper = atol(paramValue);
			}
			else if (m_addupUpper[0] == 'E') //表达式
			{
				char *expr;
				expr = &m_addupUpper[1];
				
				if (!m_globalInfo->explanExpr(addupUpper,expr,m_policyInfo.m_rateTpInfo))
				{
					THROW_C_P2(BusinessException, -1, "累计配置错误,表达式解析错误："
			                                  "addup_id=%d,m_addupUpper=%s",(long)m_id, m_addupUpper);
				}
				
			}
			else
			{
				addupUpper = atol(m_addupUpper);
			}
			
			if(addupUpper>marginFee)
				marginFee = addupUpper - marginFee;
			else
				marginFee = 0;
						
			return true;
		}
	}
	else
	{
		THROW_C_P3(BusinessException, -1, "累计作用错误,累计账目指针为空："
                    "effect_id=%d,m_addupItemCode=%d,m_addupItemCode=%d", 
                    (long)m_id,m_addupItemCode, m_addupItemCode);
	}
	
	if (m_next != NULL)
	{
		return m_next->getAcctMargin(itemCode, marginFee,
		                             sumBeginTime, sumEndTime);
	}
	return false;
}

void EffectAddup::dump(char *prefix)
{
	char myPrefix[256] = "  ";
	strcat(myPrefix,prefix);
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"---- EffectAddup : " <<m_id<<" ----" <<endl;
	cout <<myPrefix<<"m_id            = "<< m_id <<endl;
	cout <<myPrefix<<"m_orderNo       = "<< m_orderNo<<endl;
	cout <<myPrefix<<"m_addupItemCode = "<< m_addupItemCode<<endl;
	cout <<myPrefix<<"m_addupRoleId   = "<< m_addupRoleId<<endl;
	cout <<myPrefix<<"m_clearTag      = "<< m_clearTag<<endl;
	cout <<myPrefix<<"m_clearCycleNum = "<< m_clearCycleNum<<endl;
	cout <<myPrefix<<"m_addupUpper    = "<< m_addupUpper<<endl;
	if (m_addupItem==0)
		cout <<myPrefix<<"m_addupItem     = NULL"<<endl;
	else
	{
		cout <<myPrefix<<"m_addupItem     != NULL"<<endl;
		m_addupItem->dump(myPrefix);
	}
	
	if (m_next==0)
		cout <<myPrefix<<"m_next          = NULL"<<endl;
	else
	{
		cout <<myPrefix<<"m_next          != NULL"<<endl;
		m_next->dump(myPrefix);
	}
	cout <<myPrefix<<"}" <<endl;
}
