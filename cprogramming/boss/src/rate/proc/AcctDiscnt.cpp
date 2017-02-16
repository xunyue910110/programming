#include "AcctDiscnt.h"

//##ModelId=48B21A0E0259
AcctDiscnt::AcctDiscnt()
{
}

//##ModelId=48B21A0E025A
AcctDiscnt::~AcctDiscnt()
{
}

//##ModelId=48B21A0E025B
bool AcctDiscnt::initialize(const T_USER_ID &r_id,
 	                           ID_TYPE r_idTypeCode,
 	                           RateGlobalInfo *r_pRateInfo,
 	                           PM_Account *r_pAccountParam,
 	                           ComBill *r_pComBill)
{
	if(!DiscntBase::initialize(r_id,r_idTypeCode,r_pRateInfo,r_pAccountParam ,r_pComBill))
	{	
		DEBUG_PRINT("initial DiscntBase error!\n");
		return false;
	}
		
	m_pComBill->initAcct(r_id);
		 	
	return true;
}


//取账户资费和帐前调帐
//##ModelId=48B21A0E027A
//bool AcctDiscnt::getDiscount(vector<Discount>     &r_discountVector)
//{
////  	bool l_flag = true;
////    m_userTpVector.clear();
////    char l_billType = (char)ACCT_ID_TYPE;
////     
////    //获取优惠信息
////    //获取帐户普通优惠
////    if(m_pAccountParam->getUserTp(m_id,NULL_PARTY,
////                                  m_cycParam.m_endTime,
////                                  m_userTpVector,l_flag,l_billType)<0)
////    {
////  	 	DEBUG_PRINT("[%s:%d] get account [%s]tariff policy error!\n",__FILE__,__LINE__,m_id.c_str());
////  	 	m_pComBill->setErrorCode(m_id,E_ACCT_GET_ACCTTP,"0",m_idTypeCode);
////     	return false;	
////    }  
////    
////    //取系统级（面向所有账户的）政策
////    vector<sysTariff> t_tmpbaseTpVec;
////    if (!m_pAccountParam->getBaseTp(BIZ_TYPE_ACCTDIS,
////                                 		m_cycParam.m_beginTime,
////                                 		m_cycParam.m_endTime,
////                                 		t_tmpbaseTpVec))
////    {
////  		DEBUG_PRINT("[%s:%d] get account [%s] system tariff policy error!\n",__FILE__,__LINE__,m_id.c_str());
////    }
////        
////     //将系统级政策加入m_userTpVector
////  	for(vector<sysTariff>::iterator t_baseTpItr = t_tmpbaseTpVec.begin();
////      	t_baseTpItr!=t_tmpbaseTpVec.end();++t_baseTpItr)
////  	{
////  			 if ((Integer)t_baseTpItr->m_homeAreaCode!=(Integer)(m_idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE])&&
////  			 	    t_baseTpItr->m_homeAreaCode[0]!='*')
////  			 			continue;
////  			 	
////    	   if ((Integer)m_idInfo.m_idAttr[IdInfo::ID_OPEN_DATE] > (Integer)t_baseTpItr->m_beginTime)
////        {
////           t_baseTpItr->m_beginTime = (Integer)m_idInfo.m_idAttr[IdInfo::ID_OPEN_DATE];
////        }
////        
////       	UserTp l_userTp;
////       	l_userTp.m_tpId     = t_baseTpItr->m_tpId;
////				l_userTp.m_beginTime= t_baseTpItr->m_beginTime;   
////				l_userTp.m_endTime  = t_baseTpItr->m_endTime;      
////        l_userTp.m_userId   = m_id;
////        l_userTp.m_idType   = (unsigned char)ACCT_ID_TYPE; //'2'
////       	m_userTpVector.push_back(l_userTp);
////  	}
////    
////    if (!DiscntBase::getDiscount())
////    	return false;
// //m_discountVector = r_discountVector;
// 	
// return true;
//}


