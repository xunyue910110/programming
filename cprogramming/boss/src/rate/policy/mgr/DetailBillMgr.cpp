#include "DetailBillMgr.h"
#include <algorithm>
#include "RateGlobalInfo.h"

extern RateGlobalInfo g_globalInfo;
RateGlobalInfo* DetailBillMgr::m_pRateInfo = &g_globalInfo;

DetailItemFee& DetailItemFee::operator=(const long& right)
{
	m_ofee           = right;
	m_nfee           = right;
	m_afee           = right;
	m_bfee           = right;
	m_cfee           = right;
	m_dfee           = right;
	m_gfee           = right;  //add for acctdiscnt 2009-9-11 2:42:09
	m_ufee           = right; //用户级调帐费用 add for acctadjust 2009-9-28 14:54:43
	return *this;
}

DetailItemFee& DetailItemFee::operator=(const DetailItemFee& right)
{
	m_detailItemCode = right.m_detailItemCode;
	m_ofee           = right.m_ofee;
	m_nfee           = right.m_nfee;
	m_afee           = right.m_afee;
	m_bfee           = right.m_bfee;
	m_cfee           = right.m_cfee;
	m_dfee           = right.m_dfee;
	m_gfee           = right.m_gfee; //add for acctdiscnt 2009-9-11 2:42:30
	m_ufee           = right.m_ufee; //用户级调帐费用 add for acctadjust 2009-9-28 14:54:43
	return *this;
}

DetailItemFee& DetailItemFee::operator +=(const DetailItemFee &right)
{
	m_ofee           += right.m_ofee;
	m_nfee           += right.m_nfee;
	m_afee           += right.m_afee;
	m_bfee           += right.m_bfee;
	m_cfee           += right.m_cfee;
	m_dfee           += right.m_dfee;
	m_gfee           += right.m_gfee; //add for acctdiscnt 2009-9-11 2:42:53
	m_ufee           += right.m_ufee; //用户级调帐费用 add for acctadjust 2009-9-28 14:54:43
	return *this;
}

DetailItemFee& DetailItemFee::operator -=(const DetailItemFee &right)
{
	m_ofee           -= right.m_ofee;
	m_nfee           -= right.m_nfee;
	m_afee           -= right.m_afee;
	m_bfee           -= right.m_bfee;
	m_cfee           -= right.m_cfee; 
	m_dfee           -= right.m_dfee;
	m_gfee           -= right.m_gfee; //add for acctdiscnt 2009-9-11 2:43:17
	m_ufee           -= right.m_ufee;//用户级调帐费用 add for acctadjust 2009-9-28 14:54:43
	return *this;
}    

bool operator==(const ItemBill &left, const ItemBill &right)
{
    return (left.m_detailItem.m_id==right.m_detailItem.m_id &&
            left.m_detailItem.m_orderNo==right.m_detailItem.m_orderNo);
}

bool operator<(const ItemBill &left, const ItemBill &right)
{
    if (left.m_detailItem.m_orderNo==right.m_detailItem.m_orderNo)
	    return (left.m_detailItem.m_id<right.m_detailItem.m_id);
    else
        return (left.m_detailItem.m_orderNo<right.m_detailItem.m_orderNo);
}

const DetailBill& DetailBill::operator=(const DetailBill& right)
{
	m_idType           = right.m_idType;
	m_id               = right.m_id;
	m_detailBillVector = right.m_detailBillVector;
	return *this;  
}

DetailBill& DetailBill::operator +=(const DetailBill &right)
{
	if (m_id==right.m_id && m_idType==right.m_idType)
	{
		for(vector<UserDetailBill>::const_iterator t_it=right.m_detailBillVector.begin();
			  t_it!=right.m_detailBillVector.end();++t_it)
		{
			  vector<UserDetailBill>::iterator t_iter;
				for(t_iter=m_detailBillVector.begin();
			  t_iter!=m_detailBillVector.end();++t_iter)
			  {
			    if (t_it->m_subId==t_iter->m_subId)
			    	break;
			  }	  			  
			  //not find
			  if(t_iter==m_detailBillVector.end())  	
					m_detailBillVector.push_back(*t_it);
		}
	}
	return *this;
}

