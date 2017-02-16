#include "UserDiscnt.h"

//##ModelId=48B21DB4002C
UserDiscnt::UserDiscnt()
{
	m_idTypeCode = USER_ID_TYPE;
}

//##ModelId=48B21DB4002D
UserDiscnt::~UserDiscnt()
{
}

//##ModelId=48B21DB4002E
bool UserDiscnt::initialize(const T_USER_ID &r_id,
	                          ID_TYPE r_idTypeCode,
	                          RateGlobalInfo *r_pRateInfo,
	                          PM_Account *r_pAccountParam,
	                          ComBill *r_pComBill)
{
	if(!DiscntBase::initialize(r_id,r_idTypeCode,r_pRateInfo,r_pAccountParam ,r_pComBill))
		return false;
	return true;
}

//取用户和关联群资费/调帐
//bool UserDiscnt::getDiscount(vector<Discount>     &r_discountVector)
//{	 
//	//  m_tpInfoList.clear();
//	//  m_pAcctTpList.clear();
//    //取群资费（总量群资费和个人用户挂在群上的资费）	
//	//	getAcctGrpTp();
//   
//   
//  // if(!DiscntBase::getDiscount())
//   //	return false;
//
////		if(!getUserFeeTrans());
////    	return false;
//
//		m_discountVector = r_discountVector;
//#ifdef _DEBUG_
//	for(vector<Discount>::iterator t_it = m_discountVector.begin();
//		  t_it != m_discountVector.end();++t_it )
//		  cout<<*t_it<<endl;
//#endif
//       	
//	return  true;
//	
//}

////获取资费构成
//bool UserDiscnt::getPolicyCompByTp()
//{	   
//		int l_bRet = false;
//		vector<int>::iterator t_IndexItr1;
//		vector<int> *l_pAcctTpList= &m_pRateInfo->m_pAcctTpList;
//		for(t_IndexItr1 = l_pAcctTpList->begin();
//				t_IndexItr1!= l_pAcctTpList->end();
//				t_IndexItr1++)
//	 {
//	 		RateTpInfo &t_rateTp = (m_pRateInfo->m_tpInfoList)[*t_IndexItr1];
//	 		 //为帐务处理事件
//       if(t_rateTp.m_eventType == BIZ_TYPE_USERDIS||
//       		t_rateTp.m_eventType == BIZ_TYPE_ACCTDIS||
//       		t_rateTp.m_eventType == BIZ_TYPE_USERDIS2)//added on 20090505
//       { 
//      		l_bRet = true;  
//      		if(!getEffectDiscntByTariffPolicy(t_rateTp.m_pUserTp->m_userId,
//      		                            t_rateTp.m_rateRoleCode,
//      		                            (long)t_rateTp.m_pUserTp->m_tpObjId, //资费构成->资费实例
//      		                            t_rateTp.m_pUserTp->m_beginTime,
//      		                            t_rateTp.m_pUserTp->m_endTime,
//      		                            t_rateTp.m_priority,
//      		                            t_rateTp
//      		                            ))
//      		 {
//      		   DEBUG_PRINT("[%s:%d] get Discount by Traiff Comp [%d] error!\n",__FILE__,__LINE__,(long)t_rateTp.m_rateTpNo);
//      		   continue;
//      		 }
//      	}//if 帐务事件
//	 	
//	 }//for 	
//     	      
//	 
//	 //取群资费构成
//	  l_pAcctTpList= &m_pAcctTpList;
//		for(t_IndexItr1 = l_pAcctTpList->begin();
//				t_IndexItr1!= l_pAcctTpList->end();
//				t_IndexItr1++)
//	 {
//	 		RateTpInfo &t_rateTp = (m_tpInfoList)[*t_IndexItr1];
//	 		 //为帐务处理事件
//       if(t_rateTp.m_eventType == BIZ_TYPE_USERDIS||
//          t_rateTp.m_eventType == BIZ_TYPE_USERDIS2||
//       		t_rateTp.m_eventType == BIZ_TYPE_ACCTDIS)
//       { 
//      		l_bRet = true;  
//      		if(!getEffectDiscntByTariffPolicy(t_rateTp.m_pUserTp->m_userId,
//      		                            t_rateTp.m_rateRoleCode,
//      		                            (long)t_rateTp.m_pUserTp->m_tpObjId, //资费构成->资费实例
//      		                            t_rateTp.m_pUserTp->m_beginTime,
//      		                            t_rateTp.m_pUserTp->m_endTime,
//      		                            t_rateTp.m_priority,
//      		                            t_rateTp
//      		                            ))
//      		 {
//      		   DEBUG_PRINT("[%s:%d] get Discount by Traiff Comp [%d] error!\n",__FILE__,__LINE__,(long)t_rateTp.m_rateTpNo);
//      		   continue;
//      		 }
//      	}//if 帐务事件
//	 	
//	 }//for 	
//	 
//     	      
//		return l_bRet;
//}