//根据资费政策标识获取资费构成
//bool AcctDiscnt::getPolicyCompByTp()
//{
//	DiscountInfo    l_discntInfo;
//	RateTpInfo      l_rateTpInfo;
//  RateTpParam     l_rateTpParam;
//  FeePolicyComp  *l_pFeePolicyComp=NULL;
//  PriceParamImpl *l_pPriceParam=NULL;
//  int t_iRet = 0;
//
//  for(vector<UserTp>::iterator t_itr=m_userTpVector.begin();
//      t_itr!=m_userTpVector.end();++t_itr)
//  {
//    	
//    if(t_itr->m_idType!= (int)ACCT_ID_TYPE ||
//    	 !isFeePolicyValid(t_itr->m_beginTime,
//    	                   t_itr->m_endTime,
//    	                   m_cycParam))
//    	continue;
//   
//    t_iRet=m_pAccountParam->getDiscountInfo(t_itr->m_tpId,m_cycParam.m_endTime,l_discntInfo); 
//    if(t_iRet<0)
//    {
//#ifdef DEBUG
//        cout<<"资费参数未定义!资费ID="<<t_itr->m_tpId<<__FILE__<<__LINE__<<endl;
//#endif
//      m_pComBill->setErrorCode(m_id,E_ACCT_TP_DISCNT,"0",m_idTypeCode);
//      
//      //return false;
//      continue;     
//    }
//    else if (t_iRet==0)
//    {
//      if(  l_discntInfo.m_bdiscountId == 0 ||
//         !(l_discntInfo.m_discountType== DiscountInfo::DISCNT_RATE ||
//           l_discntInfo.m_discountType== DiscountInfo::DISCNT_COMMON ))
//      { //非批价模块使用的资费,则跳过
//        continue;
//      }
//    }
//    
//    t_itr->m_subTpType     = TP_TYPE_VPN;//账户也看做群
//     	 	
//    l_rateTpInfo.m_rateTpParam.clear();
//    l_rateTpInfo.m_pUserTp = &(*t_itr);
//    l_rateTpInfo.m_rateTpId= l_discntInfo.m_bdiscountId; // 计费资费ID
//    //1.根据资费政策标识求取资费构成标识
//    l_pFeePolicyComp=m_pAccountParam->getFeePolicy(l_rateTpInfo.m_rateTpId);
//    if(l_pFeePolicyComp==NULL)
//    {      
//      DEBUG_PRINT("[%s:%d]资费政策[%d]在资费构成表中无定义!\n",__FILE__,__LINE__,l_discntInfo.m_bdiscountId);
//      m_pComBill->setErrorCode(m_id,E_ACCT_TP_COMP,"0",m_idTypeCode);
//      continue;
//      //return false;
//    }
//      
//    //2.根据资费序号标识求取对应的参数信息
//    while(l_pFeePolicyComp!=NULL)
//    {
//      //作用角色过滤(角色相匹配或者统配才有效)
//      if(!(l_pFeePolicyComp->m_roleCode == ROLECODE_WIDECARD ||
//           l_pFeePolicyComp->m_roleCode == ROLECODE_SELF     ))
//      {
//        l_pFeePolicyComp = l_pFeePolicyComp->m_next;
//        continue;
//      }
//       
//       l_rateTpInfo.m_rateTpParam.clear();
//      l_rateTpInfo.m_rateTpNo     = l_pFeePolicyComp->m_compId;
//      l_rateTpInfo.m_rateRoleCode = l_pFeePolicyComp->m_roleCode; 
//      l_rateTpInfo.m_pEventTp   	= l_pFeePolicyComp->m_eventFeePolicy;
//      l_rateTpInfo.m_eventType  	= l_pFeePolicyComp->m_eventTypeId;
//      l_rateTpInfo.m_priority   	= l_pFeePolicyComp->m_priority;
//      l_rateTpInfo.m_compMethod 	= l_pFeePolicyComp->m_cdrCompFlag;
//        
//      //获取资费模板定义参数
//      l_pPriceParam = l_pFeePolicyComp->m_priceParamImpl;
//      while(l_pPriceParam!=NULL)
//      {
//        l_rateTpParam.m_rateTpNo    = l_pPriceParam->m_id;
//        l_rateTpParam.m_rateParamId = l_pPriceParam->m_priceParamId;
//        l_rateTpParam.m_source      = l_pPriceParam->m_implMethod;
//        l_rateTpParam.m_valueType   = l_pPriceParam->m_paramValueType;
//        l_rateTpParam.m_userParamId = l_pPriceParam->m_feePolicyParamId;
//        l_rateTpParam.m_value       = l_pPriceParam->m_fixedValue;
//        l_rateTpParam.m_pUserParam  = NULL;
//        //if(l_pPriceParam->m_implMethod == PARAM_SOURCE_CRM)//用户资料
//        if(l_pPriceParam->m_implMethod == PARAM_SOURCE_CRM1)//用户资料
//        {
//          T_DATE_TIME t_beginTime="19000101010101";
//          for(vector<UserTpParam>::iterator t_uTpParam =
//               t_itr->m_paramList.begin();
//              t_uTpParam!= t_itr->m_paramList.end();
//              ++t_uTpParam)
//          {
// 	  //取本月生效的参数
//				   		if(l_rateTpParam.m_userParamId == t_uTpParam->m_tpParamId&&
//				   			  t_uTpParam->m_beginTime <= m_cycParam.m_endTime &&
//				      		t_uTpParam->m_endTime >= m_cycParam.m_beginTime )
//                {
//                  	if (t_uTpParam->m_beginTime>t_beginTime)
//                  	{
//											t_beginTime = t_uTpParam->m_beginTime;
//      	        			l_rateTpParam.m_value      = t_uTpParam->m_tpValue;
//      	        			l_rateTpParam.m_pUserParam = &(*t_uTpParam);
//      	        		}
//      	    		}
//      	    }
//
//          if(l_rateTpParam.m_pUserParam==NULL)
//          {
//#ifdef DEBUG
//              cout<<"参数没有实例化：tpobjId="<<t_itr->m_tpObjId
//                  <<" tpNo = "<<l_pPriceParam->m_id
//                  <<" userParamId="<<l_pPriceParam->m_feePolicyParamId
//                  <<" "<<__FILE__<<__LINE__<<endl;
//#endif
//            
//           	m_pComBill->setErrorCode(m_id,E_ACCT_TP_PARAM,"0",m_idTypeCode);
//     		l_pPriceParam = l_pPriceParam->m_next; 
//            continue;
//            //return false;
//          }
//        } 
//        l_rateTpInfo.m_rateTpParam.push_back(l_rateTpParam);
//        l_pPriceParam = l_pPriceParam->m_next;
//      }
//       
////账户和用户优惠在资费实例表中不区分       
//      //为账户优惠事件
//      if(l_rateTpInfo.m_eventType == BIZ_TYPE_ACCTDIS||
//      	 l_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS||
//      	 l_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS2) //added on 20090505
//      {
//        if(!getEffectDiscntByTariffPolicy(t_itr->m_userId,
//      		                            l_rateTpInfo.m_rateRoleCode,
//      		                            (long)l_rateTpInfo.m_pUserTp->m_tpObjId, //资费构成->资费实例
//      		                            t_itr->m_beginTime,
//      		                            t_itr->m_endTime,
//      		                            l_rateTpInfo.m_priority,
//      		                            l_rateTpInfo,
//      		                            l_discntInfo.m_ruleId
//      		                            ))
//      	{
//      		DEBUG_PRINT("[%s:%d] get Discount by Traiff Comp error!\n",__FILE__,__LINE__);
//       		l_pFeePolicyComp = l_pFeePolicyComp->m_next;  
//       		continue;
//      	}
//        
//      }//if 账户优惠事件
//      
//      //处理下一条资费构成
//      l_pFeePolicyComp = l_pFeePolicyComp->m_next;  
//            
//    }//for 
//  }//for
//	
//	return true;
//}