bool operator==(const DetailBill &left,const DetailBill& right)
{
	return (left.m_id==right.m_id &&
	        left.m_idType==right.m_idType);
}

bool operator<(const DetailBill &left,const DetailBill& right)
{
		if(left.m_idType == right.m_idType)
			return (left.m_id < right.m_id);
		
		return (left.m_idType < right.m_idType);
}

bool operator==(const UserDetailBill &left,const UserDetailBill& right)
{
		return (left.m_subId==right.m_subId &&
	          left.m_priority==right.m_priority);
} 

//获取账目实际费用
void DetailBillMgr::getFactFee(int &itemCode,char &itemType,long &fee)
{
    fee = 0;
    if(itemType=='0') //组合帐目
    {        
    	if(itemCode!=-1)
    	{
        	IntMap& detailItemMap = m_compositItemMgr->find(itemCode);
        	getFactFeeComposit(detailItemMap,fee);
        }
        else
        	getFactFeeAll(fee);
    }
    else //if(itemType=='1')//明细帐目
    {
        getFactFeeDetail(itemCode,fee);
    }
        
    return;
}

//获取账目当前费用(上一个优惠作用后的费用)
void DetailBillMgr::getCurFee(int &itemCode,char &itemType,long &fee)
{
    fee = 0;
    if(itemType=='0') //组合帐目
    {       
    	if(itemCode!=-1)
    	{
        	IntMap& detailItemMap = m_compositItemMgr->find(itemCode);
        	getCurFeeComposit(detailItemMap,fee);
        }
        else 
            getCurFeeAll(fee);
    }
    else //if(itemType=='1')//明细帐目
    {
        getCurFeeDetail(itemCode,fee);
    }
    
    return;
}

//获取账目原始费用(优惠作用前的费用)
void DetailBillMgr::getOriginFee(int &itemCode,char &itemType,long &fee)
{
    fee = 0;
    if(itemType=='0') //组合帐目
    {
    	if(itemCode!=-1)
    	{
        	IntMap& detailItemMap = m_compositItemMgr->find(itemCode);
        	getOriginFeeComposit(detailItemMap,fee);
        }
        else 
            getOriginFeeAll(fee);
    }
    else //if(itemType=='1')//明细帐目
    {
        getOriginFeeDetail(itemCode,fee);
    }
    
    return;
}

//获取账目实际费用，但不包括调整补收的费用
void DetailBillMgr::getFactFeeButBfee(int &itemCode,char &itemType,long &fee)
{
    fee = 0;
    if(itemType=='0') //组合帐目
    {
    	if(itemCode!=-1)
    	{
        	IntMap& detailItemMap = m_compositItemMgr->find(itemCode);
        	getFactFeeButBfeeComposit(detailItemMap,fee);
        }
        else 
            getFactFeeButBfeeAll(fee);        
    }
    else //if(itemType=='1')//明细帐目
    {
        getFactFeeButBfeeDetail(itemCode,fee);
    }
    
    return;
}


//private
void DetailBillMgr::getFactFeeComposit(IntMap &detailItemMap,long &fee)
{
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {            
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        for (m_constFeeIter  = (*m_billIter).m_feeVector.begin();
             m_constFeeIter!=(*m_billIter).m_feeVector.end(); 
              ++m_constFeeIter)
        {
            if (detailItemMap.find((*m_constFeeIter).m_detailItemCode)!=
                detailItemMap.end())
            {
                fee += (*m_constFeeIter).m_nfee;
            }
        }
    }    
    return;
}

