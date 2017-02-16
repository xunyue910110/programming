#include <algorithm>
#include "base/exceptions.h"
#include "AcctBillMgr.h"

DECLARE_EXCEPTION(AccountException, BusinessException);



bool  AcctBillMgr::initialize(RateGlobalInfo *r_pRateInfo)
{
	m_pRateInfo = r_pRateInfo;
	m_pComBill  = &r_pRateInfo->m_comBill;
	m_divisor   = m_pRateInfo->m_paramMgr->m_divisor;
	return true;	
}

/*
 * 根据用户（出帐用户和总量优惠群下的用户），取账户账单
 * @param r_acctIdSet        		账户信息
 * @param r_userId              用户ID
 * @param r_accountBills        账户账单
 * @return true 正常，false 异常
 */       
void AcctBillMgr::getAccountBill(const set<T_SERVER_ID> &r_acctIdSet, const set<T_USER_ID>  &r_userIdSet, vector<AccountBill> &r_accountBills)
{
	  //循环账户集，取账户账单
	  vector<AccountBill> l_accountBills;
    
    if(!m_pRateInfo->getUserBill(m_pComBill->m_cycleId,r_acctIdSet ,l_accountBills))
    	return; 
  	 
		for(vector<AccountBill>::iterator iterab=l_accountBills.begin();
        iterab != l_accountBills.end();++iterab)
    {
    	//出帐用户
      if(r_userIdSet.find(iterab->m_userId)!=r_userIdSet.end())
       	  r_accountBills.push_back(*iterab);  		
    } 
    //add by gaojf 2009-3-5 17:29
    ::sort(r_accountBills.begin(),r_accountBills.end());
}

/**
* updateAccountBill: 更新账户账单
* @param	detailBill	       明细账单
* @param	accountBills	     账户账单
* @param	r_idInfo	       	 用户信息
* @return	true false
*/
bool AcctBillMgr::updateAccountBill(DetailBill detailBill,vector<AccountBill>  &accountBills,const IdInfo &r_idInfo)
{
	IdInfo l_idInfo=r_idInfo;
	T_AREA_CODE  l_areaCode=l_idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE].c_str();
	
	for(vector<UserDetailBill>::iterator iterua=detailBill.m_detailBillVector.begin();
		 iterua!=detailBill.m_detailBillVector.end();iterua++)
	{
		if(!((iterua->m_subId==l_idInfo.m_idAttr[IdInfo::ID_ATTR_ID]&&
			 l_idInfo.m_idAttr[IdInfo::ID_TYPE_CODE][0]==(char)USER_ID_TYPE)||
			 m_pComBill->ifNotProvGroup(iterua->m_subId)
			 ))
 		{	 	
 				if (iterua->m_subId!=ZERO_SERVER_ID)
 				{
 					if(m_pComBill->getUserInfo(iterua->m_subId,l_idInfo))
				 	  l_areaCode  = l_idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE];
 				}else	//0用户取账户的信息
 				{		
					if(m_pComBill->getAcctInfo(detailBill.m_id,l_idInfo))
					{
				 		l_areaCode  = l_idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE];
				 		l_idInfo.m_idAttr[IdInfo::ID_TYPE_CODE] =(char)USER_ID_TYPE;	
				 	}
				}
		}
		
		for(vector<DetailItemFee>::iterator itf=iterua->m_feeVector.begin();
			itf!=iterua->m_feeVector.end();itf++)
		{
			    AccountBill  accountbill;
			    accountbill.m_accountId         = detailBill.m_id;
    			accountbill.m_userId            = iterua->m_subId;
    			accountbill.m_billId            = itf->m_detailItemCode;
    			accountbill.m_areaCode          = l_areaCode;
				accountbill.m_value             = itf->m_ofee*m_divisor;
     	 		accountbill.m_discntValue       = itf->m_nfee*m_divisor;
    			accountbill.m_a_discnt          = itf->m_afee*m_divisor;
    			accountbill.m_b_discnt          = itf->m_bfee*m_divisor;
			    accountbill.m_acct_adjustBefore = itf->m_dfee*m_divisor;
			   	accountbill.m_userGrpDiscnt	    = itf->m_gfee*m_divisor;
			    accountbill.m_userAdjust        = itf->m_ufee*m_divisor;
			    
			    m_pComBill->updateBill(accountBills,accountbill);
	 	} 
	}
	return true;		
}


/**
* updateAccountBill: 更新账户账单，
* @param	detailBills	       明细账单集合
* @param	accountBills	     账户账单
* @param  r_idInfo           用户信息.
* @return	true false
*/
bool AcctBillMgr::updateAccountBill(const vector<DetailBill> &r_detailBills ,vector<AccountBill>  &r_accountBills,const IdInfo &r_idInfo)
{
	for(vector<DetailBill>::const_iterator itd=r_detailBills.begin();
		 itd!=r_detailBills.end();itd++)
  {
  	if (itd->m_idType == ACCT_ID_TYPE)
  		updateAccountBill(*itd,r_accountBills,r_idInfo);	
  }	
  ::sort(r_accountBills.begin(),r_accountBills.end());
  return true;
}


