#include "AccountAttribProc.h"
#include "RateGlobalInfo.h"
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

INSTALL_PROCEDURE_INSTANCE(AccountAttribProc);

int AccountAttribProc::execute()
{
	try
	{	
	     
#ifdef FP_DEBUG
    FP_BEGIN(AccoutAttribProc)
#endif
		//初始化帐务处理公有属性对象
		m_pComBill      = &m_pRateInfo->m_comBill;
    m_pAccountParam = m_pRateInfo->m_paramMgr;

		//初始化用户                      
		if(!m_pComBill->initUser(m_pRateInfo->m_billUserId,m_pRateInfo->m_cycParam))
		{
			return -1;
		}
	         
    //取账户信息  
		if (!m_pComBill->getAcctInfo(m_pRateInfo->m_billUserId,m_pRateInfo->m_idInfo))
		{    
			//无账户资料             
			return -1;
		}
		 m_pComBill->m_ifExistUserInfo = true;
		 m_pComBill->m_acctIdSet.clear();
		 m_pComBill->m_acctIdSet.insert(m_pRateInfo->m_billUserId);
		 m_pComBill->m_acctIds=m_pComBill->m_acctIdSet;
		 m_pComBill->m_acctDiscntForAllUsers = true;
		 m_pComBill->m_userIdSet.clear();//所有用户都从BILL_ACCOUNT_X从获取,非出帐用户为空 add by xuf 2010-1-5 15:13:20
     m_pRateInfo->m_accountBills_new.clear();//add by xuf 2010-1-5 16:13:05
      	  		
#ifdef FP_DEBUG
    FP_END(AccoutAttribProc)
#endif  		   
   
  }
	catch (LINKAGE_MDB::Mdb_Exception &e)
	{
	 	DEBUG_PRINT("账户优惠错误,原因:[%s]!\n",e.GetString());	
	 	theErrorMessages->insert(E_ACCT_DISCNT,string("账户优惠错误,原因:") + string(e.GetString())); 
	}
    	  
	return 0;   
}

