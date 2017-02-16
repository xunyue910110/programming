#include "AdjustBeforeCalc.h"
#include "ObjectBase.h"
#include "EffectDiscnt.h" //共享常量定义 add by xuf 2009-11-9 13:42:49

//##ModelId=48F94C3F0282
AdjustBeforeCalc::~AdjustBeforeCalc()
{
}

//##ModelId=48F94C3F028B
AdjustBeforeCalc::AdjustBeforeCalc()
{
	m_pAccountParam = NULL;
	m_pComBill = NULL;
	m_adjustBeforeVector = NULL;
}

//##ModelId=48F94C3F03B0
bool AdjustBeforeCalc::dealAdjust(const Discount &r_discount)
{
	  AdjustBefore l_adjustBefore;
    long l_sumnfee=0; //此次优惠本金   
    long l_effectFee=0;//实际作用值
    long l_usedfee=0; //到目前为止使用值
    
    //取调帐信息
    if(!getAdjustBeforeById(r_discount.m_adjustInsId, l_adjustBefore))
    {
    	DEBUG_PRINT("[%s:%d] 未获取到帐前调整信息!\n");
    	m_pRateInfo->m_comBill.setErrorCode(m_id,E_ACCT_GET_ADJUSTBEFORE,"0",m_idTypeCode);
    	return false;	
    }
    
    //取调帐费用      
    if(!getAdjustBeforeFee(l_adjustBefore,r_discount,l_sumnfee, l_effectFee,l_usedfee)) 
    {
    	DEBUG_PRINT("[%s:%d] 获取帐前调整费用错误!\n",__FILE__,__LINE__);
    	m_pRateInfo->m_comBill.setErrorCode(m_id,E_ACCT_GET_ADJUSTFEE,"0",m_idTypeCode);
    	return false;	
    } 
    
    //更新调整费用
    updateAdjustBeforeFee(l_adjustBefore, l_sumnfee, l_effectFee, r_discount);
                         
    //需要累计
    if (l_adjustBefore.m_addupMode != '0')
    {
       updateAddupInfo(r_discount.m_adjustInsId,r_discount.m_addupItem,(Integer)r_discount.m_startDate,(Integer)r_discount.m_endDate, l_usedfee);
    }
	
	return true;
}

//##ModelId=48FD532A0189
void AdjustBeforeCalc::initial(const T_SERVER_ID &r_id ,
	                             ID_TYPE r_idType,
	                             PM_Account *r_pAccountParam, 
	                             const CycParam   &r_cycParam,
	                             RateGlobalInfo *r_pRateInfo,
	                             vector<AdjustBefore> *r_adjustBeforeVector
	                             )
{
	m_id = r_id;
	m_idTypeCode = r_idType;
	m_pAccountParam = r_pAccountParam;
	m_cycParam = r_cycParam;
	m_pComBill = &r_pRateInfo->m_comBill;
	m_pRateInfo = r_pRateInfo;
	m_adjustBeforeVector = r_adjustBeforeVector;
}

//##ModelId=48F94C3F02D1
bool AdjustBeforeCalc::getAddupInfo(const long &r_adjustInsId, const long &r_beginTime, const int &r_addupItem, long &r_addupValue)
{
  UserSumBill2 l_sumBill2;
  const UserSumBill2 *l_pLastSumBill2=NULL;
  
  l_sumBill2.m_userId       = m_id;     
  l_sumBill2.m_tpObjId      = r_adjustInsId;    
  l_sumBill2.m_beginTime    = r_beginTime;  
  l_sumBill2.m_cycleId      = m_cycParam.m_cycleId;     
  l_sumBill2.m_billId       = r_addupItem;
  	
  if (!m_pRateInfo->getLastSumBill2(m_pRateInfo->m_userSumBill2s_new,
                                    l_sumBill2,
                                    l_pLastSumBill2)) 	
  {
  	r_addupValue = 0;
    return false;	
  }
  
  r_addupValue=l_pLastSumBill2->m_value/m_pComBill->m_divisor;
  
	return true;
}