void AcctBillMgr::getRateDiscntBill(const T_SERVER_ID &r_userId,
	                              vector<UserAccountBill> r_userAccountBills,
	                              vector<RateDiscntBill> &r_rateDiscntBills)
{
	r_rateDiscntBills.clear();
	for(vector<UserAccountBill>::iterator l_iter=r_userAccountBills.begin();
		 l_iter!=r_userAccountBills.end();++l_iter)
	{
			RateDiscntBill l_rateDiscntBill;
			l_rateDiscntBill.m_accountId = l_iter->m_acctId;
			l_rateDiscntBill.m_userId    = r_userId;
			for(vector<DetailItemFee>::iterator l_iterd=l_iter->m_detailItemFeeVector.begin();
 		 		l_iterd!=l_iter->m_detailItemFeeVector.end();++l_iterd)
 		 	{
			 l_rateDiscntBill.m_billId  = l_iterd->m_detailItemCode;
			 l_rateDiscntBill.m_value   = l_iterd->m_nfee;
			 m_pComBill->updateBill(r_rateDiscntBills,l_rateDiscntBill);
		  }		  
	}
}

/**
* getDetailBill:转换用户账单成明细账单,需过滤不出帐帐目
* @param	userId	用户标识
* @param	userBills	用户账单
* @param  detailBills 明细账单	
* @return	true false
*/
bool AcctBillMgr::getDetailBill(const T_SERVER_ID &r_id, 
	                          const ID_TYPE &r_idtype,
	                          const T_USER_ID &r_userId,
	                          const vector<UserBill> &r_userBills,
	                          DetailBill &r_detailBill
	                          )
{	 	
	 r_detailBill.m_id 			= r_id;
	 r_detailBill.m_idType 	= r_idtype;
	 
	 UserDetailBill l_userDetailBill;
	 l_userDetailBill.m_subId = r_userId;
	 
	 if (r_idtype==USER_ID_TYPE||r_idtype==ACCT_ID_TYPE)//普通用户或账户
   { 
	 	l_userDetailBill.m_priority = 0;
	 }
	 else
	 {
			l_userDetailBill.m_priority = m_pComBill->m_groupUserIdMap[r_id][r_userId].m_orderNo; //modify by xuf 2009-12-24 20:58:27
	 }
	 
	 l_userDetailBill.m_feeVector.clear();
	 DetailItemFee  l_detailItemFee;
	 
	 for (vector<UserBill>::const_iterator  iteru=r_userBills.begin();
	 	    iteru!=r_userBills.end();iteru++)
	 	{
	 		//如果非指定用户账单,继续下一条记录
	 		if (iteru->m_userId != r_userId)
	 			continue;
	 			
   		//如果非帐务明细帐目编码，继续下一条记录
    	if (!m_pComBill->ifItemExists(iteru->m_billId))
    	{
        continue;
    	}
      
       l_detailItemFee.m_detailItemCode = iteru->m_billId;
      
       if (r_idtype==USER_ID_TYPE)//用户
       {
       	//账目级进位方法
			l_detailItemFee.m_ofee 					= m_pComBill->carry(iteru->m_billId,iteru->m_value);
			l_detailItemFee.m_nfee 					= m_pComBill->carry(iteru->m_billId,iteru->m_value);
			l_detailItemFee.m_afee 					= 0;
			l_detailItemFee.m_cfee 					= l_detailItemFee.m_ofee;
			l_detailItemFee.m_bfee 					= 0;
			l_detailItemFee.m_dfee 					= 0;
       }
       else
       {
       	//账目级进位方法
			l_detailItemFee.m_ofee 					= m_pComBill->carry(iteru->m_billId,iteru->m_discntValue); 
			l_detailItemFee.m_nfee 					= l_detailItemFee.m_ofee;
			l_detailItemFee.m_cfee 					= l_detailItemFee.m_ofee;
			l_detailItemFee.m_bfee 					= m_pComBill->carry(iteru->m_billId,iteru->m_adjustBefore); 
			l_detailItemFee.m_dfee 					= 0; 
			l_detailItemFee.m_afee 					= m_pComBill->carry(iteru->m_billId,(iteru->m_value-iteru->m_discntValue-iteru->m_adjustBefore));
       }
       
       l_userDetailBill.m_feeVector.push_back(l_detailItemFee);
         
         //modify by zhouq 20100617 for 群优惠成员原始账单不能输出      
        //原始费用记录到优惠明细信息结构中
       //	m_pComBill->setEffectTrack(r_id,r_idtype, r_userId,iteru->m_billId, iteru->m_value,'0',0,0);  
        if(r_id == r_userId)
       	  m_pComBill->setEffectTrack(r_id,USER_ID_TYPE, r_userId,iteru->m_billId, iteru->m_value,'0',0,0);
    } 
    
	::partial_sort(l_userDetailBill.m_feeVector.begin(),  //zhaogc add 20091030
                 l_userDetailBill.m_feeVector.end(),
                 l_userDetailBill.m_feeVector.end());
    r_detailBill.m_detailBillVector.push_back(l_userDetailBill);
  
    //add by zhouq 20100617 for 生成优惠方式为7的0记录 begin
    if(r_id == r_userId)
    {
        if(m_pRateInfo->m_billUserId == r_id)
	        m_pComBill->setEffectTrack(r_id,USER_ID_TYPE,m_pRateInfo->m_billUserId,0,0,'7',0,9999);
    }
    //add by zhouq 20100617 for 生成优惠方式为7的0记录 end
      
	return !(l_userDetailBill.m_feeVector.empty());
}

