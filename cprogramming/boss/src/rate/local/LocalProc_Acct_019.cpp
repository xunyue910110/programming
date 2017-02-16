#include "LocalProc_Acct_019.h"

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCEDURE_INSTANCE(AcctGiftDealProc);

//赠款处理过程
int AcctGiftDealProc::execute()
{
	
	if(m_pmManager->getUserOtherInfo(m_pRateInfo->m_billUserId,
                                	 m_pRateInfo->m_cycParam,
                                	 m_userOtherInfoVec)<0)
  {
  	//获取内存数据库出错信息
  	int t_errno=0;
  	string t_errMsg="";
  	
    m_pmManager->getMdbErrMsg(t_errno,t_errMsg);
#ifdef _DEBUG_
		cout<<__FILE__<<":"<<__LINE__<<":errno=["<<t_errno<<"],errmsg=[get UserOtherInfo error,"<<t_errMsg<<"]"<<endl;
#endif  	
	
		return 1;  		
  }                              
                                                        
	getBill();
			
 	m_acctGiftDeal.init(m_pRateInfo->m_billUserId,&m_userOtherInfoVec,&m_accountBillVec,m_pmManager,m_pRateInfo);
 	m_acctGiftDeal.execute();
 	
 	updateBill();
 	
	return 0;
}

//AccountBill->UserDetailBill
void AcctGiftDealProc::getBill()
{
	m_accountBillVec.clear();
  vector<AccountBill>::iterator t_acctBillIt;
  for(t_acctBillIt=m_pRateInfo->m_accountBills_new.begin();
      t_acctBillIt!=m_pRateInfo->m_accountBills_new.end();
      t_acctBillIt++)
   {
   		if(t_acctBillIt->m_userId!=m_pRateInfo->m_billUserId)
   			continue;
   	 
   	 vector<AccountBill>::iterator t_itr = ::lower_bound(m_accountBillVec.begin(),m_accountBillVec.end(),*t_acctBillIt);
    if(t_itr == m_accountBillVec.end() || (!(*t_itr==*t_acctBillIt)))
    {
      m_accountBillVec.insert(t_itr,*t_acctBillIt);
    }else
    {
      *t_itr = *t_acctBillIt;
    }
   	  
   }
  
  
  
}

//UserDetailBill->AccountBill
void AcctGiftDealProc::updateBill()
{
	vector<AccountBill>::iterator t_acctBillIt; 
	for (t_acctBillIt=m_accountBillVec.begin();
	     t_acctBillIt!=m_accountBillVec.end();t_acctBillIt++)
		m_pRateInfo->updateBillTotal<AccountBill>(m_pRateInfo->m_accountBills_new,*t_acctBillIt);

}



