#include "AcctGiftDeal.h"


AcctGiftDeal::AcctGiftDeal()
{
	m_pAccountBillVector = NULL;
	m_pUserOtherInfoVector  = NULL;
		
}


AcctGiftDeal::~AcctGiftDeal()
{

}

void 	 AcctGiftDeal::init(const T_SERVER_ID  &r_userId, 
							vector<UserOtherInfo>  *r_pUserOtherInfoVector,
	            vector<AccountBill> 	 *r_pAccountBillVector,
	            PMManager       			 *r_pmManager,
	            RateGlobalInfo  			 *r_pRateInfo)   
{
	m_userId                 = r_userId;
	m_pUserOtherInfoVector   = r_pUserOtherInfoVector;
	m_pAccountBillVector     = r_pAccountBillVector;
	m_pmManager              = r_pmManager;
	m_pRateInfo              = r_pRateInfo;
	m_pUserSumBill2s         = &m_pRateInfo->m_userSumBill2s_new;
	m_cycleId								 = *m_pRateInfo->m_pSysCycle;
}

void AcctGiftDeal::execute()
{
	String<4> l_giftType;
	int  l_giftItem=0;   //赠送帐目号
	int	 l_recycleItem=0; //补费帐目号
	int  l_discntItem=0; //优惠帐目号
	long  l_sumDiscntFee=0; //累计优惠金额数
	long l_insId=0; //资费实例id
	long l_discntFee=0;  //优惠金额数(l_gifttype=="10")
	long l_giftSumFee=0;  //赠送金额总额度
	long l_recycleFee=0;  //补费金额
	long l_giftFee=0;   //赠送帐目号上消费总金额数
	long l_atLeastFee=0; //最低消费额度
	long l_payValue=0;  //赠送金额数
	T_DATE_TIME l_startDate;
	T_DATE_TIME l_endDate;
	long minFee=0;
	
	
	//取所有费用
	long l_allFee=0;
	getAllFee(l_allFee);
	
	
	//循环处理赠款回收记录
	vector<UserOtherInfo>::iterator t_it;
	for(t_it=m_pUserOtherInfoVector->begin();
	    t_it!=m_pUserOtherInfoVector->end();t_it++)
	{	 
		l_giftType   =  t_it->m_otherInfoType;
		l_atLeastFee =  t_it->m_attrValue1.value(); //20
		l_payValue   =  t_it->m_attrValue2.value(); //赠送话费30
		l_giftItem   =  t_it->m_attrValue3.value();
		l_giftSumFee =  t_it->m_attrValue4.value();
		l_insId      =  t_it->m_attrValue5.value();
		l_recycleItem=  t_it->m_attrValue6.value(); //补费帐目
		l_discntItem =  t_it->m_attrValue7.value();  //优惠帐目
		l_startDate  =  t_it->m_startDate;
		l_endDate    =  t_it->m_endDate;
		
		 
	 	getGiftItemFee(l_giftItem,l_giftFee);        //取赠送帐目号上消费总金额数

	 	
	 	//普通赠款
	 	if (l_giftType=="8")
	 	{
	 		if(l_allFee < l_atLeastFee )   //所有消费金额小于最低消费
	 			l_recycleFee = l_payValue;
	 		else if( l_allFee >= l_atLeastFee) //所有消费金额大于等于最低消费
	 		{
	 			if( l_giftFee <= l_payValue )    //优惠帐目项上消费金额小于等于赠送金额
	 				l_recycleFee = l_payValue - l_giftFee; //补费金额
	 			else
	 				l_recycleFee = 0;	
	 		}
	 		
	 	}
	 	//超某赠款
	 	else if(l_giftType=="9")
	 	{
	 		if(l_allFee < l_atLeastFee )
	 			l_recycleFee = l_payValue;
	 		else if(l_allFee >= l_atLeastFee )
	 		{
	 			minFee = l_allFee - l_atLeastFee<=l_giftFee?(l_allFee-l_atLeastFee):l_giftFee; //取较小的优惠金额
	 			if(minFee<l_payValue)
	 			l_recycleFee=l_payValue-l_giftFee;
	 			else 
	 			l_recycleFee=0;
	 		}
	 	}	
	 	
	 	//按比例优惠
	 	else if(l_giftType=="10")
	 	{
	 	 getAddupInfo(l_insId,
							    l_startDate.value(), 
									l_discntItem, 
									l_sumDiscntFee);  //取优惠累计量
	 		
	 		if(l_allFee < l_atLeastFee )
	 			return ;
	 		else if(l_allFee>=l_atLeastFee)
	 		{
	 			l_discntFee=l_payValue*l_giftFee/100;
	 			if(l_giftSumFee<=l_discntFee+l_sumDiscntFee)
	 				l_discntFee=l_giftSumFee-l_sumDiscntFee;	
	 		}
	 			
	 		 updateAddupInfo(l_insId, 
	                  l_startDate.value(),
    							  l_discntItem,
    							  l_endDate.value(),
    								 l_discntFee+l_sumDiscntFee);		
	 	}
	  else 
	  	continue;
	  	

		updateSubIdDetailitemFee(l_recycleItem, l_recycleFee);

			
	}//for
	
}