/**
* getDetailBill:取用户群明细账单：除userId之外的userBills 加到detailBill
* @param	r_userGroupId	群标识
* @param	r_userId	    用户标识
* @param	r_userIdSet	  用户群下用户
* @param  r_detailBill  明细账单	
* @return	true false
*/
bool AcctBillMgr::getDetailBill(const T_SERVER_ID &r_userGroupId,
	                          const T_USER_ID &r_userId,
	                          set<T_USER_ID> &r_userIdSet,
	                          DetailBill &r_detailBill
	                          )
{  
	userBillToGrpBill(r_detailBill);
	//取群内其他用户的用账单
	vector<UserBill> l_userBills;
	m_pComBill->m_userGrpBills_new.clear();
			  	   
	if(!m_pRateInfo->getUserBill(m_pComBill->m_cycleId,r_userIdSet,r_userId,l_userBills))
	  	return false;

	    
	  //添加群内其他用户到明细账单
		for (set<T_USER_ID>::iterator itgu1 = r_userIdSet.begin();
	     itgu1 != r_userIdSet.end();++itgu1)
	  {
	    	if (*itgu1!=r_userId)
	    	{ 
           		getDetailBill(r_userGroupId,GROUP_ID_TYPE,*itgu1,l_userBills,r_detailBill);
           	 //add by zhouq 20100617 for 生成优惠方式为7的0记录 begin
 	           m_pComBill->setEffectTrack(r_userGroupId,GROUP_ID_TYPE,*itgu1,0,0,'7',0,9999);
 	          //add by zhouq 20100617 for 生成优惠方式为7的0记录 end
	    	}
	  } 
	  
	   ::partial_sort(r_detailBill.m_detailBillVector.begin(),
                 	r_detailBill.m_detailBillVector.end(), 
                  r_detailBill.m_detailBillVector.end());
	  
	 //更新明细账单到用户账单 
	 if(!m_pComBill->m_grpDiscntForAllUsers) //群下部分用户参与总量优惠
	 {
	 	if(!updateUserBill(r_detailBill,m_pComBill->m_userGrpBills_new))
			 return false;
	 }
	                  
	return !(r_detailBill.m_detailBillVector.empty());	
}


 /**
* getDetailBill 根据把群下指定角色用户转换成明细账单
* @param  r_userGrpId  虚拟用户标识
* @param	r_userIdSet	 用户群下成员
* @param  r_userBills  用户账单
* @param  r_detailBill 明细账单
* @param  r_roleCode   角色
* @param  m_roleCodeVector   枚举角色：离散
* @param  m_roleCodeMap   枚举角色：连续
* @return true 成功,false 失败  
*/ 
bool AcctBillMgr::getDetailBill(const T_SERVER_ID &r_userGrpId, 
	                              set<T_USER_ID> &r_userIdSet,
                       					const vector<UserBill> &r_userBills,
                       					DetailBill &r_detailBill,
                       					const T_MEMBER_TYPE &r_roleCode,
																vector<T_MEMBER_TYPE> 	&r_roleCodeVector,  
																map<T_MEMBER_TYPE, T_MEMBER_TYPE> &r_roleCodeMap
                       					)
{
	if (r_roleCode<ROLECODE_SELF&&r_roleCode!=ROLECODE_ENUM)
		return false;
	
 map<T_USER_ID,T_MEMBERINFO_TYPE> l_userTypeMap;		
 if (r_roleCode>ROLECODE_SELF||r_roleCode==ROLECODE_ENUM)
 {	
	map<T_SERVER_ID,map<T_USER_ID,T_MEMBERINFO_TYPE> >::iterator t_pos;
	if ((t_pos=m_pComBill->m_groupUserIdMap.find(r_userGrpId))==m_pComBill->m_groupUserIdMap.end())
		if(!m_pComBill->GetUserInfoByGrpId(r_userGrpId))
			return false;
	
	if (t_pos!=m_pComBill->m_groupUserIdMap.end())
	{
		l_userTypeMap = t_pos->second;		
	}
	else
		l_userTypeMap = m_pComBill->m_groupUserIdMap[r_userGrpId];
 }	
 
  for (set<T_USER_ID>::iterator itgu1 = r_userIdSet.begin();
	     itgu1 != r_userIdSet.end();++itgu1)
	  {	  		  		
	  	if (r_roleCode==ROLECODE_SELF||
	  		  l_userTypeMap[*itgu1].m_memberRole==r_roleCode||
	  		  (r_roleCode==ROLECODE_ENUM&&m_pComBill->judgeRoleCodeIn(r_roleCodeVector,r_roleCodeMap,l_userTypeMap[*itgu1].m_memberRole)))
	      getDetailBill(r_userGrpId,*itgu1,r_userBills,r_detailBill);
	  }
	    
	  return !(r_detailBill.m_detailBillVector.empty());
} 