//##ModelId=48F94C3F0326
bool AdjustBeforeCalc::getAdjustBeforeById(const long &r_adjustInsId, AdjustBefore &r_adjustBefore)
{
	
	    for (vector<AdjustBefore>::const_iterator lit=(*m_adjustBeforeVector).begin();
        lit!=(*m_adjustBeforeVector).end();++lit)
    {
        if ((*lit).m_adjustInsId == r_adjustInsId)
	    {
	    		 r_adjustBefore = *lit;           
           return true;
	    }
	}
	
	return false;
}

//##ModelId=48F94C3F032C
bool AdjustBeforeCalc::getAdjustBeforeFee(const AdjustBefore &r_adjustBefore,const Discount &r_discount, long &r_nfee, long &r_effectFee,long &r_usedFee)
{
	long l_remainFee=0; //调整剩余金额
	long l_usedFee=0;  //调帐已经使用金额
	long l_sumfee=0;   //此次优惠基于的本金
	
	if(r_adjustBefore.m_addupMode != '0')
	{
		getAddupInfo(r_adjustBefore.m_adjustInsId, (Integer)r_discount.m_startDate, r_discount.m_addupItem, l_usedFee);
  }
	
	l_remainFee = r_adjustBefore.m_effectValue-l_usedFee;
	
	//根据参考帐目获取费用     
	getReferItemFee(r_discount.m_referItem,r_nfee);
	
	l_sumfee=r_nfee;
        
    //补费
    if ((r_adjustBefore.m_effectMode=='0') || (r_adjustBefore.m_effectMode=='5'))
    {
        //按金额
        if (r_adjustBefore.m_effectValueType == '0')
        {
            r_effectFee=l_remainFee;
        }
        else
        {
            r_effectFee = 1.0 * l_remainFee * l_sumfee 
                            / PRECISION_FACTOR + 0.5;
        }
        
        //和用户优惠限定比较
        if ((r_effectFee>r_adjustBefore.m_limitFee) && (r_adjustBefore.m_limitFee!=0))
        {
            r_effectFee = r_adjustBefore.m_limitFee;
        }
        
        r_effectFee=-1*r_effectFee;
    }  
    //退费
    else if ((r_adjustBefore.m_effectMode=='1') || (r_adjustBefore.m_effectMode=='6'))
    {
        //按金额
        if (r_adjustBefore.m_effectValueType == '0')
        {
            r_effectFee = l_remainFee;
        }
        else
        {
            r_effectFee = 1.0 * l_remainFee * l_sumfee 
                            / PRECISION_FACTOR + 0.5;
        }
            
        //和用户优惠限定比较
        if ((r_effectFee>r_adjustBefore.m_limitFee) && (r_adjustBefore.m_limitFee!=0)) 
        {
            r_effectFee = r_adjustBefore.m_limitFee;
        }
        
        //特殊累计作用
        if ((r_adjustBefore.m_addupMode=='2') && (r_adjustBefore.m_limitFee!=0)) 
        {
            r_effectFee = r_adjustBefore.m_limitFee*
                        	(MinusCycle(m_cycParam.m_cycleId,r_adjustBefore.m_startCycId)+2)-
                        	(r_adjustBefore.m_effectValue-l_remainFee);
        }

#ifndef _NEGATIVEBILL_        
        //和用户实际费用比较
        if (r_effectFee>l_sumfee)
        {
          r_effectFee = l_sumfee;
        }
#endif
    }
    //上限
    else if ((r_adjustBefore.m_effectMode=='2') || (r_adjustBefore.m_effectMode=='7'))
    {
        //按金额
        if (r_adjustBefore.m_effectValueType == '0')
        {
            r_effectFee = l_sumfee-l_remainFee;
        }
        else
        {
            r_effectFee = 0;
        }
        
        if (r_effectFee < 0)
        {
            r_effectFee = 0;
        }
    }
    //下限
    else if ((r_adjustBefore.m_effectMode=='3') || (r_adjustBefore.m_effectMode=='8'))
    {
        if (r_adjustBefore.m_effectValueType == '0')
        {
            r_effectFee = l_sumfee - l_remainFee;
        }
        else
        {
            r_effectFee = 0;
        }
            
        if (r_effectFee > 0)
        {
            r_effectFee = 0;
        }
    }
    //设定
    else if ((r_adjustBefore.m_effectMode=='4') || (r_adjustBefore.m_effectMode=='9'))
    {
        if (r_adjustBefore.m_effectValueType == '0')
        {
            r_effectFee=l_sumfee-l_remainFee;
        }
        else
        {
            r_effectFee = 0;
        }
    }
    else
    {    
    	  DEBUG_PRINT("TS_A_ADJUSTBEFORE帐前调整信息effectMode错误!\n");
        r_effectFee=0;
        m_pRateInfo->m_comBill.setErrorCode(m_id,E_ACCT_GET_ADJUSTMODE,"0");
        return false;
    }
	
	//已经使用的费用
	r_usedFee = l_usedFee + r_effectFee;
	return true;
}

