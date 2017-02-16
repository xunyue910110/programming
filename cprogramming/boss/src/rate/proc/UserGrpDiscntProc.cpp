#include "UserGrpDiscntProc.h"
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

INSTALL_PROCEDURE_INSTANCE(UserGrpDiscntProc);

//##ModelId=48D740E70035
bool UserGrpDiscntProc::init()
{
  //初始化帐务处理公有属性对象
  m_pComBill = &m_pRateInfo->m_comBill;
  	
  m_acctBillMgr.initialize(m_pRateInfo); 
   	
  if(!m_pComBill->initUser(m_pRateInfo->m_billUserId,m_pRateInfo->m_cycParam))
  {
  	return false;
  }
  
  //modify by xuf 2009-11-29 15:26:49
  if(!m_pComBill->getUserGroupInfo(m_pRateInfo->m_billUserId,m_pRateInfo->m_idInfo))
	{
		return false;	
	}
	//add 2009-11-29 15:28:31
	m_pComBill->m_ifExistUserInfo = true;
	
	 //获取用户状态异动信息
	 if (m_pmManager->getUserServState(m_pComBill->m_userId, m_pRateInfo->m_vecUserServState)<=0)
	 {
    	 //基本资料有问题
    	 DEBUG_PRINT("[%s:%d]用户[%s]状态不存在!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str());
    	 m_pComBill->setErrorCode(m_pComBill->m_userId,E_ACCT_GET_USERSERV,"0");
    	 
    	 return false;
   }   
  	
		//获取用户服务信息
		if (m_pmManager->getUserServ(m_pComBill->m_userId, m_pRateInfo->m_vecUserServ)<=0)
		{
         //基本资料有问题
         DEBUG_PRINT("[%s:%d]用户[%s]服务不存在!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str());
         m_pComBill->setErrorCode(m_pComBill->m_userId,E_ACCT_GET_USERSERV,"0");
         
         return false;
		}    
	
	m_pComBill->m_userGroupIdSet.insert(m_pRateInfo->m_billUserId);
		
	ProcedureHelper::ExecuteProcedure("UserGrpTpProc"); 
	
	if (m_pComBill->m_isExistGrpTotalDiscnt)
	{
		//Modified by xuf 2009-11-23 17:32:06
		if(!m_pComBill->getTotalDiscntGroupUsers())
		{
			return false;
		}
		
		set<T_SERVER_ID> t_userIdSet =  m_pComBill->m_userIdSet;
		if(t_userIdSet.find(m_pRateInfo->m_billUserId)==t_userIdSet.end())
			t_userIdSet.insert(m_pRateInfo->m_billUserId);
	
		m_pComBill->getUserPayInfo(t_userIdSet);		
	}
	  	  
	return true;
}

void UserGrpDiscntProc::pricing()
{
#ifdef FP_DEBUG
    FP_BEGIN(UserGrpDiscntProc_pricing)
#endif	

      //取总量优惠群下用户的内存用户账单，转换成内部明细账单
      if(!m_acctBillMgr.getDetailBill(m_pRateInfo->m_billUserId,
      	                              "0",
			                          			m_pComBill->m_userIdSet, 
			                          			m_pComBill->m_detailBill))
		 {
				DEBUG_PRINT("[%s:%d] get group [%s] bill error!\n",__FILE__,__LINE__,m_pComBill->m_userGrpId.c_str());		
		 }
			     		
			     	     	
      //用户群优惠对象初始化
			m_userGroupDiscnt.initialize(m_pRateInfo->m_billUserId, 
			                         		GROUP_ID_TYPE, 
			                         		m_pRateInfo,
			                         		m_pmManager ,
                               		m_pComBill);
        
      
      //取用户群优惠信息（用户资费，系统资费，帐前调帐，特殊优惠）
   		if(!m_userGroupDiscnt.getDiscount(m_pComBill->m_userGrpDiscountVector))
	 		{
			 	DEBUG_PRINT("[%s:%d] get user group [%s] discount error!\n",__FILE__,__LINE__,m_pComBill->m_userGrpId.c_str());
	    	return;	
	 		}
			  		   					
      //群优惠处理（处理总量优惠）
			if(!m_userGroupDiscnt.soluteDiscount( ))
			{
				DEBUG_PRINT("[%s:%d] solute user [%s] discount error!\n",__FILE__,__LINE__,m_pComBill->m_userGrpId.c_str());
	    	return;	
    	}
                       		
#ifdef FP_DEBUG
    FP_END(UserGrpDiscntProc_pricing)
#endif	
}

int UserGrpDiscntProc::execute()
{
try{
		if(!init())
			return 1; 	
		
	//如果存在群总量优惠  modify by xuf 2009/11/24 
    if(m_pComBill->m_isExistGrpTotalDiscnt)  
    {
			pricing();
	  }
	}
	catch(Mdb_Exception &e)
  {
#ifdef DEBUG		
		cout<<"["<<__FILE__<<":"<<__LINE__<<"]用户优惠错误!原因:"<<e.GetString()<<endl;		
#endif		 	
  	theErrorMessages->insert(E_USER_DISCNT_PARAM, m_pComBill->m_userId + ": user discount  error! error reason= " + string(e.GetString()) ); 
  	return 1;
  }	
				
		return 0;
}