/**
* getDetailBill:转换用户账单成明细账单
* @param	r_id	虚拟用户标识
* @param	r_userId	用户标识
* @param	r_userBills	用户账单
* @param  r_detailBill 明细账单	
* @return	true false
*/
bool AcctBillMgr::getDetailBill(const T_SERVER_ID &r_id, 
	                          		const T_USER_ID &r_userId,
	                          		const vector<UserBill> &r_userBills,
	                          		DetailBill &r_detailBill
	                          		)
{	 	
	 r_detailBill.m_id 			= r_id;
	 r_detailBill.m_idType 	= GROUP_ID_TYPE;
	 
	 UserDetailBill l_userDetailBill;
	 l_userDetailBill.m_subId = r_userId;
	 l_userDetailBill.m_priority = m_pComBill->m_groupUserIdMap[m_pComBill->m_userGrpId][r_userId].m_orderNo;
	 
	 l_userDetailBill.m_feeVector.clear();
	 DetailItemFee  l_detailItemFee;
	 
	 for (vector<UserBill>::const_iterator  iteru=r_userBills.begin();
	 	    iteru!=r_userBills.end();iteru++)
	 	{
	 		//如果非指定用户账单,继续下一条记录
	 		if (iteru->m_userId != r_userId)
	 			continue;
	 			      
       	l_detailItemFee.m_detailItemCode = iteru->m_billId;
				l_detailItemFee.m_ofee 					 = m_pComBill->carry(iteru->m_billId,iteru->m_value); 
       	l_detailItemFee.m_nfee 					 = m_pComBill->carry(iteru->m_billId,iteru->m_discntValue);  
       	l_detailItemFee.m_afee 					 = m_pComBill->carry(iteru->m_billId,iteru->m_value - iteru->m_discntValue);		
       	l_detailItemFee.m_cfee 					 = l_detailItemFee.m_ofee;
				l_detailItemFee.m_bfee 					 = 0; 
				l_detailItemFee.m_dfee 					 = 0;        	      		
    
       l_userDetailBill.m_feeVector.push_back(l_detailItemFee);
              
    } 
    
    ::partial_sort(l_userDetailBill.m_feeVector.begin(),  //zhaogc add 20091030
                 l_userDetailBill.m_feeVector.end(),
                 l_userDetailBill.m_feeVector.end());
    r_detailBill.m_detailBillVector.push_back(l_userDetailBill);
    
	return !(l_userDetailBill.m_feeVector.empty());
}


/**
* updateUserBill: 更新用户账单
* @param	detailBill	明细账单
* @param	userBills	  用户账单
* @return	true false
*/
bool AcctBillMgr::updateUserBill(const DetailBill &detailBill,vector<UserBill> &userBills)
{
	for(vector<UserDetailBill>::const_iterator iterd=detailBill.m_detailBillVector.begin();
		iterd!=detailBill.m_detailBillVector.end();iterd++)
	{
		for(vector<DetailItemFee>::const_iterator iterf=iterd->m_feeVector.begin();
			iterf!=iterd->m_feeVector.end();iterf++)
		 {
			UserBill l_userBill;
			l_userBill.m_userId       = iterd->m_subId;
			l_userBill.m_billId       = iterf->m_detailItemCode;
			l_userBill.m_value        = iterf->m_ofee*m_divisor;
			l_userBill.m_discntValue  = iterf->m_nfee*m_divisor;
			l_userBill.m_adjustBefore = iterf->m_bfee*m_divisor;
		  
		  updatePartBill(userBills,l_userBill);
		 } 
	}
	
	return true;
}