void DetailBillMgr::getCurFeeComposit(IntMap &detailItemMap,long &fee)
{
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {            
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        for (m_constFeeIter  = (*m_billIter).m_feeVector.begin();
             m_constFeeIter!=(*m_billIter).m_feeVector.end(); 
              ++m_constFeeIter)
        {
            if (detailItemMap.find((*m_constFeeIter).m_detailItemCode)!=
                detailItemMap.end())
            {
                fee += (*m_constFeeIter).m_cfee;
            }
        }
    }    
    return;
}

void DetailBillMgr::getOriginFeeComposit(IntMap &detailItemMap,long &fee)
{
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {            
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        for (m_constFeeIter  = (*m_billIter).m_feeVector.begin();
             m_constFeeIter!=(*m_billIter).m_feeVector.end(); 
              ++m_constFeeIter)
        {
            if (detailItemMap.find((*m_constFeeIter).m_detailItemCode)!=
                detailItemMap.end())
            {
                fee += (*m_constFeeIter).m_ofee;
            }
        }
    }    
    return;
}

void DetailBillMgr::getFactFeeButBfeeComposit(IntMap &detailItemMap,long &fee)    
{
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {            
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        for (m_constFeeIter  = (*m_billIter).m_feeVector.begin();
             m_constFeeIter!=(*m_billIter).m_feeVector.end(); 
              ++m_constFeeIter)
        {
            if (detailItemMap.find((*m_constFeeIter).m_detailItemCode)!=
                detailItemMap.end())
            {
                fee += (*m_constFeeIter).m_nfee;
                if( (*m_constFeeIter).m_bfee>0)
                    fee += (*m_constFeeIter).m_bfee;
            }
        }
    }    
    return;
}

void DetailBillMgr::getFactFeeDetail(int &itemCode,long &fee)
{
    m_detailItemFee.m_detailItemCode = itemCode;
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {       
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        m_constFeeIter=lower_bound((*m_billIter).m_feeVector.begin(),
                                     (*m_billIter).m_feeVector.end(),
                                     m_detailItemFee);
        
        if(m_constFeeIter!=(*m_billIter).m_feeVector.end() &&
           (*m_constFeeIter).m_detailItemCode==itemCode )
        {
            fee += (*m_constFeeIter).m_nfee;            
        }
    }        
    return;
}

void DetailBillMgr::getCurFeeDetail(int &itemCode,long &fee)
{
    m_detailItemFee.m_detailItemCode = itemCode;
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {       
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        m_constFeeIter=lower_bound((*m_billIter).m_feeVector.begin(),
                                   (*m_billIter).m_feeVector.end(),
                                   m_detailItemFee);
        
        if(m_constFeeIter!=(*m_billIter).m_feeVector.end() &&
           (*m_constFeeIter).m_detailItemCode==itemCode )
        {
            fee += (*m_constFeeIter).m_cfee;            
        }
    }        
    return;
}

void DetailBillMgr::getOriginFeeDetail(int &itemCode,long &fee)
{
    m_detailItemFee.m_detailItemCode = itemCode;
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {       
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        m_constFeeIter=lower_bound((*m_billIter).m_feeVector.begin(), 
                              (*m_billIter).m_feeVector.end(),
                              m_detailItemFee);
        
        if(m_constFeeIter!=(*m_billIter).m_feeVector.end() &&
           (*m_constFeeIter).m_detailItemCode==itemCode )
        {
            fee += (*m_constFeeIter).m_ofee;            
        }
    }        
    return;
}

void DetailBillMgr::getFactFeeButBfeeDetail(int &itemCode,long &fee)
{
    m_detailItemFee.m_detailItemCode = itemCode;
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {       
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        m_constFeeIter=lower_bound((*m_billIter).m_feeVector.begin(),
                              (*m_billIter).m_feeVector.end(),
                              m_detailItemFee);
        
        if(m_constFeeIter!=(*m_billIter).m_feeVector.end() &&
           (*m_constFeeIter).m_detailItemCode==itemCode )
        {
            fee += (*m_constFeeIter).m_nfee;
            if( (*m_constFeeIter).m_bfee>0 )
                fee += (*m_constFeeIter).m_bfee;        
        }
    }            
    return;
} 

