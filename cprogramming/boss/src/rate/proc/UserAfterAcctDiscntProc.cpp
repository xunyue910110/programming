#include "UserAfterAcctDiscntProc.h"                                                                                                                  
#include "appfrm/Application.h"                                                                                                              
#include "appfrm/ErrorHandle.h"                                                                                                              
                                                                                                                                             
INSTALL_PROCEDURE_INSTANCE(UserAfterAcctDiscntProc);                                                                                         
                                                                                                                                             
//##ModelId=48D740E70035                                                                                                                     
bool UserAfterAcctDiscntProc::init()                                                                                                                  
{                                                                                                                                            
  //初始化帐务处理公有属性对象                                                                                                               
  m_pComBill = &m_pRateInfo->m_comBill;                                                                                                      
  	                                                                                                                                         
  m_acctBillMgr.initialize(m_pRateInfo); 	                                                                                                   
                                                                                                                                             
  if (!m_pComBill->ifNeedAccountDeal())                                                                                                      
	{                                                                                                                                          
			DEBUG_PRINT("[%s:%d] not need account process!\n",__FILE__,__LINE__);                                                                  
			return false;	                                                                                                                         
	}		                                                                                                                                       
                                                                                                                                             
	return true;                                                                                                                               
}                                                                                                                                            
                                                                                                                                             
                                                                                                                                             
                                                                                                                                             
