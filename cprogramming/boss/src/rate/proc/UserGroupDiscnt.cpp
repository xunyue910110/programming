#include "UserGroupDiscnt.h"

//##ModelId=48B21A0E03AD
UserGroupDiscnt::UserGroupDiscnt()
{
}

//##ModelId=48B21A0E03AE
UserGroupDiscnt::~UserGroupDiscnt()
{
}

//##ModelId=48B21A0E03AF
bool UserGroupDiscnt::initialize(const T_USER_ID &r_id,
	                              ID_TYPE r_idTypeCode,
	                              RateGlobalInfo *r_pRateInfo,
	                              PM_Account *r_pAccountParam,
	                              ComBill *r_pComBill)
{
  return DiscntBase::initialize(r_id,r_idTypeCode,r_pRateInfo,r_pAccountParam ,r_pComBill);

}


bool UserGroupDiscnt::soluteDiscount( )
{
	 int l_orderNo = 0;   
	 	    
	  //循环执行单个虚拟用户的资费构成
	  vector<Discount>::iterator iter;
	  for(iter=m_discountVector.begin();
	     iter!=m_discountVector.end();++iter)
	 {   
	 	    l_orderNo++;
	 	    T_TP_ID l_tpId = ((*iter).m_rateTpInfo).m_rateTpId;
	 	    m_pRateInfo->m_detailBillMgr.setDiscntInfo(l_tpId, (*iter).m_adjustInsId, l_orderNo);
	      
	     if(!m_pComBill->m_grpDiscntForAllUsers) //部分用户参与群总量优惠
	     {
	      	m_pComBill->m_detailBill.m_id=0;
	      	m_pComBill->m_detailBill.m_detailBillVector.clear();      	      	

	      	if(!m_acctBillMgr.getDetailBill(m_id,m_pComBill->m_userIdSet,m_pComBill->m_userGrpBills_new,m_pComBill->m_detailBill,iter->m_roleCode,iter->m_rateTpInfo.m_roleCodeVector,iter->m_rateTpInfo.m_roleCodeMap))
	      		continue;
	     }
#ifdef DEBUG 
      	 cout<<"****************资费开始前DetailBill*************"<<endl;   		 
    		 cout<<m_pComBill->m_detailBill<<endl;
    		 cout<<"*************资费开始前m_userBills_new******************"<<endl;
 				for(vector<UserBill>::iterator t_itera=m_pComBill->m_userGrpBills_new.begin();
 					t_itera!=m_pComBill->m_userGrpBills_new.end();++t_itera)
 					{
 	 	  			cout<<*t_itera<<endl; 
 					}  	
#endif	      
	      //设置账单
	  		m_pRateInfo->m_detailBillMgr.setUserDetailBill(&m_pComBill->m_detailBill);
	        
        if ((*iter).m_idTypeCode == DISCNT_TYPE) //优惠
        {
        	try{
         			//初始化资费信息 
	  		  	NodeBase::m_policyInfo.m_rateTpInfo = &iter->m_rateTpInfo;                                

						EventFeePolicy    *l_pEventTp = NULL;
						l_pEventTp = iter->m_rateTpInfo.m_pEventTp;
					
						if (l_pEventTp!=NULL) 
				 { 
           			l_pEventTp->execute();
           	       if((iter+1)==m_discountVector.end() || l_tpId!=((*(iter+1)).m_rateTpInfo).m_rateTpId ||
                   (l_tpId==((*(iter+1)).m_rateTpInfo).m_rateTpId && (*iter).m_discountId != (*(iter+1)).m_discountId))
              		{
                  		m_pRateInfo->m_detailBillMgr.updateCurFee();
              		}
       		#ifdef _DEBUG_
           	       cout<<"本条事件优惠后费用："<<"FEEPOLICY_ID="<<l_tpId<<",EventFeePolicy_ID="<<l_pEventTp->m_id<<","<<endl;
                   cout<<m_pComBill->m_detailBill<<endl;
           	 #endif
           	
           	        }
          	 }
         		 catch(exception &e)
						 {
#ifdef _DEBUG_		
							cout<<"["<<__FILE__<<":"<<__LINE__<<"]用户["<<m_id<<"]优惠错误!原因:"<<e.what()<<endl;		
#endif		
						 	m_pComBill->setErrorCode(m_id,E_USER_DISCNT_POLICY,"0",m_idTypeCode,(*iter).m_discountId);	
						 }  				                               
        }

      if(!m_pComBill->m_grpDiscntForAllUsers)  //部分用户参与群总量优惠                
      { //明细账单更新到群总量账单 
        m_acctBillMgr.updateUserBill(m_pComBill->m_detailBill,m_pComBill->m_userGrpBills_new);
      } 
      
#ifdef DEBUG 
      	 cout<<"****************资费结束后DetailBill********************"<<endl;   		 
    		 cout<<m_pComBill->m_detailBill<<endl;
    		 cout<<"*************资费结束后m_pComBill->m_userGrpBills_new******************"<<endl;
 				for(vector<UserBill>::iterator t_itera=m_pComBill->m_userGrpBills_new.begin();
 						t_itera!=m_pComBill->m_userGrpBills_new.end();++t_itera)
 					{
 	 	  			cout<<*t_itera<<endl; 
 					}  	
#endif	
        
   }//for
   
		if(!m_pComBill->m_grpDiscntForAllUsers)  //部分用户参与群总量优惠                
		{ //明细账单更新到群总量账单 
       m_acctBillMgr.getDetailBill(m_id,m_pComBill->m_userIdSet,m_pComBill->m_userGrpBills_new,m_pComBill->m_detailBill);
		} 
	 
	return true;
}