//##ModelId=48B21A0E02F1
void AcctDiscnt::specialDeal()
{
}

//2009-9-9 15:55:43
//重载DiscntBase中的soluteDiscount
//账户级资费在执行某个资费前取明细账单
bool AcctDiscnt::soluteDiscount( )
{  	  
	  int l_orderNo = 0;   
	  
	  if (m_pComBill->m_acctDiscntForAllUsers)//所有用户参与账户优惠
	  {
	  		m_pComBill->m_detailBill.m_id=0;
	      m_pComBill->m_detailBill.m_detailBillVector.clear();      	      	

	     	m_acctBillMgr.getDetailBill(m_id,m_pRateInfo->m_accountBills_new,m_pComBill->m_detailBill,m_pRateInfo->m_idInfo,NULL);
	     
	  }	
	  
	  //循环执行单个账户的资费构成
	  vector<Discount>::iterator iter;
	  for(iter=m_discountVector.begin();
	     iter!=m_discountVector.end();++iter)
	 {   
	 	    l_orderNo++;
	 	    T_TP_ID l_tpId = ((*iter).m_rateTpInfo).m_rateTpId;
	 	    m_pRateInfo->m_detailBillMgr.setDiscntInfo(l_tpId, (*iter).m_adjustInsId, l_orderNo);
	      
	     //账户级优惠前的明细账单是否需要重新加载
			bool t_isLoadDetailBill=false;
			if ((!m_pComBill->m_acctDiscntForAllUsers) || iter->m_specDealTag == DEALTAG_LOADDETAILBILL_NOBILL)
			{
			    t_isLoadDetailBill=true;
			    m_pComBill->m_commonDealFlag=DEALTAG_LOADDETAILBILL_NOBILL;
			}
	      
	     if(t_isLoadDetailBill) //账户级优惠前的明细账单需要重新加载
	     {
	      	m_pComBill->m_detailBill.m_id=0;
	      	m_pComBill->m_detailBill.m_detailBillVector.clear();      	      	

	     	m_acctBillMgr.getDetailBill(m_id,m_pRateInfo->m_accountBills_new,m_pComBill->m_detailBill,m_pRateInfo->m_idInfo,&(*iter));
	      	
	     }
#ifdef DEBUG 
      	 cout<<"****************资费开始前DetailBill*************"<<endl;   		 
    		 cout<<m_pComBill->m_detailBill<<endl;
    		 cout<<"*************资费开始前m_accountBills_new******************"<<endl;
 				for(vector<AccountBill>::iterator t_itera=m_pRateInfo->m_accountBills_new.begin();
 					t_itera!=m_pRateInfo->m_accountBills_new.end();++t_itera)
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
        else if ((*iter).m_idTypeCode == ADJUST_BEFORE_TYPE) //调账
        {   
      		//作用时间点判断
    			//1:月底作用,实时不作用;  2:实时作用,月底不作用
    			if ((m_pComBill->m_isMonthEnd == false && (*iter).m_effectTimeTag == '1')||
    	  		  (m_pComBill->m_isMonthEnd == true && (*iter).m_effectTimeTag == '2'))
    			{  
        		continue;
    			}
        	        	  	
        	if(!m_adjustBeforeCalc.dealAdjust(*iter))
        	{
          	DEBUG_PRINT("[%s:%d] 处理[%s]帐前调帐错误!\n",__FILE__,__LINE__,m_id.c_str()); 
          	m_pComBill->setErrorCode(m_id,E_ACCT_DEAL_ADJUSTBEFORE,"0",m_idTypeCode);
          	continue;
        	}        
        }
        else if ((*iter).m_idTypeCode==ACCT_FEE_TRANS_TYPE) //账户间费用转移
        {
        	//TODO
        }
        else//特殊优惠处理
        {
          //TODO
        }
          
      if(!m_pComBill->m_acctDiscntForAllUsers)  //部分用户参与账户优惠                
      { //明细账单更新到账户账单 
       m_acctBillMgr.updateAccountBill(m_pComBill->m_detailBill,m_pRateInfo->m_accountBills_new,m_pRateInfo->m_idInfo);
      } 
#ifdef DEBUG 
      	 cout<<"****************资费结束后DetailBill*************"<<endl;   		 
    		 cout<<m_pComBill->m_detailBill<<endl;
    		 cout<<"*************资费结束后m_accountBills_new******************"<<endl;
 				for(vector<AccountBill>::iterator t_itera=m_pRateInfo->m_accountBills_new.begin();
 					t_itera!=m_pRateInfo->m_accountBills_new.end();++t_itera)
 					{
 	 	  			cout<<*t_itera<<endl; 
 					}  	
#endif	
        
   }//for
	
	
	 if(m_pComBill->m_acctDiscntForAllUsers)  //所有用户参与账户优惠                
   { //明细账单更新到账户账单 
       m_acctBillMgr.updateAccountBill(m_pComBill->m_detailBill,m_pRateInfo->m_accountBills_new,m_pRateInfo->m_idInfo);
   } 
   
	return true;
}