/**
* getDetailBill:转换多个用户的用户账单成明细账单
* @param	r_id	虚拟用户标识
* @param	r_userIds	用户标识结合
* @param	r_userBills	用户账单
* @param  r_detailBill 明细账单	
* @return	true false
*/
bool AcctBillMgr::getDetailBill(const T_ACCT_ID &r_userGrpId,
	                          		const set<T_USER_ID> &r_userIds,
	                          		const vector<UserBill> &r_userBills,
	                          		DetailBill &r_detailBill
	                          		)
{	 
	r_detailBill.m_id     = r_userGrpId;
	r_detailBill.m_idType = GROUP_ID_TYPE;
	r_detailBill.m_detailBillVector.clear();
	 
 	map<T_SERVER_ID,map<T_USER_ID,T_MEMBERINFO_TYPE> >::iterator t_pos;
	if ((t_pos=m_pComBill->m_groupUserIdMap.find(r_userGrpId))==m_pComBill->m_groupUserIdMap.end())
		if(!m_pComBill->GetUserInfoByGrpId(r_userGrpId))
			return false;
	
	map<T_USER_ID,T_MEMBERINFO_TYPE> l_userTypeMap;	
	if (t_pos!=m_pComBill->m_groupUserIdMap.end())
	{
		l_userTypeMap = t_pos->second;		
	}
	else
		l_userTypeMap = m_pComBill->m_groupUserIdMap[r_userGrpId];	 
	      
     for (vector<UserBill>::const_iterator  itera=r_userBills.begin();
             itera!=r_userBills.end();itera++)
	 	{   
	 		//如果不是该用户集的
	 	 if (r_userIds.find(itera->m_userId)==r_userIds.end())
	 		{
	 			continue;
	 		}
	 				 			
		UserDetailBill t_userDetailBill;
		t_userDetailBill.m_subId    = itera->m_userId;
		t_userDetailBill.m_priority = l_userTypeMap[t_userDetailBill.m_subId].m_orderNo;
                    
		DetailItemFee t_detailItemFee;
		t_detailItemFee.m_detailItemCode = itera->m_billId;
	       	
		t_detailItemFee.m_ofee 					= m_pComBill->carry(t_detailItemFee.m_detailItemCode,itera->m_value); 
		t_detailItemFee.m_nfee 					= m_pComBill->carry(t_detailItemFee.m_detailItemCode,itera->m_discntValue);   
		t_detailItemFee.m_afee   				= m_pComBill->carry(t_detailItemFee.m_detailItemCode,(itera->m_value-itera->m_discntValue-itera->m_adjustBefore));   
		t_detailItemFee.m_cfee 					= m_pComBill->carry(t_detailItemFee.m_detailItemCode,itera->m_value); 
		t_detailItemFee.m_bfee 					= m_pComBill->carry(t_detailItemFee.m_detailItemCode, itera->m_adjustBefore);
	 			
		t_userDetailBill.m_feeVector.push_back(t_detailItemFee); 
	 					
	 	   vector<UserDetailBill>::iterator t_it;
	 	   for (t_it=r_detailBill.m_detailBillVector.begin();
	 	        t_it!=r_detailBill.m_detailBillVector.end();++t_it)
	 			{
					if (t_it->m_subId==t_userDetailBill.m_subId)//找到该用户账单	
					{
						t_it->m_feeVector.push_back(t_detailItemFee);
						::partial_sort((*t_it).m_feeVector.begin(),  //zhaogc add 20091030
                           (*t_it).m_feeVector.end(), (*t_it).m_feeVector.end());
						break;	
					} 					
	 			}
	 					
	 	   if(t_it==r_detailBill.m_detailBillVector.end())//未找到该用户账单
	 	   {  
	 	   	  ::partial_sort(t_userDetailBill.m_feeVector.begin(),  //zhaogc add 20091030
                 					t_userDetailBill.m_feeVector.end(),
                 					t_userDetailBill.m_feeVector.end());  
	 	   		r_detailBill.m_detailBillVector.push_back(t_userDetailBill);
	 	   }
	 					

    }
         
   ::partial_sort(r_detailBill.m_detailBillVector.begin(),
                  r_detailBill.m_detailBillVector.end(), 
                  r_detailBill.m_detailBillVector.end());   
                   
  return !(r_detailBill.m_detailBillVector.empty());		
}


//r_acctId的r_accountBills添加到r_detailBill，r_userIds的账单除外
bool AcctBillMgr::getDetailBill(const T_ACCT_ID &r_acctId,
	                          const set<T_USER_ID> &r_userIds,
	                          vector<AccountBill> &r_accountBills,
	                          DetailBill &r_detailBill
	                          )
{	 
	r_detailBill.m_id     = r_acctId;
	r_detailBill.m_idType = ACCT_ID_TYPE;
	r_detailBill.m_detailBillVector.clear();
	vector<AccountBill> t_accountBills;
 	t_accountBills.clear();
	 
	 map<T_SERVER_ID,int> l_userOrderMap;
	 if (m_pComBill->getUserDiscntOrderInAcct(r_acctId,l_userOrderMap)<=0) return false;
	 
     t_accountBills.swap(r_accountBills);
     
     for (vector<AccountBill>::iterator  itera=t_accountBills.begin();
             itera!=t_accountBills.end();itera++)
	 	{   
         //备份账单和新账单保持一致
         r_accountBills.push_back(*itera); 
        
	 	//如果不是该账户的，或者是该用户集的
	 	 if (itera->m_accountId!=r_acctId||
	 	    (r_userIds.find(itera->m_userId)!=r_userIds.end()))
	 		{
	 			  continue;
	 		}
	 				 			
		UserDetailBill t_userDetailBill;
		t_userDetailBill.m_subId    = itera->m_userId;
		t_userDetailBill.m_priority = l_userOrderMap[t_userDetailBill.m_subId];
           
           
		DetailItemFee t_detailItemFee;
		t_detailItemFee.m_detailItemCode = itera->m_billId;
  	       //进位	       	
		   t_detailItemFee.m_ofee 					= m_pComBill->carry(t_detailItemFee.m_detailItemCode,itera->m_value); 
		   t_detailItemFee.m_nfee 					= t_detailItemFee.m_ofee;  
		   t_detailItemFee.m_afee   				= m_pComBill->carry(t_detailItemFee.m_detailItemCode,(itera->m_a_discnt-(itera->m_value-itera->m_discntValue-itera->m_acct_adjustBefore)));   
		   t_detailItemFee.m_cfee 					= m_pComBill->carry(t_detailItemFee.m_detailItemCode,itera->m_value); 
		   t_detailItemFee.m_bfee 					= m_pComBill->carry(t_detailItemFee.m_detailItemCode, itera->m_b_discnt);
			 t_detailItemFee.m_dfee          = 0;
	 			
	 	   t_userDetailBill.m_feeVector.push_back(t_detailItemFee); 
	 					
	 	   vector<UserDetailBill>::iterator t_it;
	 	   for (t_it=r_detailBill.m_detailBillVector.begin();
	 	        t_it!=r_detailBill.m_detailBillVector.end();++t_it)
	 			{
					if (t_it->m_subId==t_userDetailBill.m_subId)//找到该用户账单	
					{
						t_it->m_feeVector.push_back(t_detailItemFee);
						break;	
					} 					
	 			}
	 					
	 	   if(t_it==r_detailBill.m_detailBillVector.end())//未找到该用户账单
	 	   {
	 	     ::partial_sort(t_userDetailBill.m_feeVector.begin(),  //zhaogc add 20091030
                 				t_userDetailBill.m_feeVector.end(),
                 				t_userDetailBill.m_feeVector.end());
	 	       r_detailBill.m_detailBillVector.push_back(t_userDetailBill);
	 	   }
	 					
      		 //原始费用记录到优惠明细信息结构中   
      		 m_pComBill->setEffectTrack(r_acctId,ACCT_ID_TYPE, itera->m_userId,itera->m_billId, itera->m_value,'0',0,0);  

    }
         
   ::partial_sort(r_detailBill.m_detailBillVector.begin(),
                  r_detailBill.m_detailBillVector.end(), 
                  r_detailBill.m_detailBillVector.end());   
                   
  return !(r_detailBill.m_detailBillVector.empty());		
}