//##ModelId=4850B4980232                                                                                                                     
void UserAfterAcctDiscntProc::pricing()                                                                                                               
{                                                                                                                                            
#ifdef FP_DEBUG                                                                                                                              
    FP_BEGIN(UserAfterAcctDiscntProc_pricing)                                                                                                         
#endif	                                                                                                                                     
                                                                                                                                             
 //求取用戶帳戶帳單，如果為21事件觸發不需要單獨求取，僅僅對27事件如下求取有效！----fug s 20110420
    
    if(m_pcdr[ATTRI_BIZ_TYPE] != BIZ_TYPE_BILL)
    	{
     		set<T_SERVER_ID> t_userIdSet =  m_pComBill->m_userIdSet;
    		if(t_userIdSet.find(m_pRateInfo->m_billUserId)==t_userIdSet.end())
    			t_userIdSet.insert(m_pRateInfo->m_billUserId);
    			
    		m_pComBill->getUserPayInfo(t_userIdSet); 
    		
    		 if(!m_pRateInfo->getUserBill(m_pComBill->m_cycleId,m_pComBill->m_acctIdSet,m_pRateInfo->m_accountBills_new))
      	    return;	                                                                                                                                         
        ::sort(m_pRateInfo->m_accountBills_new.begin(),m_pRateInfo->m_accountBills_new.end());
 
		 }
//		 else  
//		 	{                                                                                                            
//       ProcedureHelper::ExecuteProcedure("AcctBillToDetailBillProc"); 
//      }
       
   	
      	                                                                                                                                       
  //用户优惠对象                                                                                                                             
	m_userDiscnt.initialize(m_pComBill->m_userId,                                                                                              
                           USER_ID_TYPE,                                                                                                     
                           m_pRateInfo,                                                                                                      
                           m_pmManager,                                                                                                      
                           m_pComBill);                                                                                                      
                                                                                                                                             
#ifdef FP_DEBUG                                                                                                                              
    FP_BEGIN(UserAfterAcctDiscntProc_pricing_UserAcctTpProc)                                                                                          
#endif	                                                                                                                                     
   //取优惠信息（）                                                                                           
		ProcedureHelper::ExecuteProcedure("UserAfterAcctTpProc"); //changed by dingr                                                                  
                                                                                                                                             
#ifdef FP_DEBUG                                                                                                                              
    FP_END(UserAfterAcctDiscntProc_pricing_UserAcctTpProc)                                                                                            
#endif	                                                                                                                                     
	                                                                                                                                           
    m_userDiscnt.getDiscount(m_pComBill->m_userAfterAcctDiscntVec);                                                                                  
                                                                                                                                             
#ifdef FP_DEBUG                                                                                                                              
    FP_BEGIN(UserAfterAcctDiscntProc_pricing_soluteDiscount)                                                                                          
#endif     
     //轉換帳戶帳單為基於用戶的帳戶明細帳單
	   //DetailBill.m_idType = USER_ID_TYPE
	   //DetailBill.m_id = user_id
	   //vector<UserDetailBill>      m_detailBillVector 記錄帳戶id及帳戶帳單轉換的明細帳單。
	   //AcctBillMgr.cpp 新增函數getDetailBill，由帳戶帳單轉換成用戶級明細帳戶帳單。
	   	m_pComBill->m_detailBill.m_id=0;
	    m_pComBill->m_detailBill.m_detailBillVector.clear();                                                                            
   
	   	m_acctBillMgr.getDetailBill(m_pRateInfo->m_userId,m_pRateInfo->m_idInfo,m_pRateInfo->m_accountBills_new,m_pComBill->m_detailBill);
     
     #ifdef _DEBUG_ 	                                                                                                                               
      cout<<"---------------------转换后的明细账单：-------------------"<<endl;
      cout<<m_pComBill->m_detailBill<<endl;
      cout<<"----------------------------------------------------------"<<endl;			                                                                                         
     #endif                                                                                                                              
    //执行用户优惠                                                                                                                           
    if(!m_userDiscnt.soluteDiscount( ))                                                                                                      
    {                                                                                                                                        
			DEBUG_PRINT("[%s:%d] solute user [%s] discount error!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str());                              
	    return;	                                                                                                                               
    } 
    
    DetailBill r_detailBill;
   	r_detailBill.m_idType 	= ACCT_ID_TYPE;
    r_detailBill.m_detailBillVector.clear();
    vector<UserDetailBill>::iterator itera=m_pComBill->m_detailBill.m_detailBillVector.begin();
    T_SERVER_ID r_acct_id = itera->m_subId;
   //轉換用戶明細帳單到帳戶明細帳單vector，更新帳戶明細帳單到帳戶帳單
   	for(;itera!=m_pComBill->m_detailBill.m_detailBillVector.end();itera++)
		 {
		 	if(itera->m_subId != r_acct_id)
		 		{
		 			m_acctBillMgr.updateAccountBill(r_detailBill,m_pRateInfo->m_accountBills_new,m_pRateInfo->m_idInfo);
          r_acct_id = itera->m_subId;
          r_detailBill.m_detailBillVector.clear();
		 		}
		 	 r_detailBill.m_id 			= itera->m_subId;
       UserDetailBill t_userDetailBill;
		   t_userDetailBill.m_subId    = m_pComBill->m_detailBill.m_id;
		   t_userDetailBill.m_priority = 0;
       
       for(vector<DetailItemFee>::const_iterator iterf=itera->m_feeVector.begin();
			iterf!=itera->m_feeVector.end();iterf++)
		 {
		   DetailItemFee t_detailItemFee;
		   t_detailItemFee.m_detailItemCode = iterf->m_detailItemCode;
		   t_detailItemFee.m_ofee   = iterf->m_ofee; 
		   t_detailItemFee.m_nfee   = iterf->m_nfee;
		   t_detailItemFee.m_afee   = iterf->m_afee; 
		   t_detailItemFee.m_bfee   = iterf->m_bfee;
		   t_detailItemFee.m_cfee   = iterf->m_cfee;
		   t_detailItemFee.m_gfee   = iterf->m_gfee;
		   t_detailItemFee.m_dfee   = iterf->m_dfee;
		   t_detailItemFee.m_ufee   = iterf->m_ufee;
       
		   t_userDetailBill.m_feeVector.push_back(t_detailItemFee); 
		  }
	  	::partial_sort(t_userDetailBill.m_feeVector.begin(),
	  			t_userDetailBill.m_feeVector.end(), 
	  			t_userDetailBill.m_feeVector.end());
	  	r_detailBill.m_detailBillVector.push_back(t_userDetailBill);      
	   }
	   //將明細帳單裝換成帳戶級帳單  ----fug 20110420 E
	   m_acctBillMgr.updateAccountBill(r_detailBill,m_pRateInfo->m_accountBills_new,m_pRateInfo->m_idInfo);                                                                                                                                        

#ifdef FP_DEBUG                                                                                                                              
    FP_END(UserAfterAcctDiscntProc_pricing_soluteDiscount)                                                                                            
#endif                                                                                                                                       
                                                                                                                                             
    //更新用户账单                                                                                                                           
    updateBill();                                                                                                                            
		                                                                                                                                         
#ifdef FP_DEBUG                                                                                                                              
    FP_END(UserAfterAcctDiscntProc_pricing)                                                                                                           
#endif	                                                                                                                                     
}                                                                                                                                            
                                                                                                                                             
//更新增量账单                                                                                                                               
//##ModelId=48AF623A0392                                                                                                                     
void UserAfterAcctDiscntProc::updateBill()                                                                                                            
{                                                                                                                                            
#ifdef FP_DEBUG                                                                                                                              
    FP_BEGIN(UserAfterAcctDiscntProc_updateBill)                                                                                                      
#endif		                                                                                                                                   
 
  ProcedureHelper::ExecuteProcedure("UpdateAccountBillProc"); 	                                                                                 
  	                                                                                                                                         
#ifdef _DEBUG_ 	                                                                                                                               
    cout<<"************账户级优惠后的用户级优惠后******************"<<endl;	                                                                               
 	 	cout<<m_pComBill->m_detailBill<<endl; 	 	 			                                                                                         
#endif                                                                                                                                       
                                                                                                                                             
#ifdef FP_DEBUG                                                                                                                              
    FP_END(UserAfterAcctDiscntProc_updateBill)                                                                                                        
#endif	  		                                                                                                                               
}                                                                                                                                            
                                                                                                                                             
//##ModelId=48D73C0F0119                                                                                                                     
int UserAfterAcctDiscntProc::execute()                                                                                                                
{                                                                                                                                            
try{                                                                                                                                         
		if(!init())                                                                                                                              
			return 1; 	                                                                                                                           
		                                                                                                                                         
		pricing();                                                                                                                               
	}                                                                                                                                          
	catch(Mdb_Exception &e)                                                                                                                    
  {                                                                                                                                          
#ifdef DEBUG		                                                                                                                             
		cout<<"["<<__FILE__<<":"<<__LINE__<<"]账户级优惠后的用户优惠错误!原因:"<<e.GetString()<<endl;		                                                       
#endif		 	                                                                                                                                 
  	theErrorMessages->insert(E_USER_DISCNT_PARAM, m_pComBill->m_userId + ": userAfterAcct discount  error! error reason= " + string(e.GetString()) ); 
  	return 1;                                                                                                                                
  }	                                                                                                                                         
				                                                                                                                                     
		return 0;                                                                                                                                
}                                                                                                                                            