void DetailBillMgr::getFactFeeAll(long &fee)
{
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {            
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        for (m_constFeeIter  = (*m_billIter).m_feeVector.begin();
             m_constFeeIter!=(*m_billIter).m_feeVector.end(); 
              ++m_constFeeIter)
        {
            fee += (*m_constFeeIter).m_nfee;
        }
    }    
    return;
}

void DetailBillMgr::getCurFeeAll(long &fee)
{
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {            
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        for (m_constFeeIter  = (*m_billIter).m_feeVector.begin();
             m_constFeeIter!=(*m_billIter).m_feeVector.end(); 
              ++m_constFeeIter)
        {
            fee += (*m_constFeeIter).m_cfee;
        }
    }    
    return;
}

void DetailBillMgr::getOriginFeeAll(long &fee)
{
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {            
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        for (m_constFeeIter  = (*m_billIter).m_feeVector.begin();
             m_constFeeIter!=(*m_billIter).m_feeVector.end(); 
              ++m_constFeeIter)
        {
            fee += (*m_constFeeIter).m_ofee;
        }
    }    
    return;
}

void DetailBillMgr::getFactFeeButBfeeAll(long &fee)    
{
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {            
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        for (m_constFeeIter  = (*m_billIter).m_feeVector.begin();
             m_constFeeIter!=(*m_billIter).m_feeVector.end(); 
              ++m_constFeeIter)
        {
                fee += (*m_constFeeIter).m_nfee;
                if( (*m_constFeeIter).m_bfee>0)
                    fee += (*m_constFeeIter).m_bfee;
        }
    }    
    return;
}        

void DetailBillMgr::updateCurFee()
{
    for (m_billIter=(*m_detailBill).m_detailBillVector.begin();
         m_billIter!=(*m_detailBill).m_detailBillVector.end(); 
         ++m_billIter)
    {            
        if(!m_userIds.empty() && 
		   m_userIds.find(m_billIter->m_subId) == m_userIds.end())
		{continue;}
        for (m_feeIter=(*m_billIter).m_feeVector.begin();
             m_feeIter!=(*m_billIter).m_feeVector.end(); 
             ++m_feeIter)
        {
            (*m_feeIter).m_cfee = (*m_feeIter).m_nfee;
        }
    }
    return;
}



//更新账目费用
    
//param:
//itemCode       : 账目
//effectFee      : 优惠的费用
//discountType   : 优惠类型
//ifNegativeBill : 是否可以优惠为负值
//ifNotShowNegative:优惠值是否体现在账户账单中
//detailItemMap  : 明细账目集合

