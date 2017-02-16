#include "PayRelationProc.h"
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "Mdb_Exception.h"

using namespace LINKAGE_MDB;

INSTALL_PROCEDURE_INSTANCE(PayRelationProc);
//##ModelId=484F386701D4
int PayRelationProc::execute()
{
#ifdef FP_DEBUG
    FP_BEGIN(PayRelationProc_execute)
#endif			
	try
	{
		m_pComBill = &m_pRateInfo->m_comBill;
  	m_acctBillMgr.initialize(m_pRateInfo);
		
		if (!m_pComBill->ifNeedAccountDeal())
		{
			DEBUG_PRINT("[%s:%d] not need account process !\n",__FILE__,__LINE__);
			return 1;	
		}	
		
		
		//不做付费关系绑定条件（满足条件的不做) add by xuf 2010-1-30 10:24:20
		if (m_pRateInfo->m_paramMgr->m_pCondBasePayRelation!=NULL)
    {
    	//从不做付费关系绑定->使用空账单绑定，防止资料错误或预付费转后付费后，要把已有的账单清掉 add by xuf 2010-3-4 16:48:27
    	if (m_pRateInfo->m_paramMgr->m_pCondBasePayRelation->execute())
    	{
    		//return 0;
    		m_pComBill->m_detailBill.m_detailBillVector.clear();
    	}
    }
			
		//在实时群总量优惠模式下，有群总量优惠,对用户级不做付费关系绑定 add by xuf 2009-11-27 15:12:34
	 	if (m_pmManager->m_isNeedRealGrpDiscntTag&&
	 		 	m_pComBill->m_isExistGrpTotalDiscnt&&
	 		 	m_pRateInfo->m_bizType != BIZ_TYPE_GRPBILL)
	 		 	return 0;
	 	 		
		dealPayRelation(m_payRelationSolution);

		//清空用户集合
		m_pRateInfo->m_detailBillMgr.m_userIdsUnion.clear();
					
		//更新内存账单和输出账单
    updateBill();  
  }
  catch (LINKAGE_MDB::Mdb_Exception &e)
	{
	  DEBUG_PRINT("[%s:%d] 付费关系绑定失败,原因:[%s]!\n",__FILE__,__LINE__,e.GetString());	
		return 1;
	}
  catch (const exception & ex) 
  {
	  DEBUG_PRINT("[%s:%d] 付费关系绑定失败,原因:[%s]!\n",__FILE__,__LINE__,ex.what());	
  	return 1;
  }
#ifdef FP_DEBUG
    FP_END(PayRelationProc_execute)
#endif		
	return 0;
}



//更新内存账单（明细账单->内存账户账单）
//##ModelId=48D73AD50117
void PayRelationProc::updateBill()
{
#ifdef FP_DEBUG
    FP_BEGIN(PayRelationProc_updateBill)
#endif
    	
   	if(!m_pComBill->m_isExistAcctDiscnt||
			(m_pComBill->m_isExistAcctDiscnt&&
		 	(m_pRateInfo->m_pCdr)[ATTRI_BIZ_TYPE]!=BIZ_TYPE_ACCTBILL
		 	))	 	
  {

  	//备份用户账户账单
 		m_acctBillMgr.getAccountBill(m_pComBill->m_acctIdSet,m_pComBill->m_userIdSet,m_pRateInfo->m_accountBills);

#ifdef DEBUG
    cout<<"账户账单备份:"<<endl;
    for (vector<AccountBill>::iterator l_iter= m_pRateInfo->m_accountBills.begin();
    	   l_iter!= m_pRateInfo->m_accountBills.end();++l_iter)
    {
    	cout<<*l_iter<<endl;	
    }
#endif 	    
    //changed by dingr --去掉存在账户级优惠不生成结余文件的限制。因为实时优惠会取账户级优惠，但不进行计算。有此限制会导致有些用户无法生成实时结余文件
    // if (m_pRateInfo->m_accountBills_new.size()>0&&
    // 	  !m_pComBill->m_isExistAcctDiscnt)
    //if (m_pRateInfo->m_accountBills_new.size()>0)
    //{
    //	m_pComBill->updateBalanceAcctId(m_pRateInfo->m_balAcctIdList,m_pRateInfo->m_accountBills_new);
    //}
  }

#ifdef DEBUG
    cout<<"付费关系绑定后的账户账单:m_accountBills_new"<<endl;
    for (vector<AccountBill>::iterator l_iter= m_pRateInfo->m_accountBills_new.begin();
    	   l_iter!= m_pRateInfo->m_accountBills_new.end();++l_iter)
    {
    	cout<<*l_iter<<endl;	
    }
#endif

#ifdef FP_DEBUG
    FP_END(PayRelationProc_updateBill)
#endif		  
}