//取用户间费用转移
bool UserDiscnt::getUserFeeTrans()
{
	  
    //获取帐前调整信息
    if (m_pAccountParam->getUserFeeSwap(m_id,
                            m_pComBill->m_cycleId,
                            m_userFeeSwaps)<0)
    {
    	DEBUG_PRINT("get user:[%s] feeSwap error!\n",m_id.c_str());
    	m_pComBill->setErrorCode(m_id,E_ACCT_GET_BILLSWAP,"0",m_idTypeCode);
     	return false;		
    }	
    
    	                               
    vector<UserFeeSwap>::iterator itera=m_userFeeSwaps.begin();
    if (itera!=m_userFeeSwaps.end())
    {
    	Discount   l_discount;
    	FDateTime l_DateTime;
    	l_discount.m_discountId     = 0;
    	l_discount.m_adjustInsId    = 0;
    	l_discount.m_startCycId     = itera->m_startCycId;
    	l_discount.m_startDate      = l_DateTime.fullFillToMonthBegin(itera->m_startCycId);
    	l_discount.m_endDate        = l_DateTime.fullFillToMonthEnd(itera->m_endCycId);
    	l_discount.m_invalidStateId = 0;
    	l_discount.m_idTypeCode     = USER_FEE_TRANS_TYPE;
    	l_discount.m_adjustModeType = '0';
    	l_discount.m_priority       = 9999;
    	l_discount.m_ifShowNegative = '0';
    	l_discount.m_effectTimeTag  = '0';
         
        //排重插入
        vector<Discount>::iterator iterd; 
        for (iterd=m_discountVector.begin();
             iterd!=m_discountVector.end(); iterd++)
        {
            if ((iterd->m_discountId == l_discount.m_discountId &&
            		 l_discount.m_discountId!=0)&& 
                iterd->m_idTypeCode == l_discount.m_idTypeCode)
            {
                break;
            }
        }

        if (iterd == m_discountVector.end())
        {
            m_discountVector.push_back(l_discount);
        }

    }

  //优惠排序
    ::partial_sort (m_discountVector.begin(),m_discountVector.end(),
                    m_discountVector.end());
                    
    return true;
}