/* updateAdjustBeforeFee:根据调账更新账单
 * @param  r_adjustBefore    调账信息.
 * @param  r_nfee            优惠后费用.
 * @param  r_effectFee       调账金额.     
 * @param  r_discount        优惠信息
 * @return 无
*/   
//##ModelId=48F94C3F036D
void AdjustBeforeCalc::updateAdjustBeforeFee(AdjustBefore &r_adjustBefore, const long &r_nfee, long &r_effectFee, const Discount &r_discount)
{
	  char l_discountType;
    
    //0：优惠补费 1：优惠退费 2：优惠上限 3：优惠下限 4：优惠设定 
    //5：帐前补费 6：帐前退费 7：帐前上限 8：帐前下限 9：帐前设定
    if ((r_adjustBefore.m_effectMode=='0') || (r_adjustBefore.m_effectMode=='3'))
    {
        l_discountType=DISCNT_TYPE_ADJUST_ASACCT_RECEIVE; //4：帐前调整体现为优惠（补收）
    }
    else if ((r_adjustBefore.m_effectMode=='1') || (r_adjustBefore.m_effectMode=='2'))
    {
        l_discountType=DISCNT_TYPE_ADJUST_ASACCT_DERATE; //3：帐前调整体现为优惠（减免）
    }
    else if (r_adjustBefore.m_effectMode == '4')
    {
        if (r_effectFee > 0)
        {
            l_discountType=DISCNT_TYPE_ADJUST_ASACCT_DERATE;
        }
        else
        {
            l_discountType=DISCNT_TYPE_ADJUST_ASACCT_RECEIVE;
        }
    }
    else if ((r_adjustBefore.m_effectMode=='5') || (r_adjustBefore.m_effectMode=='7'))
    {
        l_discountType=DISCNT_TYPE_ADJUST_RECEIVE; //帐前调整（补收）
    }
    else if ((r_adjustBefore.m_effectMode=='6') || (r_adjustBefore.m_effectMode=='8'))
    {
        l_discountType=DISCNT_TYPE_ADJUST_DERATE; //帐前调整（减免）
    }
    else
    {
        if (r_effectFee > 0)
        {
            l_discountType=DISCNT_TYPE_ADJUST_DERATE;
        }
        else
        {
            l_discountType=DISCNT_TYPE_ADJUST_RECEIVE;
        }
    }

	updateEffectItemFee(r_discount.m_effectItem, 
	                         r_nfee, 
	                         r_effectFee, 
	                         r_adjustBefore.m_actMode, 
	                         l_discountType, 
	                         r_discount,r_adjustBefore.m_id,
	                         r_adjustBefore.m_idType,
	                         r_adjustBefore.m_adjustInsId);
	                      
}

//##ModelId=48F94C3F0377
void AdjustBeforeCalc::updateAddupInfo(const long &r_adjustInsId,const int &r_addupItem, const long &r_beginTime,const long &r_endTime,const long &r_effectFee)
{	
	UserSumBill2 l_sumBill;
  	l_sumBill.m_userId=m_id;     
	l_sumBill.m_tpObjId=r_adjustInsId;
	l_sumBill.m_cycleId=m_cycParam.m_cycleId;     
	l_sumBill.m_beginTime=r_beginTime;   
	l_sumBill.m_endTime=r_endTime;     
	l_sumBill.m_billId=r_addupItem;
	l_sumBill.m_value=labs(r_effectFee)*m_pComBill->m_divisor; //modify by xuf 2009-9-29 20:16:48     
	l_sumBill.m_type=0;;                                 
	l_sumBill.m_groupUserId=0; 
	
	m_pRateInfo->updateBillTotal(m_pRateInfo->m_userSumBill2s_new,l_sumBill);
	
}