//##ModelId=48D9DC190331
void PayRelationProc::dealPayRelation(PayRelationSolution& payRelationSolution)
{
#ifdef FP_DEBUG
    FP_BEGIN(PayRelationProc_dealPayRelation)
#endif		
	vector<DetailItemFee> *l_pDetailItemFeeVector=NULL;
  m_pComBill->m_detailBillVector.clear();
	m_pRateInfo->m_accountBills.clear();
	m_pRateInfo->m_accountBills_new.clear();
	
	//add by xuf 2009-12-2 14:54:46
	//虚用户参与付费关系绑定
	if(m_pRateInfo->m_bizType==BIZ_TYPE_GRPBILL)
	{
			if(m_pComBill->m_userIdSet.find(m_pRateInfo->m_billUserId)==m_pComBill->m_userIdSet.end())
			m_pComBill->m_userIdSet.insert(m_pRateInfo->m_billUserId);
	}
	
	//循环用户付费关系处理
	for(set<T_USER_ID>::iterator iteru=	m_pComBill->m_userIdSet.begin();
	   iteru !=	m_pComBill->m_userIdSet.end();iteru++)
 {  
 	
 	   if((m_pRateInfo->m_pCdr)[ATTRI_BIZ_TYPE]!=BIZ_TYPE_GRPBILL&&
        *iteru!=m_pComBill->m_userId) //25事件才处理其他用户付费关系
        continue;

	   //部分用户群的付费关系处理过滤
	   if(m_pRateInfo->m_detailBillMgr.m_userIdsUnionTag != USERIDSUNIONTAG_ALL )
	   {
	 	 if(!m_pRateInfo->m_detailBillMgr.m_userIdsUnion.empty() && 
	 	    m_pRateInfo->m_detailBillMgr.m_userIdsUnion.find(*iteru) == m_pRateInfo->m_detailBillMgr.m_userIdsUnion.end())
	 	 {continue;}
	   }
	   	
 
#ifdef FP_DEBUG
    FP_BEGIN(PayRelationProc_getInfo)
#endif			  
 	  IdInfo    l_idInfo;
 		if((*iteru==m_pComBill->m_userId&&
 			 !m_pComBill->m_ifExistUserInfo)||
 			(*iteru!=m_pComBill->m_userId&&
 			!m_pComBill->getUserInfo(*iteru,l_idInfo))
 			)  
	 {
	 	 DEBUG_PRINT("[%s:%d] 用户[%s]资料错误!\n",__FILE__,__LINE__,(*iteru).c_str());	
	 	 continue;	
	 }	 
	 		 	
	 	if(!m_pComBill->getUserPayInfo(*iteru))
	 	{
	 		DEBUG_PRINT("[%s:%d] 获取用户[%s]付费关系错误!\n",__FILE__,__LINE__,(*iteru).c_str());
//	 	 	theErrorMessages->insert(E_ACCT_GET_PAYRELA, *iteru + "get user payinfo error!"); 		
	 	  continue;	
	 	}


#ifdef FP_DEBUG
    FP_END(PayRelationProc_getInfo)
#endif		 	   	  
 	  vector<UserDetailBill>::iterator iter;
 	  for(iter=m_pComBill->m_detailBill.m_detailBillVector.begin();
 	  	  iter!=m_pComBill->m_detailBill.m_detailBillVector.end();++iter)
 	  {	 
 	  	if(iter->m_subId==(*iteru)) 
 	  	{	
 	  		l_pDetailItemFeeVector=&iter->m_feeVector;
 	  		 break;
 	  	}	
 	  }
 	  
 	  if(iter==m_pComBill->m_detailBill.m_detailBillVector.end()||
 	  	l_pDetailItemFeeVector==NULL)
 	  {
 	    DEBUG_PRINT("[%s:%d] 用户[%s]明细账单未获取到!\n",__FILE__,__LINE__,(*iteru).c_str());
	 	  continue ;	 	
 	  }	

     //账户账单表中init_fee为用户账单表的discnt_value 2009-8-27 23:24:59	 
 	   for(vector<DetailItemFee>::iterator l_itd=l_pDetailItemFeeVector->begin();
   	 l_itd!=l_pDetailItemFeeVector->end();++l_itd)
   	{
   		l_itd->m_ofee = l_itd->m_nfee;
   	}
 	 
 	 
#ifdef DEBUG 
   //付费关系绑定前
   cout<<"用户["<<*iteru<<"]付费关系绑定前!"<<endl;
   for(vector<DetailItemFee>::iterator l_itd=l_pDetailItemFeeVector->begin();
   	 l_itd!=l_pDetailItemFeeVector->end();++l_itd)
   	{
   		cout<<*l_itd<<endl;
   	}
#endif 	 
 	 
		m_ifErrorPayrealation = payRelationSolution.solutePayRelation(*iteru,
                                          m_pComBill,
                                          l_pDetailItemFeeVector,
                                          &m_userAccountBillVector,
                                          m_pRateInfo
                                          );
   
   if (m_ifErrorPayrealation<0)
   {
   	DEBUG_PRINT("[%s:%d] 用户[%s]付费关系绑定失败!\n",__FILE__,__LINE__,(*iteru).c_str());	
   	continue;
   }
   
      //付费关系绑定后
#ifdef DEBUG    
   cout<<"用户["<<*iteru<<"]付费关系绑定后!"<<endl;
   for(vector<UserAccountBill>::iterator l_itd=m_userAccountBillVector.begin();
   	 l_itd!=m_userAccountBillVector.end();++l_itd)
   	{
   		cout<<*l_itd<<endl;
   	}
#endif   

#ifdef FP_DEBUG
    FP_BEGIN(PayRelationProc_billConvet)
#endif	     	
   //用户账户账单->明细账单
   //m_acctBillMgr.getDetailBill(*iteru, m_userAccountBillVector, m_pComBill->m_detailBillVector);
   //用户账户账单->账户账单new 2009-9-9 17:04:24
    m_acctBillMgr.updateAccountBill(*iteru, m_userAccountBillVector, m_pRateInfo->m_accountBills_new,m_pRateInfo->m_idInfo);
    
#ifdef FP_DEBUG
    FP_END(PayRelationProc_billConvet)
#endif	
  
#ifdef FP_DEBUG
    FP_BEGIN(PayRelationProc_rateDiscnt)
#endif	  
   //计费优惠帐务关系处理
		if (*iteru==m_pComBill->m_userId)
		{  
   	  vector<DetailItemFee>  l_rateDetailItemFees;
   	   //转换计费账单到内部明细账单
			m_acctBillMgr.getDetailItemFee(m_pComBill->m_userId,
	                          m_pRateInfo->m_rateDiscnts_new,
	                          l_rateDetailItemFees);
	                          
	   	if(!(l_rateDetailItemFees.empty())) 
	   	{                        
   			m_ifErrorPayrealation = payRelationSolution.solutePayRelation(*iteru,
                                          m_pComBill,
                                          &l_rateDetailItemFees,
                                          &m_userAccountBillVector,
                                          m_pRateInfo
                                          );
      	if (m_ifErrorPayrealation<0)
   			{
   				DEBUG_PRINT("[%s:%d] 用户[%s]付费关系错误!\n",__FILE__,__LINE__,(*iteru).c_str());	
   				continue;
  			}
   
     		m_acctBillMgr.getRateDiscntBill(*iteru, m_userAccountBillVector, m_pRateInfo->m_rateDiscnts_new);		
   		
			}//if(!(l_rateDetailItemFees.empty()) 
		}// if (*iteru== m_pComBill->m_userId)	 
#ifdef FP_DEBUG
    FP_END(PayRelationProc_rateDiscnt)
#endif			
	                                       
 }//for	

#ifdef FP_DEBUG
    FP_END(PayRelationProc_dealPayRelation)
#endif		 
}

//