bool AcctBillMgr::getDetailBill(set<T_ACCT_ID> &r_acctIdSet, 
	                          set<T_USER_ID> &r_userIds,  
	                         	vector<DetailBill> &r_detailBills,
	                         	vector<AccountBill> &r_accountBills
	                     			)
{
   //费用移动
   for(vector<DetailBill>::iterator  t_iter=r_detailBills.begin();
    	  t_iter!=r_detailBills.end();t_iter++)
   {
   	 userBillToGrpBill(*t_iter);
   }
	
	//循环账户集，取账户账单
//  for(set<T_ACCT_ID>::iterator iteram=r_acctIdSet.begin();
//  	iteram!=r_acctIdSet.end();iteram++)
//  {
//		m_pRateInfo->getUserBill(m_pComBill->m_cycleId,*iteram ,r_accountBills);
//  }
  
  if(!m_pRateInfo->getUserBill(m_pComBill->m_cycleId,r_acctIdSet,r_accountBills))
  	return false;
  
  for(set<T_ACCT_ID>::iterator iteram=r_acctIdSet.begin();
  	iteram!=r_acctIdSet.end();iteram++)
  {
  	//把账户账单（排除出帐用户）添加到明细账单
  	DetailBill l_detailBill;
  	if(!getDetailBill(*iteram,r_userIds,r_accountBills,l_detailBill))
  		continue;
      
    vector<DetailBill>::iterator t_it;
    for(t_it=r_detailBills.begin();
    	  t_it!=r_detailBills.end();t_it++)
    {   	
    		if (*t_it==l_detailBill)
    		{
    			*t_it+=l_detailBill;
    			::partial_sort(	(*t_it).m_detailBillVector.begin(),
                   		 		(*t_it).m_detailBillVector.end(), 
                    			(*t_it).m_detailBillVector.end());
    			 break;
    		}		
  
    }
      
    //not find
    if (t_it==r_detailBills.end()) 
    	r_detailBills.push_back(l_detailBill); 	
    	    
	}
 	::sort(r_accountBills.begin(),r_accountBills.end());
  return !(r_detailBills.empty());		
}


/**
* getDetailBill:根据用户账户账单得到明细账单
* @param	userId           用户标识
* @param	userAccountBills 明细账单
* @param	detailBill 明细账单
* @return	true false
*/
bool AcctBillMgr::getDetailBill(const T_USER_ID &r_userId, 
	                          const vector<UserAccountBill> &r_userAccountBills ,
	                          vector<DetailBill> &r_detailBills)
{
	for(vector<UserAccountBill>::const_iterator iter= r_userAccountBills.begin();
		  iter!= r_userAccountBills.end();++iter)
  {
  	  DetailBill l_detailBill;
    	l_detailBill.m_id      = iter->m_acctId;
    	l_detailBill.m_idType  = ACCT_ID_TYPE;
    	
    	UserDetailBill l_userDetailBill;
      l_userDetailBill.m_subId     = r_userId;
      l_userDetailBill.m_priority  = iter->m_userPriority;
      l_userDetailBill.m_feeVector = iter->m_detailItemFeeVector;
      
    	l_detailBill.m_detailBillVector.push_back(l_userDetailBill);
    	
    	vector<DetailBill>::iterator t_itd;
    	for(t_itd=r_detailBills.begin();
    		t_itd!=r_detailBills.end();++t_itd)
    	{
    		 if(*t_itd==l_detailBill)
    		 {
    		 	 (*t_itd)+=l_detailBill;
    		 	 	break;	
    		 }
    	}
    	
    	if(t_itd==r_detailBills.end())
    		r_detailBills.push_back(l_detailBill);    	
  }
  
	return !(r_detailBills.empty());
}

