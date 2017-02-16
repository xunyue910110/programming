#include "UserDiscntProc.h"
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

INSTALL_PROCEDURE_INSTANCE(UserDiscntProc);

//##ModelId=48D740E70035
bool UserDiscntProc::init()
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
void UserDiscntProc::pricing()
{
#ifdef FP_DEBUG
    FP_BEGIN(UserDiscntProc_pricing)
#endif	

 //加载求取用户账单流程-dingr
  ProcedureHelper::ExecuteProcedure("UserBillToDetailBillProc"); 
		
     	
  //用户优惠对象
	m_userDiscnt.initialize(m_pComBill->m_userId,
                           USER_ID_TYPE,
                           m_pRateInfo, 
                           m_pmManager,
                           m_pComBill);
  
#ifdef FP_DEBUG
    FP_BEGIN(UserDiscntProc_pricing_UserAcctTpProc)
#endif	 
   //取优惠信息（用户/用户群，系统资费，帐前调帐）
		ProcedureHelper::ExecuteProcedure("UserAcctTpProc"); //changed by dingr

#ifdef FP_DEBUG
    FP_END(UserDiscntProc_pricing_UserAcctTpProc)
#endif	
	
    m_userDiscnt.getDiscount(m_pComBill->m_discountVector);

#ifdef FP_DEBUG
    FP_BEGIN(UserDiscntProc_pricing_soluteDiscount)
#endif
    //执行用户优惠    
    if(!m_userDiscnt.soluteDiscount( ))
    {
			DEBUG_PRINT("[%s:%d] solute user [%s] discount error!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str());
	    return;	
    }
#ifdef FP_DEBUG
    FP_END(UserDiscntProc_pricing_soluteDiscount)
#endif 
                
    //更新用户账单
    updateBill();
    

   //如果存在群总量优惠仅仅25，26 事件能触发群总量优惠
     if(m_pComBill->m_isExistGrpTotalDiscnt&&
        (m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_GRPBILL||
         m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_ACCTBILL))                           	
    {
    	//取用户群明细账单
      //取总量优惠群的出帐用户外的其他用户的内存用户账单，添加到内部明细账单
      if(!m_acctBillMgr.getDetailBill(m_pComBill->m_userGrpId,
                                m_pComBill->m_userId,
			                          m_pComBill->m_userIdSet, 
			                          m_pComBill->m_detailBill))
		 {
				DEBUG_PRINT("[%s:%d] get group [%s] bill error!\n",__FILE__,__LINE__,m_pComBill->m_userGrpId.c_str());
	    	return;		
		 }
			     		
			     	     	
      //用户群优惠对象初始化
			m_userGroupDiscnt.initialize(m_pComBill->m_userGrpId, 
			                         		GROUP_ID_TYPE, 
			                         		m_pRateInfo,
			                         		m_pmManager ,
                               		m_pComBill);
        
			if(!m_pComBill->getUserGroupInfo(m_pComBill->m_userGrpId,m_pRateInfo->m_idInfo))
			{
			 	DEBUG_PRINT("[%s:%d] get user group %s info error!\n",__FILE__,__LINE__,m_pComBill->m_userGrpId.c_str());
	    	return;	
			}
			
      
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
                       
		}//如果存在群优惠
		
#ifdef FP_DEBUG
    FP_END(UserDiscntProc_pricing)
#endif	
}

//更新增量账单
//##ModelId=48AF623A0392
void UserDiscntProc::updateBill()
{
#ifdef FP_DEBUG
    FP_BEGIN(UserDiscntProc_updateBill)
#endif		
  ProcedureHelper::ExecuteProcedure("UpdateUserBillProc"); 	
  	
#ifdef DEBUG 	
    cout<<"************用户级优惠后******************"<<endl;	
 		cout<<"*************m_detailBill******************"<<endl;
 	 	cout<<m_pComBill->m_detailBill<<endl; 	 	 			
#endif 

#ifdef FP_DEBUG
    FP_END(UserDiscntProc_updateBill)
#endif	  		
}

//##ModelId=48D73C0F0119
int UserDiscntProc::execute()
{
try{
		if(!init())
			return 1; 	
		
		pricing();
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