bool AcctGiftDeal::getGiftItemFee(const int& r_itemCode, long& r_fee)
{
	  r_fee    = 0;
    getDetailItemFromGiftItem(r_itemCode);

    bool      ifExist = false;
    int       i = 0;
    vector<AccountBill>::iterator iter;
   // memset(m_existDetailItem, 0, sizeof(m_existDetailItem));
    
    for (iter=(*m_pAccountBillVector).begin();
         iter!=(*m_pAccountBillVector).end(); iter++)
    {
       // i++;
        if ((m_detailItem[iter->m_billId]==1) && (iter->m_discntValue!=0))
        {
           // m_existDetailItem[iter->m_detailItemCode] = i;
            
            r_fee += iter->m_discntValue;
            ifExist = true;
        }   
        
        
    }
    
    return ifExist;
	
}

bool AcctGiftDeal::getAddupInfo(const long& r_insId, const long &r_startDate, 
										  const int &r_discntItem, long &r_discntValue)
{
  UserSumBill2  l_sumBill2;
  const UserSumBill2 *l_pLastSumBill2=NULL;
  
  l_sumBill2.m_userId       = m_userId;     
  l_sumBill2.m_tpObjId      = r_insId;    
  l_sumBill2.m_beginTime    = r_startDate;  
  l_sumBill2.m_cycleId      = *(m_pRateInfo->m_pSysCycle);     
  l_sumBill2.m_billId       = r_discntItem;
  	
  if (!m_pRateInfo->getLastSumBill2(m_pRateInfo->m_userSumBill2s_new,
                                    l_sumBill2,
                                    l_pLastSumBill2)) 	
  {
  	r_discntValue = 0;
    return false;	
  }
  
  r_discntValue=l_pLastSumBill2->m_value;
  
	return true;
}

void AcctGiftDeal::getAllFee(long& r_fee)
{
		r_fee = 0;	
	  vector<AccountBill>::iterator iter;
    
    for (iter=(*m_pAccountBillVector).begin();
         iter!=(*m_pAccountBillVector).end(); iter++)
    {
			r_fee += iter->m_discntValue;  
    }
    
    
}

void AcctGiftDeal::getDetailItemFromGiftItem(const int& r_ItemCode)
{
    if(!m_pmManager->getDetailItemFromPayItem(r_ItemCode, m_detailItemVector))
    {
      DEBUG_PRINT("根据赠款帐目获取明细帐目错误!\n");
    }
    
    vector<DetailItem>::iterator iter;
    memset(m_detailItem, 0, sizeof(m_detailItem)*MAX_ITEM);    
    
    //帐目本身为明细帐目
    if (m_detailItemVector.empty()) 
    {
    	m_detailItem[r_ItemCode] = 1;
    	return;
    }
    
    for (iter=m_detailItemVector.begin(); 
         iter!=m_detailItemVector.end(); iter++)
    {
        m_detailItem[iter->m_id] = 1;
    }
    
}

void AcctGiftDeal::updateAddupInfo(const long &r_insId, const long &r_startDate,
    										 const int &r_discntItem,const long &r_endDate,
    										 const long &r_effectFee)
{	
	UserSumBill2 l_sumBill;
  l_sumBill.m_userId=m_userId;     
	l_sumBill.m_tpObjId=r_insId;
	l_sumBill.m_cycleId=*(m_pRateInfo->m_pSysCycle);     
	l_sumBill.m_beginTime=r_startDate;   
	l_sumBill.m_endTime=r_endDate;     
	l_sumBill.m_billId=r_discntItem;
	l_sumBill.m_value=r_effectFee;      
	l_sumBill.m_type=0;                                 
	l_sumBill.m_groupUserId=0; 
	
	m_pRateInfo->updateBillTotal(m_pRateInfo->m_userSumBill2s_new,l_sumBill);
	
}  										 
    										 
void AcctGiftDeal::updateSubIdDetailitemFee(const int &r_detailItemCode, 
                                           const long &r_effectFee)
{
  AccountBill t_accountBill;
  t_accountBill.m_billId= r_detailItemCode;
  
  vector<AccountBill>::iterator t_feeIter;
	t_feeIter=lower_bound(m_pAccountBillVector->begin(),
                        m_pAccountBillVector->end(),
                        t_accountBill);
    //更新账单费用
    if (t_feeIter!=m_pAccountBillVector->end() &&
        (*t_feeIter).m_billId==r_detailItemCode )
    {
        (*t_feeIter).m_discntValue = r_effectFee;
        (*t_feeIter).m_a_discnt = -r_effectFee;
        (*t_feeIter).m_b_discnt = 0;
        
        return;
    }

    //如果未找到billId
    t_accountBill.m_value = 0;
    t_accountBill.m_discntValue = r_effectFee;    
    t_accountBill.m_a_discnt = -r_effectFee;
    t_accountBill.m_b_discnt = 0;
    
    m_pAccountBillVector->push_back(t_accountBill);
    partial_sort(m_pAccountBillVector->begin(),
                 m_pAccountBillVector->end(),
                 m_pAccountBillVector->end());
}