void AcctBillMgr::getDetailItemFee(const T_SERVER_ID &r_userId,
	                   vector<RateDiscntBill> r_rateDiscntBills,
	                   vector<DetailItemFee> &r_detailItemFees)
{
	for(vector<RateDiscntBill>::iterator l_iter=r_rateDiscntBills.begin();
		 l_iter!=r_rateDiscntBills.end();++l_iter)
	{
		 if (l_iter->m_userId==r_userId)
		 {
		 	DetailItemFee l_detailItemFee;
		 	l_detailItemFee.m_detailItemCode = l_iter->m_billId;
		 	l_detailItemFee.m_ofee = l_iter->m_value;         
   	  l_detailItemFee.m_nfee = l_iter->m_value;          	
			l_detailItemFee.m_afee = 0;         
			l_detailItemFee.m_bfee = 0;           
			l_detailItemFee.m_cfee = l_iter->m_value;  
			l_detailItemFee.m_dfee = 0;       
		 	
		 	m_pComBill->updateBill(r_detailItemFees,l_detailItemFee);
		 
		 }
	}
}

void AcctBillMgr::userBillToGrpBill(DetailBill &r_detailBill)
{
	for (vector<UserDetailBill>::iterator t_it=r_detailBill.m_detailBillVector.begin();
		   t_it!=r_detailBill.m_detailBillVector.end();++t_it)
  {
  		for(vector<DetailItemFee>::iterator t_itd=t_it->m_feeVector.begin();
  			  t_itd!=t_it->m_feeVector.end();++t_itd)
  		{
  				t_itd->m_ofee = t_itd->m_nfee;
  		}
  }
}