//按“用户平摊+账目平摊”方式处理优惠值
long DetailBillMgr::update_userAveItemAve(const int &itemCode,                                        
                                          long &effectFee,
                                          const char &ifNotShowNegative, 
                                          const char &discountType, 
                                          const char &ifNegativeBill)
{	
    char ifShowMinus;    
    long discountFee = 0;
    long sumDiscountFee = 0;
    long integerfee = 0;   
    long sumFee;
    long leftFee = 0;        
    
	//Modified by xuzy at 2011.2.7
	//在账户优惠后，帐户欠费不允许为负值，最多优惠到零
	if(effectFee > 0 && m_detailBill->m_idType !=USER_ID_TYPE)
	{
		sumFee=0;
		getFactFeeAll(sumFee);
		if(effectFee > sumFee)
			effectFee = sumFee;
	}
	
    IntMap &detailItemMap = m_compositItemMgr->find(itemCode);
    getFactFee(itemCode, detailItemMap, sumFee);
	
	if (sumFee==0 && effectFee>0 && ifNegativeBill!=NEGATIVE_BILL_YES)
        return effectFee;
    
    //减免,且减免值大于总费用,且不能优惠为负账单的情况，才可能有剩余值
	if (effectFee>sumFee && ifNegativeBill!=NEGATIVE_BILL_YES)
	{
		leftFee = effectFee-sumFee;
        effectFee = sumFee;        
    }    
        
    if (effectFee<0 && ifNotShowNegative=='1')
    {
        ifShowMinus = '1';
    }
    else if (effectFee>0 && ifNotShowNegative=='2')
    {
        ifShowMinus = '1';
    }    
    else
    {
        ifShowMinus = '0';
    }
    
    if (sumFee==0 )
    {
        int idNum = 0;//用户数     
		if(!m_userIds.empty())
			{idNum = m_userIds.size();}
		else{idNum = m_detailBill->m_detailBillVector.size();}
        discountFee = 1.0 * effectFee / idNum - 0.5;
        
        for (m_billIter=m_detailBill->m_detailBillVector.begin();
             m_billIter!=m_detailBill->m_detailBillVector.end(); ++m_billIter)
        {            
            if(!m_userIds.empty() && 
               m_userIds.find(m_billIter->m_subId) == m_userIds.end())
            {continue;}
            getItemFactFee(*m_billIter,itemCode,detailItemMap);
            
            if( (*m_billIter).m_detailItemNum==0 )
            {
            	updateSubIdFeeItemFirst(*m_billIter,discountFee,
                                        ifShowMinus,discountType,
                                        detailItemMap);
            }
            else
            {
            	updateSubIdFeeItemAve(*m_billIter,discountFee,
                                        ifShowMinus,discountType,
                                        ifNegativeBill,detailItemMap);
            }
            
            sumDiscountFee += discountFee;
        }
    }
    else
    {
        float adjustValue;
        if (effectFee>0)
            adjustValue = 0.5;
        else
            adjustValue = -0.5;            
            
        for (m_billIter=m_detailBill->m_detailBillVector.begin();
             m_billIter!=m_detailBill->m_detailBillVector.end(); ++m_billIter)
        {
            if(!m_userIds.empty() && 
               m_userIds.find(m_billIter->m_subId) == m_userIds.end())
            {continue;} 
        	getItemFactFee(*m_billIter,itemCode,detailItemMap);
            discountFee = 1.0 * effectFee * 
                          (*m_billIter).m_sumFee / sumFee + adjustValue;
                          
            if( (*m_billIter).m_detailItemNum==0 && discountFee<0)
            {
            	updateSubIdFeeItemFirst(*m_billIter,discountFee,
                                        ifShowMinus,discountType,
                                        detailItemMap);
            }
            else
            {
            	updateSubIdFeeItemAve(*m_billIter,discountFee,
                                      ifShowMinus,discountType,
                                      ifNegativeBill,detailItemMap);
            }
                                     
            sumDiscountFee += discountFee;
        }        
    }
    
    integerfee = effectFee-sumDiscountFee;
    // 补全归整
    if (integerfee!=0)
    {            
        for (m_billIter=m_detailBill->m_detailBillVector.begin();
             m_billIter!=m_detailBill->m_detailBillVector.end(); ++m_billIter)           
        {                
            if(!m_userIds.empty() && 
               m_userIds.find(m_billIter->m_subId) == m_userIds.end())
            {continue;} 
        	getItemFactFee(*m_billIter,itemCode,detailItemMap);
            if ( (*m_billIter).m_detailItemNum>0 )
            {
                if ( (*m_billIter).m_sumFee>=integerfee)
                {
                    discountFee=integerfee;
                    integerfee = 0;
                }
                else
                {        
                	if((*m_billIter).m_sumFee>=0)
                	{
                		 if(ifNegativeBill!=NEGATIVE_BILL_YES)//不支持负账单
                		 {
                    	discountFee = (*m_billIter).m_sumFee;
                    	integerfee -= (*m_billIter).m_sumFee;
                     }
                     else //支持负账单，减免到单独帐目
                     {
                     	 discountFee =  integerfee;
                     	 integerfee = 0;
                     }
                    }
                    else //如果可以优惠为负值,则可能出现m_sumFee<0的情况
                    {
                    	discountFee=integerfee;
                    	integerfee = 0;
                    }
                }
    
                updateSubIdFeeItemAve(*m_billIter,discountFee,
                                      ifShowMinus,discountType,
                                      ifNegativeBill,detailItemMap);
            }
                        
            if (integerfee==0)
            {
                break;
            }
        }        
    }
    return leftFee;
}

