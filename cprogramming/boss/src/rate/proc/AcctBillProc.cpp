#include "AcctBillProc.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

INSTALL_PROCEDURE_INSTANCE(UserBillToDetailBillProc);
INSTALL_PROCEDURE_INSTANCE(AcctBillToDetailBillProc);
INSTALL_PROCEDURE_INSTANCE(UpdateAccountBillProc);
INSTALL_PROCEDURE_INSTANCE(UpdateUserBillProc);

//用户账单->内部明细账单过程
//##ModelId=4A5E9E9D036F
int UserBillToDetailBillProc::execute()
{
	try
	{
		//成员变量初始化
		init();

		//add by xuf 2009-12-27 19:30:54
		m_pComBill->m_detailBill.m_detailBillVector.clear();

		//取用户明细账单(由主流程统一取用户的内存用户账单作为备份)
		if(!m_acctBillMgr.getDetailBill(m_pComBill->m_userId,
		                            		USER_ID_TYPE,
	                          				m_pComBill->m_userId,
	                          				m_pRateInfo->m_userBills_new,
	                          				m_pComBill->m_detailBill))
		{
			//DEBUG_PRINT("[%s:%d] get user:[%s] detailbill error!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str()); 
			return 1; 

		}
		
#ifdef DEBUG 		
 		cout<<"*************m_detailBill******************"<<endl;
 	 	cout<<m_pComBill->m_detailBill<<endl; 			
#endif 

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
void UserBillToDetailBillProc:: init()
{
	m_pComBill     = &m_pRateInfo->m_comBill;
	m_acctBillMgr.initialize(m_pRateInfo);
	//Added By dingr 2009/8/27 14:24:25
    m_pComBill->m_detailBill.m_detailBillVector.clear();
}

//账户账单->内部明细账单
//##ModelId=4A5E9E9D036F
int AcctBillToDetailBillProc::execute()
{

	 init();

	//取账户账单（排除出帐用户）
 	 //并添加到内部明细账单,（非出帐用户）添加到账户备份账单
   	if(!m_acctBillMgr.getAccountBill(m_pComBill->m_acctIdSet,
	  	                            m_pComBill->m_userIdSet,  
	  	                            m_pRateInfo->m_accountBills_new,  
	  	                            m_pRateInfo->m_accountBills))
   	{	 
   		 theErrorMessages->insert(E_ACCT_GET_BILL,"get account detailbill error!" ); 
   		 DEBUG_PRINT("取账户账单失败!\n");
   		 return 0;
   	}
    
#ifdef DEBUG

 		cout<<"*************m_accountBills******************"<<endl;
 			for(vector<AccountBill>::iterator t_itera=m_pRateInfo->m_accountBills.begin();
 			t_itera!=m_pRateInfo->m_accountBills.end();++t_itera)
 			{
 	 	  	cout<<*t_itera<<endl; 
 			}  	
 			
#endif    
    
    //取账户累计量
     m_acctBillMgr.getSpecialSumBill(m_pComBill->m_acctIds,
    	                             m_pRateInfo->m_userSumBill2s_new,
    	                             m_pRateInfo->m_userSumBill2s);
   
	return 1;
	
}
void AcctBillToDetailBillProc:: init()
{
	m_pComBill                   = &m_pRateInfo->m_comBill;
	m_acctBillMgr.initialize(m_pRateInfo);
}


//更新内部明细账单->账户账单
//##ModelId=4A5E9E9D036F
int UpdateAccountBillProc::execute()
{
	try
	{
		//成员变量初始化
		init();

		//更新内部账单格式到用户账单
			if(!m_acctBillMgr.updateAccountBill(m_pRateInfo->m_accountBills_new))
		{
			DEBUG_PRINT("[%s:%d] get user:[%s] updateAccountBill error!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str()); 
			return 1; 

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
void UpdateAccountBillProc:: init()
{
	m_pComBill                  = &m_pRateInfo->m_comBill;
  m_acctBillMgr.initialize(m_pRateInfo);
	
}



//更新内部用户明细账单->用户账单
//##ModelId=4A5E9E9D036F
int UpdateUserBillProc::execute()
{
	try
	{
		//成员变量初始化
		init();

		//更新内部账单格式到用户账单
		  if(!m_acctBillMgr.updateUserBill(m_pComBill->m_detailBill,m_pRateInfo->m_userBills_new))
		{
			DEBUG_PRINT("[%s:%d] get user:[%s] updateUserBill error!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str()); 
			return 1; 
		}
		
	}
	catch(Mdb_Exception &e)
 	{
		#ifdef DEBUG		
		cout<<"["<<__FILE__<<":"<<__LINE__<<"]用户优惠更新账单错误!原因:"<<e.GetString()<<endl;		
		#endif		 	
  		theErrorMessages->insert(E_USER_DISCNT_PARAM, m_pComBill->m_userId + ": user discount  error! error reason= " + string(e.GetString()) ); 
  		return 1;
  	}	
				
		return 0;
}
void UpdateUserBillProc:: init()
{
	m_pComBill     = &m_pRateInfo->m_comBill;
	m_acctBillMgr.initialize(m_pRateInfo);
	
}