/**
 * getReferItemFee:根据参考帐目获取明细帐目费用
 * @param  r_referItemCode     参考帐目编码.
 * @param  r_factFee           作用帐目上实际费用.
 * @return  true 成功 false 失败
*/
void AdjustBeforeCalc::getReferItemFee(int r_referItemCode,long &r_factFee)
{
	//'0' 组合帐目 '1'明细帐目 其他，未知
	m_pRateInfo->m_detailBillMgr.getFactFee(r_referItemCode,r_factFee);

}

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
void AdjustBeforeCalc::updateEffectItemFee(const int &r_effectItemCode,
    	                         const long &r_nsumfee, 
    	                         long &r_effectFee,
    	                         const char &r_discountMode,
    	                         const char &r_discountType, 
    	                         const Discount &r_discount,
    	                         const T_SERVER_ID &r_id,
    	                         const char  r_idType,
    	                         const long      r_adjustInsId )
{
#ifdef _NEGATIVEBILL_
 	const char IS_SPPORT_NEGATIVE_BILL  = NEGATIVE_BILL_YES;
#else
	const char IS_SPPORT_NEGATIVE_BILL  = NEGATIVE_BILL_NO;
#endif
	   //帐前调账信息初始化 --dingr added
      m_pRateInfo->m_detailBillMgr.m_pRateInfo=m_pRateInfo;
      m_pRateInfo->m_detailBillMgr.m_discntBill.m_adjustInsId=r_adjustInsId;
      m_pRateInfo->m_detailBillMgr.m_discntBill.m_id=r_id;
      m_pRateInfo->m_detailBillMgr.m_discntBill.m_idType=(ID_TYPE)r_idType;
	//TODO nsumfee 重复取了一次，需要优化
	switch(r_discountMode)
	{
		//'0'-按金额平摊
	 case   DISPATCH_USERAVE_ITEMAVE : //'0' :	
			m_pRateInfo->m_detailBillMgr.update_userAveItemAve(r_effectItemCode,
                                              r_effectFee,
                                              r_discount.m_ifShowNegative, 
                                              r_discountType, 
                                              IS_SPPORT_NEGATIVE_BILL);	
                                             	
	 	   	break;
	 //'1'-按金额平摊+帐目优先级	   	
	 case   DISPATCH_USERAVE_ITEMPRIOR : //'1' :
	 	
			m_pRateInfo->m_detailBillMgr.update_userAveItemPrior(r_effectItemCode,
                                              r_effectFee,
                                              r_discount.m_ifShowNegative, 
                                              r_discountType, 
                                              IS_SPPORT_NEGATIVE_BILL);

	 	    break;
	 //'2'-按用户优先级，金额平摊	    
	 case  DISPATCH_USERPRIOR_ITEMAVE : // '2' :
	
			m_pRateInfo->m_detailBillMgr.update_userPriorItemAve(r_effectItemCode,
                                              r_effectFee,
                                              r_discount.m_ifShowNegative, 
                                              r_discountType, 
                                              IS_SPPORT_NEGATIVE_BILL);                                               
	 			break;
	 //'3'-按用户+帐目优先级			
	 case  DISPATCH_USERPRIOR_ITEMPRIOR : //'3' :	
	 			m_pRateInfo->m_detailBillMgr.update_userPriorItemPrior(r_effectItemCode,
                                              r_effectFee,
                                              r_discount.m_ifShowNegative, 
                                              r_discountType, 
                                              IS_SPPORT_NEGATIVE_BILL);
                                             
	 	    break;
	 //'5'-作用在user_id=0的用户上    
	 case  DISPATCH_ZEROUSER : //'5' :	
	 			m_pRateInfo->m_detailBillMgr.update_setUser(ZERO_SERVER_ID,
	 			                                      r_effectItemCode,
                                              r_effectFee,
                                              r_discount.m_ifShowNegative, 
                                              r_discountType, 
                                              NEGATIVE_BILL_YES);
                                             
	 	    break;
	 default : //按用户优先级最高的作用	 	
				m_pRateInfo->m_detailBillMgr.update_fisrtUser(r_effectItemCode,
                                              r_effectFee,
                                              r_discount.m_ifShowNegative, 
                                              r_discountType, 
                                              IS_SPPORT_NEGATIVE_BILL);	 					 
     
  }
  
}

//