DetailBill *AcctBillMgr::getRoleBill(const T_SERVER_ID     &r_groupId, T_MEMBER_TYPE &r_roleCode,ID_TYPE r_idType)
{	
  if (r_idType==USER_ID_TYPE) //参考群
{
	m_pComBill->m_otherDetailBill.m_detailBillVector.clear();
	map<T_SERVER_ID,map<T_USER_ID,T_MEMBERINFO_TYPE> >::iterator t_pos;
	if ((t_pos=m_pComBill->m_groupUserIdMap.find(r_groupId))==m_pComBill->m_groupUserIdMap.end())
		if(!m_pComBill->GetUserInfoByGrpId(r_groupId))
			return NULL;
	
	map<T_USER_ID,T_MEMBERINFO_TYPE> l_userTypeMap;	
	if (t_pos!=m_pComBill->m_groupUserIdMap.end())
	{
		l_userTypeMap = t_pos->second;		
	}
	else
		l_userTypeMap = m_pComBill->m_groupUserIdMap[r_groupId];
  
  	//取群内指定用户的用户账单
		vector<UserBill> l_userBills;
		for (map<T_USER_ID,T_MEMBERINFO_TYPE>::iterator itgu = l_userTypeMap.begin();
	     itgu != l_userTypeMap.end();++itgu)
  	{
  		if (r_roleCode<=0 ||r_roleCode==itgu->second.m_memberRole)
      { 	 	
      	//如果是出帐用户,从内部DetailBill 获取 add by xuf 2010-1-6 9:38:46
      	if(itgu->first==m_pComBill->m_userId)
      	{
      		  UserDetailBill t_userDetailBill;
      		  t_userDetailBill.m_subId= m_pComBill->m_userId;
      		  t_userDetailBill.m_priority = 0;
      		  vector<UserDetailBill>::iterator t_itr;
      		    		  
      		  //从m_pComBill->m_detailBil 查找出帐用户明细账单
      		  t_itr=::lower_bound(m_pComBill->m_detailBill.m_detailBillVector.begin(),
      		  	                m_pComBill->m_detailBill.m_detailBillVector.end(),t_userDetailBill);
      		  if ((t_itr != m_pComBill->m_detailBill.m_detailBillVector.end())&&(*t_itr == t_userDetailBill))
      		  {
      		  		t_userDetailBill=*t_itr;
      		  }
      		  
      		  //从 m_pComBill->m_otherDetailBill 查找是否存在 		            
      			//if(!::binary_search(m_pComBill->m_otherDetailBill.m_detailBillVector.begin(),m_pComBill->m_otherDetailBill.m_detailBillVector.end(),t_userDetailBill))
      			if(find(m_pComBill->m_otherDetailBill.m_detailBillVector.begin(),m_pComBill->m_otherDetailBill.m_detailBillVector.end(),t_userDetailBill)==m_pComBill->m_otherDetailBill.m_detailBillVector.end())
      			{
      				//添加出帐用户到m_pComBill->m_otherDetailBill
      				m_pComBill->m_otherDetailBill.m_detailBillVector.push_back(t_userDetailBill);
      			}	
      			    	    	
      	}
        else//非出帐用户
        {	 	
 	  		l_userBills.clear();
      	 	m_pRateInfo->getUserBill(m_pComBill->m_cycleId,itgu->first,l_userBills);
      	 	getDetailBill(r_groupId,GROUP_ID_TYPE,itgu->first,l_userBills,m_pComBill->m_otherDetailBill);
	  }
	  }  
	  }  
  }
  else //参考账户
  {
  	//如果账户账单已经存在
  	if(!m_pComBill->m_otherDetailBill.m_detailBillVector.empty()&&
  		 m_pComBill->m_otherDetailBill.m_id==r_groupId)
  	{
  		//更新当前用户明细账单到角色账单
  		UserDetailBill t_userDetailBill;
      t_userDetailBill.m_subId= m_pComBill->m_userId;
      t_userDetailBill.m_priority = 0;
      vector<UserDetailBill>::iterator t_itr,t_itro;
        		  
      //从m_pComBill->m_detailBil 查找出帐用户明细账单
      t_itr=::lower_bound(m_pComBill->m_detailBill.m_detailBillVector.begin(),
      	                m_pComBill->m_detailBill.m_detailBillVector.end(),t_userDetailBill);
      if ((t_itr != m_pComBill->m_detailBill.m_detailBillVector.end())&&(*t_itr == t_userDetailBill))
      {
       	//m_pComBill->m_otherDetailBill 查找出帐用户账单
       	t_itro=::lower_bound(m_pComBill->m_otherDetailBill.m_detailBillVector.begin(),
      	                m_pComBill->m_otherDetailBill.m_detailBillVector.end(),t_userDetailBill);
      	if ((t_itro != m_pComBill->m_otherDetailBill.m_detailBillVector.end())&&(*t_itro == t_userDetailBill))
      	{
      		*t_itro = *t_itr; //找到则更新
      	}
      	else //未找到则添加
      	{
       		//添加出帐用户到m_pComBill->m_otherDetailBill
      		m_pComBill->m_otherDetailBill.m_detailBillVector.push_back(*t_itr);
      	}
      }
  	}
    else
    {
    m_pComBill->m_otherDetailBill.m_detailBillVector.clear();
  	map<T_SERVER_ID,int> t_userOrderMap;
	
	 //获取账户下的用户
    m_pComBill->getUserDiscntOrderInAcct(r_groupId,t_userOrderMap);
		
  	//取群内指定用户的用户账单
		vector<UserBill> t_userBills;
		for (map<T_SERVER_ID,int>::iterator itgu = t_userOrderMap.begin();
	     itgu != t_userOrderMap.end();++itgu)
  	{
      	//如果是出帐用户,并且内部DetailBill有账单，从内部DetailBill获取 
      	if(itgu->first==m_pComBill->m_userId&&
      		!m_pComBill->m_detailBill.m_detailBillVector.empty())
      	{
      		  UserDetailBill t_userDetailBill;
      		  t_userDetailBill.m_subId= m_pComBill->m_userId;
      		  t_userDetailBill.m_priority = 0;
      		  vector<UserDetailBill>::iterator t_itr;
      		    		  
      		  //从m_pComBill->m_detailBil 查找出帐用户明细账单
      		  t_itr=::lower_bound(m_pComBill->m_detailBill.m_detailBillVector.begin(),
      		  	                m_pComBill->m_detailBill.m_detailBillVector.end(),t_userDetailBill);
      		  if ((t_itr != m_pComBill->m_detailBill.m_detailBillVector.end())&&(*t_itr == t_userDetailBill))
      		  {
      		  		t_userDetailBill=*t_itr;
      		  }
      		  
      		  //从 m_pComBill->m_otherDetailBill 查找是否存在 		            
      			if(find(m_pComBill->m_otherDetailBill.m_detailBillVector.begin(),m_pComBill->m_otherDetailBill.m_detailBillVector.end(),t_userDetailBill)==m_pComBill->m_otherDetailBill.m_detailBillVector.end())
      			{
      				//添加出帐用户到m_pComBill->m_otherDetailBill
      				m_pComBill->m_otherDetailBill.m_detailBillVector.push_back(t_userDetailBill);
      			}	
      			    	    	
      	}
        else//非出帐用户或内部DetailBill有账单
        {	 	
 	  		  t_userBills.clear();
      	 	m_pRateInfo->getUserBill(m_pComBill->m_cycleId,itgu->first,t_userBills);
      	 	getDetailBill(r_groupId,ACCT_ID_TYPE,itgu->first,t_userBills,m_pComBill->m_otherDetailBill);
	      }

	  }//for  
	  }//else
  }
  
  //排序账单
	::sort(m_pComBill->m_otherDetailBill.m_detailBillVector.begin(),m_pComBill->m_otherDetailBill.m_detailBillVector.end());
#ifdef _DEBUG_
cout<<"m_pComBill->m_otherDetailBill:"<<endl;
cout<<m_pComBill->m_otherDetailBill<<endl;
#endif
	   
	//若账单为空
	if (m_pComBill->m_otherDetailBill.m_detailBillVector.empty())
		return NULL;
	   
   return    &m_pComBill->m_otherDetailBill;  	
  
}



//取特殊累计量
bool AcctBillMgr::getSpecialSumBill(set<T_SERVER_ID> r_idSet, 
	                              vector<UserSumBill2> &r_sumBillList,
	                              vector<UserSumBill2> &r_sumBillListBak)
{
	 //取累计量
	 vector<UserSumBill2> l_sumBillList;
	 for(set<T_SERVER_ID>::iterator iter=r_idSet.begin();
	 	  iter!=r_idSet.end();++iter)
	 {
	 	//取非月为pComBill->m_otherDetailBill);
	      }

	  }//for  
	  }//else
  }
  
  //鎺掑簭璐﹀崟
	::sort(m_pComBill->m_otherDetailBill.m_detailBillVector.begin(),m_pComBill->m_otherDetailBill.m_detailBillVector.end());
#ifdef _DEBUG_
cout<<"m_pComBill->m_otherDetailBill:"<<endl;
cout<<m_pComBill->m_otherDetailBill<<endl;
#endif
	   
	//鑻ヨ处鍗曚负绌