//按“用户平摊+账目优先”方式处理优惠值
long DetailBillMgr::update_userAveItemPrior(const int &itemCode,
                                          long &effectFee,
                                          const char &ifNotShowNegative, 
                                          const char &discountType, 
                                          const char &ifNegativeBill)
{
    char ifShowMinus;    
    long discountFee = 0;
    long sumDiscountFee = 0;
    long integerfee = 0;        
    long sumFee;
    long leftFee = 0;

	//Modified by xuzy at 2011.2.7
	//在账户优惠后，帐户欠费不允许为负值，最多优惠到零
	if(effectFee > 0 && m_detailBill->m_idType !=USER_ID_TYPE)
	{
		sumFee=0;
		getFactFeeAll(sumFee);
		if(effectFee > sumFee)
			effectFee = sumFee;
	}
	
    IntMap &detailItemMap = m_compositItemMgr->find(itemCode);
    getFactFee(itemCode, detailItemMap, sumFee);    
	if (sumFee==0 && effectFee>0 && ifNegativeBill!=NEGATIVE_BILL_YES)
        return effectFee;
    
    //减免,且减免值大于总费用,且不能优惠为负账单的情况，才可能有剩余值
	if (effectFee>sumFee && ifNegativeBill!=NEGATIVE_BILL_YES)
	{
		leftFee = effectFee-sumFee;
        effectFee = sumFee;
    } 
        
    if (effectFee<0 && ifNotShowNegative=='1')
    {
        ifShowMinus = '1';
    }
    else if (effectFee>0 && ifNotShowNegative=='2')
    {
        ifShowMinus = '1';
    }    
    else
    {
        ifShowMinus = '0';
    }
    
    if (sumFee==0)
    {
        int idNum = 0;//用户数
        if(!m_userIds.empty())
			{idNum = m_userIds.size();}
		else{idNum = m_detailBill->m_detailBillVector.size();}

        discountFee = 1.0 * effectFee / idNum - 0.5;
        for (m_billIter=m_detailBill->m_detailBillVector.begin();
             m_billIter!=m_detailBill->m_detailBillVector.end(); ++m_billIter)
        {
            if(!m_userIds.empty() && 
               m_userIds.find(m_billIter->m_subId) == m_userIds.end())
            {continue;} 
            getItemFactFee(*m_billIter,itemCode,detailItemMap);
            if( (*m_billIter).m_detailItemNum==0 && discountFee<0)
            {
            	updateSubIdFeeItemFirst(*m_billIter,discountFee,
                                        ifShowMinus,discountType,
                                        detailItemMap);
            }
            else
            {
            	updateSubIdFeeItemPrior(*m_billIter,discountFee,
                                      ifShowMinus,discountType,
                                      ifNegativeBill,detailItemMap);
            }
                        
            sumDiscountFee += discountFee;
        }
    }
    else
    {
        float adjustValue;
        if (effectFee>0)
            adjustValue = 0.5;
        else
            adjustValue = -0.5;            
            
        for (m_billIter=m_detailBill->m_detailBillVector.begin();
             m_billIter!=m_detailBill->m_detailBillVector.end(); ++m_billIter)
        {
            if(!m_userIds.empty() && 
               m_userIds.find(m_billIter->m_subId) == m_userIds.end())
            {continue;} 
            getItemFactFee(*m_billIter,itemCode,detailItemMap);
            
            discountFee = 1.0 * effectFee * (*m_billIter).m_sumFee / sumFee + 
                          adjustValue;
            
            if( (*m_billIter).m_detailItemNum== 0 && discountFee<0)
            {
            	updateSubIdFeeItemFirst(*m_billIter,discountFee,
                                        ifShowMinus,discountType,
                                        detailItemMap);
            }
            else
            {
            	updateSubIdFeeItemPrior(*m_billIter,discountFee,
                                        ifShowMinus,discountType,
                                        ifNegativeBill,detailItemMap);
            }
            sumDiscountFee += discountFee;
        }        
    }
    
    integerfee = effectFee-sumDiscountFee;
    // 补全归整
    if (integerfee!=0)
    {            
        for (m_billIter=m_detailBill->m_detailBillVector.begin();
             m_billIter!=m_detailBill->m_detailBillVector.end(); ++m_billIter)           
        {                
            if(!m_userIds.empty() && 
               m_userIds.find(m_billIter->m_subId) == m_userIds.end())
            {continue;} 
            getItemFactFee(*m_billIter,itemCode,detailItemMap);
            if ( (*m_billIter).m_detailItemNum>0 )
            {
            	if ( (*m_billIter).m_sumFee>=integerfee )
            	{
            	    discountFee=integerfee;
            	    integerfee = 0;
            	}
            	else
            	{       
            		if((*m_billIter).m_sumFee>=0)
                	{
                    	
                    	if(ifNegativeBill!=NEGATIVE_BILL_YES)//不支持负账单
                		{
                    		discountFee = (*m_billIter).m_sumFee;
                    		integerfee -= (*m_billIter).m_sumFee;
                     	}
                     	else //支持负账单，减免到单独帐目
                     	{
                     	 	discountFee =  integerfee;
                     	 	integerfee = 0;
                     	}
                    	
                    }
                    else //如果可以优惠为负值,则可能出现m_sumFee<0的情况
                    {
                    	discountFee=integerfee;
                    	integerfee = 0;
                    }            	    
            	}
            	
            	updateSubIdFeeItemPrior(*m_billIter,discountFee,
            	                     ifShowMinus,discountType,
            	                     ifNegativeBill,detailItemMap);
            }

                        
            if (integerfee==0)
            {
                break;
            }
        }        
    }
    return leftFee;
}