//bool UserDiscnt::getAcctGrpTp()
//{
//	if(m_pComBill->m_userGroupIdSet.empty())
//	{
//		return true;	
//	}
//		
//	bool l_flag=true;
//	m_userTpVector.clear();
//	//1. 取群政策
//	for (set<T_USER_ID>::iterator l_it = m_pComBill->m_userGroupIdSet.begin();  
//		  l_it != m_pComBill->m_userGroupIdSet.end();++l_it)
//	{
//  	if(m_pAccountParam->getUserTp(*l_it,
//                           		 		NULL_PARTY,
//                            			m_cycParam.m_endTime,
//                            			m_userTpVector,
//                            			l_flag)<0)
//		{//本条资费错误，记录错误信息，取下一条资费
//#ifdef _DEBUG_
//	    cout<<__FILE__<<":"<<__LINE__<<"getUserTp false!userId="<<*l_it<<endl;
//#endif
//			m_pComBill->setErrorCode(m_id,E_ACCT_GET_GROUPTP,"0",m_idTypeCode,atol((*l_it).c_str()));
//	  	continue;
//		}
//		
//		//增加调用取成员信息
//  if(!m_pComBill->GetUserInfoByGrpId(*l_it))
//  {//
//#ifdef _DEBUG_
//	    cout<<__FILE__<<":"<<__LINE__<<"GetUserInfoByGrpId false!userId="<<*l_it<<endl;
//#endif
//			m_pComBill->setErrorCode(m_id,E_ACCT_GET_GROUPTP,"0",m_idTypeCode,atol((*l_it).c_str()));  	
//   	continue;
//  }
//
//	
//	}
// 
//	if(m_userTpVector.empty())
//	{
// 		return true;;	
//	}
// 	
// 	//2.把帐务群资费放入m_pRateInfo->m_tpinfolist
//  getAcctTpInfo();
//
//#ifdef _DEBUG_	
//  cout<<"------------帐务群批价政策列表-----------"<<endl;
//  vector<int>::const_iterator t_idxItr;
//  for(t_idxItr = m_pAcctTpList.begin();t_idxItr!=m_pAcctTpList.end();
//      ++t_idxItr)
//  {
//    cout<<m_tpInfoList[*t_idxItr]<<endl;
//  }
//  cout<<"-------------------------------------------"<<endl;	
//#endif
//	
//	return true;
//}
//
//bool UserDiscnt::getAcctTpInfo()
//{
//  DiscountInfo    t_discntInfo;
//  RateTpInfo      t_rateTpInfo;
//  RateTpParam     t_rateTpParam;
//  int             t_rateTpNum = 0;
//  const FeePolicyComp  *t_feePolicyComp=NULL;
//  const PriceParamImpl *t_priceParam=NULL;
//  
//  for(vector<UserTp>::iterator t_itr=m_userTpVector.begin();
//      t_itr!=m_userTpVector.end();++t_itr)
//  {
//  	//非用户级或无效资费
//    if(t_itr->m_idType != USER_ID_TYPE||
//    	!isFeePolicyValid(t_itr->m_beginTime,
//    	                  t_itr->m_endTime,
//    	                  m_cycParam))
//     	continue;
//       
//    if(m_pAccountParam->getDiscountInfo(t_itr->m_tpId,t_discntInfo)==false)
//    {
//#ifdef _DEBUG_
//    	cout<<__FILE__<<":"<<__LINE__<<",资费参数在td_b_discnt中未定义!资费ID="<<t_itr->m_tpId<<endl;
//#endif
//      m_pComBill->setErrorCode(m_id,E_ACCT_TP_DISCNT,"0",m_idTypeCode,(long)(t_itr->m_tpId));
//      
//      continue;
//    }else
//    {
//      if( t_discntInfo.m_bdiscountId == 0 ||
//         !(t_discntInfo.m_discountType== DiscountInfo::DISCNT_RATE ||
//           t_discntInfo.m_discountType== DiscountInfo::DISCNT_COMMON ))
//      { //非批价模块使用的资费,则跳过
//        continue;
//      }
//    }
//    
//    t_rateTpInfo.m_rateTpParam.clear();
//    t_rateTpInfo.m_pUserTp = &(*t_itr);
//    t_rateTpInfo.m_rateTpId= t_itr->m_tpId; // 资费ID
//    //1.根据资费政策标识求取资费构成标识
//    t_feePolicyComp = m_pAccountParam->getFeePolicy(t_discntInfo.m_bdiscountId);
//    if(t_feePolicyComp == NULL)
//    {
//#ifdef _DEBUG_
//        cout<<__FILE__<<":"<<__LINE__<<",批价资费构成未定义!资费ID="<<t_itr->m_tpId<<endl;
//#endif
//      m_pComBill->setErrorCode(m_id,E_ACCT_TP_COMP,"0",m_idTypeCode,(long)(t_itr->m_tpId));
//      continue;
//    } 
//        
//    //2.根据资费序号标识求取对应的参数信息
//    while(t_feePolicyComp!=NULL)
//    { 
//    	t_rateTpInfo.m_rateTpParam.clear();
//      t_rateTpInfo.m_eventType= t_feePolicyComp->m_eventTypeId;
//      //非帐务优惠的,validFlag无效的跳过
//      if( t_itr->m_validFlag == false &&
//        !(t_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS ||
//          t_rateTpInfo.m_eventType == BIZ_TYPE_ACCTDIS  ||
//          t_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS2 )) //added on 20090505
//      {
//        t_feePolicyComp = t_feePolicyComp->m_next;
//        continue;
//      }
//         
//      t_rateTpInfo.m_rateTpNo     = t_feePolicyComp->m_compId;
//      t_rateTpInfo.m_rateRoleCode = t_feePolicyComp->m_roleCode;
//      //作用角色过滤(角色相匹配或者统配才有效)
//      if(!(t_rateTpInfo.m_rateRoleCode == ROLECODE_WIDECARD ||
//           t_rateTpInfo.m_rateRoleCode == ROLECODE_SELF     ||
//           t_rateTpInfo.m_rateRoleCode == m_pComBill->m_groupUserIdMap[t_itr->m_userId][m_id].m_memberRole)) //modify 20090917 ))
//      {
//        t_feePolicyComp = t_feePolicyComp->m_next;
//        continue;
//      }
//      t_rateTpInfo.m_pEventTp 	= t_feePolicyComp->m_eventFeePolicy;
//      t_rateTpInfo.m_priority 	= t_feePolicyComp->m_priority;
//      t_rateTpInfo.m_compMethod	=	t_feePolicyComp->m_cdrCompFlag;
//      t_rateTpInfo.m_execMode  	=	t_feePolicyComp->m_execMode;
//      
//      //获取资费模板定义参数
//      t_priceParam = t_feePolicyComp->m_priceParamImpl;
//      while(t_priceParam!=NULL)
//      {
//        t_rateTpParam.m_rateTpNo    = t_priceParam->m_id;
//        t_rateTpParam.m_rateParamId = t_priceParam->m_priceParamId;
//        t_rateTpParam.m_source      = t_priceParam->m_implMethod;
//        t_rateTpParam.m_valueType   = t_priceParam->m_paramValueType;
//        t_rateTpParam.m_userParamId = t_priceParam->m_feePolicyParamId;
//        t_rateTpParam.m_value       = t_priceParam->m_fixedValue;
//        t_rateTpParam.m_pUserParam  = NULL;
//        //if(t_priceParam->m_implMethod == PARAM_SOURCE_CRM)//用户资料
//        if(t_priceParam->m_implMethod == PARAM_SOURCE_CRM1)//用户资料
//        {
//					T_DATE_TIME t_beginTime="19000101010101";
//                            
//          for(vector<UserTpParam>::iterator t_uTpParam =
//               t_itr->m_paramList.begin();
//              t_uTpParam!= t_itr->m_paramList.end();
//              ++t_uTpParam)
//          {
//          	if(t_rateTpParam.m_userParamId == t_uTpParam->m_tpParamId)
//          	{    
//          	    //取本月生效的参数
//				  		if(t_uTpParam->m_beginTime <= m_cycParam.m_endTime &&
//				  			 t_uTpParam->m_endTime >= m_cycParam.m_beginTime 
//				  			 )
//          	  {
//          	    if (t_uTpParam->m_beginTime>t_beginTime)
//          	    {
//          	    	t_beginTime = t_uTpParam->m_beginTime;
//          	    	t_rateTpParam.m_value = t_uTpParam->m_tpValue;
//									t_rateTpParam.m_pUserParam = &(*t_uTpParam);
//          	    }
//          	  }                     
//				   	}  
//          }//for
//          
//          
//          if(t_rateTpParam.m_pUserParam==NULL)
//          {
//#ifdef _DEBUG_
//             cout<<__FILE__<<":"<<__LINE__<<",参数没有实例化：tpobjId="<<t_itr->m_tpObjId
//                  <<" tpNo = "<<t_priceParam->m_id
//                  <<" userParamId="<<t_priceParam->m_feePolicyParamId<<endl;
//#endif
//            m_pComBill->setErrorCode(m_id,E_ACCT_TP_PARAM,"0",m_idTypeCode,(long)(t_itr->m_tpObjId));
//            t_priceParam = t_priceParam->m_next;
//            continue;
//          }
//        } 
//        t_rateTpInfo.m_rateTpParam.push_back(t_rateTpParam);
//        t_priceParam = t_priceParam->m_next;
//      }
//         
//      //为帐务处理事件
//      if(t_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS ||
//         t_rateTpInfo.m_eventType == BIZ_TYPE_ACCTDIS ||
//         t_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS2 ) //added on 20090505
//      {
//        //if(本月有效)
//        if(isFeePolicyValid(t_itr->m_beginTime,
//    	                      t_itr->m_endTime,
//    	                      m_cycParam,
//							  t_rateTpInfo.m_eventType))
//        {
//        	vector<RateTpInfo>::iterator t_iter;
//        	for(t_iter = m_tpInfoList.begin();
//        			t_iter != m_tpInfoList.end();++t_iter)
//        {
//                //资费实例已取
//        		if (t_iter->m_pUserTp->m_tpObjId==t_rateTpInfo.m_pUserTp->m_tpObjId&&
//        			  t_iter->m_rateTpNo==t_rateTpInfo.m_rateTpNo)
//        			break;
//        }
//        	
//        	if(t_iter == m_tpInfoList.end())
//        	{
//        		m_tpInfoList.push_back(t_rateTpInfo);  
//        		m_pAcctTpList.push_back(t_rateTpNum); 
//          	++t_rateTpNum;
//        	}
//        }
//      }
//      
//      t_feePolicyComp = t_feePolicyComp->m_next;
//    }//while
//   	
//  }//for
//  //对m_pAcctTpList进行排序
//  ::sort(m_pAcctTpList.begin(),m_pAcctTpList.end(),*(m_pRateInfo->m_cmpRateTpIndex));	
//	
//	return true;
//}
//
