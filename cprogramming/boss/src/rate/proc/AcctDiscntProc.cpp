#include "AcctDiscntProc.h"
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "Mdb_Exception.h"

INSTALL_PROCEDURE_INSTANCE(AcctDiscntProc);

int AcctDiscntProc::execute()
{		
	try
	{	
		m_pComBill=&m_pRateInfo->m_comBill;
		if (!m_pComBill->ifNeedAccountDeal())
		{
			DEBUG_PRINT("[%s:%d] not need account process!\n",__FILE__,__LINE__);
			return 1;	
		}	
	 

    if(!m_pComBill->m_isExistAcctDiscnt||
			 (m_pComBill->m_isExistAcctDiscnt&&
			 m_pRateInfo->m_bizType!=BIZ_TYPE_ACCTBILL))		
		{
			return 1;
		}
					
  	if(!init())
  		return 1;
   
  	pricing(); 
   
  	updateBill();

  }
	catch (LINKAGE_MDB::Mdb_Exception &e)
	{
	  DEBUG_PRINT("账户优惠错误,原因:[%s]!\n",e.GetString());	
	 	theErrorMessages->insert(E_ACCT_DISCNT,string("账户优惠错误,原因:") + string(e.GetString())); 
	}
    	  
	return 0;   
}


bool AcctDiscntProc::init()
{
 	 //给账户依次加锁
   for(set<T_ACCT_ID>::iterator iter=m_pComBill->m_acctIds.begin();
        iter!=m_pComBill->m_acctIds.end();iter++)
   {
      if(!m_pRateInfo->lockAccountId(*iter))
      {
      	//Added By Sunam 2009/8/10 19:49:16 加锁失败
      	m_pRateInfo->m_lockStatus = false;
      	theErrorMessages->insert(E_INITIALIZE, *iter + "lock account error!"  );
  	    DEBUG_PRINT("帐户[%s]加锁失败!\n",(*iter).c_str());
      	return false;
      }
   }
    
 //加载求取用户帐户账单流程-dingr
  ProcedureHelper::ExecuteProcedure("AcctBillToDetailBillProc");
  	  		 
	return true;
}
    
bool AcctDiscntProc::pricing()
{   
    		
	  //循环执行各账户优惠
			map<T_ACCT_ID,vector<Discount> >::iterator tmp_iter; 
      for (tmp_iter=m_pComBill->m_acctsDiscountMap.begin();
        	 tmp_iter!=m_pComBill->m_acctsDiscountMap.end();
        	 tmp_iter++)
    	{
    		 T_ACCT_ID t_id=tmp_iter->first;
    		 DEBUG_PRINT("帐户[%s]优惠开始...\n",t_id.c_str());
   	  	 if (!m_pComBill->getAcctInfo(t_id,m_pRateInfo->m_idInfo))
     		{
     			theErrorMessages->insert(E_INITIALIZE,t_id+" get account info error!" ); 
     			DEBUG_PRINT("帐户[%s]无资料!",t_id.c_str());
     			continue;
     		}	
   	   

   		  if(!m_acctDiscnt.initialize(t_id,ACCT_ID_TYPE,m_pRateInfo,m_pmManager ,m_pComBill))
     		 {
   	  			theErrorMessages->insert(E_INITIALIZE,t_id+" initialize account discount error ! account: " ); 
     			DEBUG_PRINT("帐户[%s]优惠初始化失败!",t_id.c_str());
     			continue;
   		  	}	
		  

		   if(!m_acctDiscnt.getDiscount(tmp_iter->second))
		   	{
		   	 continue;	
		   	}
		 
    		 if(!m_acctDiscnt.soluteDiscount())
   		 {
     			DEBUG_PRINT("执行帐户[%s]优惠失败!",t_id.c_str());
     			theErrorMessages->insert(E_INITIALIZE,t_id+ " exec account discount error! " ); 
     			continue;	
    		 }
     
    	
    		DEBUG_PRINT("帐户[%s]优惠结束!\n",t_id.c_str());
    
#ifdef DEBUG 
		if(t_id==m_pComBill->m_detailBill.m_id)
		{
      	 cout<<"****************优惠后DetailBill*************"<<endl;   		 
    			cout<<m_pComBill->m_detailBill<<endl;
   	}
#endif
    }//for
    
    
    
    return true;
}
    
bool AcctDiscntProc::updateBill()
{
 	//加载更新用户帐户账单流程-dingr
  	ProcedureHelper::ExecuteProcedure("UpdateAccountBillProc");

#ifdef DEBUG
		cout<<"**********所有账户优惠结束*******************"<<endl; 
 			cout<<"*************m_accountBills_new******************"<<endl;
 			for(vector<AccountBill>::iterator t_itera=m_pRateInfo->m_accountBills_new.begin();
 			t_itera!=m_pRateInfo->m_accountBills_new.end();++t_itera)
 			{
 	 	  	cout<<*t_itera<<endl; 
 			}  
#endif
     //if (m_pRateInfo->m_accountBills_new.size()>0 )
    //{
    //	m_pComBill->updateBalanceAcctId(m_pRateInfo->m_balAcctIdList,m_pRateInfo->m_accountBills_new);
    //}
     
     return true;
}