//按“用户优先+账目平摊”方式处理优惠值
long DetailBillMgr::update_userPriorItemAve(const int &itemCode,
                                            long &effectFee,
                                            const char &ifNotShowNegative, 
                                            const char &discountType, 
                                            const char &ifNegativeBill)
{                                           
    char ifShowMinus;  
    long discountFee;
    long remainFee;
    long sumFee;
    long leftFee = 0;

	//Modified by xuzy at 2011.2.7
	//在账户优惠后，帐户欠费不允许为负值，最多优惠到零
	if(effectFee > 0 && m_detailBill->m_idType !=USER_ID_TYPE)
	{
		sumFee=0;
		getFactFeeAll(sumFee);
		if(effectFee > sumFee)
			effectFee = sumFee;
	}
	
    IntMap &detailItemMap = m_compositItemMgr->find(itemCode);
    getFactFee(itemCode, detailItemMap, sumFee);
	if (sumFee==0 && effectFee>0 && ifNegativeBill!=NEGATIVE_BILL_YES)
        return effectFee;
    
    //减免,且减免值大于总费用,且不能优惠为负账单的情况，才可能有剩余值
	if (effectFee>sumFee && ifNegativeBill!=NEGATIVE_BILL_YES)
	{
		leftFee = effectFee-sumFee;
        effectFee = sumFee;
    } 
        
    if