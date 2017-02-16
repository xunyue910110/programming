#include "DiscntBase.h"
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "NodeBase.h"
#include "CondBase.h"

//##ModelId=48B21A0F00A2
DiscntBase::DiscntBase()
{
	m_pAccountParam = NULL;
	m_pComBill      = NULL;
	m_pRateInfo     = NULL;
}

//##ModelId=48B21A0F00A3
DiscntBase::~DiscntBase()
{
  m_discountVector.clear();
  m_adjustBeforeVector.clear();	
  m_userFeeSwaps.clear();
}


bool DiscntBase::initialize(const T_USER_ID &r_id,
	                          ID_TYPE r_idTypeCode,
	                          RateGlobalInfo *r_pRateInfo,
	                          PM_Account *r_pAccountParam,
	                          ComBill *r_pComBill)
{
    m_id 						= r_id;
    m_idTypeCode 		= r_idTypeCode;
    m_pAccountParam = r_pAccountParam;
    m_pComBill 			= r_pComBill;
    m_pRateInfo 		= r_pRateInfo;
    m_cycParam 			= m_pComBill->m_cycParam;
        	  
	  //设置账单
	  m_pRateInfo->m_detailBillMgr.setUserDetailBill(&m_pComBill->m_detailBill);
	      	  	  
	  //初始化帐前调帐对象	
 	  m_adjustBeforeCalc.initial(m_id,
 	  	                         m_idTypeCode,
	                             m_pAccountParam,
	                             m_pComBill->m_cycParam,
	                             m_pRateInfo,
	                             &m_pComBill->m_adjustBeforeVector
	                             );


	  m_discountVector.clear();
	  m_acctBillMgr.initialize(r_pRateInfo);

	return true;
}
 
 
bool DiscntBase::soluteDiscount( )
{  	  
	  int l_orderNo = 0;
	  
	  //循环执行单个用户的资费构成和帐前调整
	  vector<Discount>::iterator iter;
	  for(iter=m_discountVector.begin();
	     iter!=m_discountVector.end();++iter)
	 {   
	 	    l_orderNo++;
	 	    T_TP_ID l_tpId = ((*iter).m_rateTpInfo).m_rateTpId;
	 	    m_pRateInfo->m_detailBillMgr.setDiscntInfo(l_tpId, (*iter).m_adjustInsId, l_orderNo);
	        
        if ((*iter).m_idTypeCode == DISCNT_TYPE) //优惠
        {
        	try{
         			//初始化资费信息 
	  		  	NodeBase::m_policyInfo.m_rateTpInfo = &iter->m_rateTpInfo;                                

						EventFeePolicy    *l_pEventTp = NULL;
						l_pEventTp = iter->m_rateTpInfo.m_pEventTp;
					
						if (l_pEventTp!=NULL) 
       			{
/*       				
#ifdef _DEBUG_
   l_pEventTp->dump("discnt:"); 
#endif 
*/       				
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
        else if((*iter).m_idTypeCode == USER_FEE_TRANS_TYPE) //用户间费用转移
        {
        	//TODO
        	
        }
        else if ((*iter).m_idTypeCode==ACCT_FEE_TRANS_TYPE) //账户间费用转移
        {
        	//TODO
        }
        else//特殊优惠处理
        {
          //TODO
        }
   }//for
	
	return true;
}


//
///**getEffDiscntByAdjustBefore:根据帐前调整信息获取有效的优惠信息
// *@param  r_adjustBefore    帐前调整信息
// *@param  r_discnt          优惠信息
// *@return true:获取到有效的优惠信息  false:未获取到
// */
bool DiscntBase::getEffDiscntByAdjustBefore(const AdjustBefore& r_adjustBefore,Discount& r_discnt)
{
	  char        l_adjustModeType = '0';
    char        l_ifShowNegative = '0';
    int         l_priority = 0;
    char        l_effectTimeTag = '0';
    
    FDateTime l_DateTime;
    r_discnt.m_discountId     = r_adjustBefore.m_adjustReasonId;
    r_discnt.m_adjustInsId    = r_adjustBefore.m_adjustInsId;
    r_discnt.m_startCycId     = r_adjustBefore.m_startCycId;
    r_discnt.m_startDate      = l_DateTime.fullFillToMonthBegin(r_adjustBefore.m_startCycId);
    r_discnt.m_endDate        = l_DateTime.fullFillToMonthEnd(r_adjustBefore.m_endCycId);
    r_discnt.m_invalidStateId = 0;
    r_discnt.m_idTypeCode     = ADJUST_BEFORE_TYPE;
    
    m_pComBill->m_discntInfo = r_discnt;
    
    if (!m_pAccountParam->getAdjustReason(r_adjustBefore.m_adjustReasonId,
                                           l_adjustModeType, 
                                           l_priority, 
                                           l_ifShowNegative,
                                           l_effectTimeTag,
                                           r_discnt.m_ruleId))
    {
    	DEBUG_PRINT("[%s:%d] 获取调账原因[%d]错误!\n",__FILE__,__LINE__,r_adjustBefore.m_adjustReasonId);
    	m_pComBill->setErrorCode(m_id,E_ACCT_GET_ADJUSTREASON,"0",m_idTypeCode);
      return false;
    }

	//add by xuzy 2011.2.7
	if(r_discnt.m_userDiscntMode == USER_DISCNTMODE_NORMAL )//正常的用户级优惠顺序
	{
		 //账户优惠之后的账前调账在此不作处理 
		if(l_adjustModeType == '3') return false;
	}
	else//账户级优惠后的用户级优惠顺序
	{
		 //非账户优惠之后的账前调账在此不作处理 
		if(l_adjustModeType != '3') return false;
	}

     if(r_discnt.m_ruleId == DEFAULT_RULE_ID)
     {
     		r_discnt.m_condRule = NULL;
     }
     else
     {
      	r_discnt.m_condRule=m_pAccountParam->getCondition(r_discnt.m_ruleId);
    		if(r_discnt.m_condRule!=NULL)
    		{
    			if(!r_discnt.m_condRule->execute())
    			{
    			     return false;
    			}
        }	
    	}
    
    ObjectBase *l_pObjectBase=m_pAccountParam->getObjectByObjId(T_TP_ID(r_adjustBefore.m_effectObjectId));
	  if (l_pObjectBase==NULL)
	  {
			r_discnt.m_effectItem = r_adjustBefore.m_effectObjectId;
     	r_discnt.m_roleCode   = ROLECODE_SELF; 
	  }
	  else
	  {
	 		if (l_pObjectBase->m_attrType!='0')
	 		{
		  DEBUG_PRINT("[%s:%d] 对象表或调帐表配置错误，作用对象ID和作用帐目不一致!\n",__FILE__,__LINE__);
    	m_pComBill->setErrorCode(m_id,E_ACCT_OBJECT_ADJUST,"0",m_idTypeCode);
    	return false;		
	 		}
         
     	r_discnt.m_effectItem = l_pObjectBase->m_attrId;
     	r_discnt.m_roleCode   = l_pObjectBase->m_roleCode; 
    }  
    
    l_pObjectBase=m_pAccountParam->getObjectByObjId(T_TP_ID(r_adjustBefore.m_addupObjectId));
	  if (l_pObjectBase==NULL)
	  {
    		r_discnt.m_addupItem = r_adjustBefore.m_addupObjectId;
	   }
	   else
	   {
	 		if (l_pObjectBase->m_attrType!='1')
	 		{
		  	DEBUG_PRINT("[%s:%d] 对象表或调帐表配置错误，累计对象ID和累计帐目不一致!\n",__FILE__,__LINE__);
    		m_pComBill->setErrorCode(m_id,E_ACCT_OBJECT_ADJUST,"0",m_idTypeCode);
    		return false;		
	 		}
	 
	 		r_discnt.m_addupItem = l_pObjectBase->m_attrId;
 	  }//else
   
    //根据参考对象获取参考帐目
		l_pObjectBase=m_pAccountParam->getObjectByObjId(T_TP_ID(r_adjustBefore.m_referObjId));
	  if (l_pObjectBase==NULL)
	  {
			//r_discnt.m_referItem = r_adjustBefore.m_referObjId;
			r_discnt.m_referItem = r_discnt.m_effectItem; //无参考对象，默认为作用帐目 ModelId by xuf 2009-11-9 13:54:56
	  }
	  else
	  {
	 		if (l_pObjectBase->m_attrType!='0')
	 		{
		  	DEBUG_PRINT("[%s:%d] 对象表或调帐表配置错误，参考对象ID和参考帐目不一致!\n",__FILE__,__LINE__);
    		m_pComBill->setErrorCode(m_id,E_ACCT_OBJECT_ADJUST,"0",m_idTypeCode);
    		return false;		
	 		}
         
     	r_discnt.m_referItem = l_pObjectBase->m_attrId;
    }  
   
   
    r_discnt.m_adjustModeType = l_adjustModeType;
    r_discnt.m_priority       = l_priority;
    r_discnt.m_ifShowNegative = l_ifShowNegative;
    r_discnt.m_effectTimeTag  = l_effectTimeTag;
    
    return true;
}

bool  DiscntBase::getEffectDiscntByTariffPolicy(const T_SERVER_ID  &r_id,
                                							const T_MEMBER_TYPE &r_roleCode,
                                							const long  &r_tpObjId, 
                               								const T_DATE_TIME &r_beginTime, 
                              			 					const T_DATE_TIME &r_endTime, 
                               								const int  &r_priority,
                               								RateTpInfo r_rateTpInfo,
                               								DiscountInfo *r_discountInfo
                               								)
{
      		Discount     l_discount;
       	 	vector<Discount>::iterator  iterd;
    
        	l_discount.m_discountId = r_tpObjId;
        	l_discount.m_idTypeCode = DISCNT_TYPE;
        	l_discount.m_startDate  = r_beginTime;
    	  	l_discount.m_endDate    = r_endTime;
    	  	l_discount.m_priority   = r_priority;
    	    l_discount.m_rateTpInfo = r_rateTpInfo;
    	    l_discount.m_adjustInsId= 0;
    	    l_discount.m_ruleId			= 0;
    	    l_discount.m_feepolicyCompId = r_rateTpInfo.m_rateTpNo; //add for acctdiscnt 2009-9-11 20:27:28
    	    l_discount.m_roleCode   = r_roleCode;
			l_discount.m_specDealTag     = 0;
			if(r_discountInfo != NULL )
			{
				l_discount.m_ruleId			= r_discountInfo->m_ruleId;
				l_discount.m_specDealTag    = r_discountInfo->m_specDealTag;
			}
			if(l_discount.m_specDealTag == DEALTAG_LOADDETAILBILL_NOBILL)
			{
				m_pComBill->m_acctDiscntForAllUsers = false;
			}
    	    
    	    if(l_discount.m_ruleId	 == DEFAULT_RULE_ID)
    			{
      			l_discount.m_condRule = NULL;
    			}else
    			{
      			l_discount.m_condRule=m_pAccountParam->getCondition(l_discount.m_ruleId);
    				if(l_discount.m_condRule!=NULL)
    				{
    			     m_pComBill->m_acctDiscntForAllUsers = false;
        	 		}	
    			}
    	    
              
          //触发账户级优惠后的用户级优惠资费modify by xuzy 2011.2.7
	   if(l_discount.m_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS3)
          {
          	m_pComBill->m_userAfterAcctDiscntVec.push_back(l_discount);		
          }
	   else//触发非账户级优惠后的用户级优惠资费
	  {   //触发用户和资费实例属主非同一用户
    	  	if(m_id!=r_id)
    	  	{  
    	  		if (r_roleCode==ROLECODE_SELF||((r_roleCode>0||r_roleCode==ROLECODE_ENUM)&&r_rateTpInfo.m_execMode==EXEC_METHOD_GRPDISPATCH))//群总量优惠(分摊模式)
    	  		{ 	  			
    	  		    if (m_pComBill->m_userGroupIdSet.find(r_id)==m_pComBill->m_userGroupIdSet.end())
        	  		{
        	  			DEBUG_PRINT("[%s:%d] 资料错误!用户资费[%d]既不属于用户也不属于群组\n",__FILE__,__LINE__,(long)r_rateTpInfo.m_rateTpId);
        	  			m_pComBill->setErrorCode(m_id,E_ACCT_GET_ACCTTP,"0",m_idTypeCode);
        	  			return false;
        	  		}	
        	  		
        	  		if (r_rateTpInfo.m_execMode==EXEC_METHOD_GRPDISPATCH) //对部分角色用户总量优惠
        	  			 m_pComBill->m_grpDiscntForAllUsers = false;
        	  			 
        	  		//总量优惠群
        	  		m_pComBill->initUserGroup(r_id);
    
               	m_pComBill->m_userGrpDiscountVector.push_back(l_discount);	
               	
               				 	  		
    	  	  }
    	  	  else if ((r_roleCode==ROLECODE_WIDECARD)||
    	  	  	      ((r_roleCode>0||r_roleCode==ROLECODE_ENUM)&&r_rateTpInfo.m_execMode!=EXEC_METHOD_GRPDISPATCH)
    	  	  	       ) //账户或群下所有用户继承，由群下部分用户继承，
    	  	  {
    	  	 	  m_pComBill->m_discountVector.push_back(l_discount);
    	  	  } 
    	    }
    	    else//触发用户和资费实例属主是同一用户且自身或指定角色编码
    	    {    
    	    	   if ((r_rateTpInfo.m_pUserTp->m_idType==(char)USER_ID_TYPE&&
    	    	   	    m_pRateInfo->m_idInfo.m_idAttr[IdInfo::ID_TYPE_CODE][0]==(char)USER_ID_TYPE)|| //此处更改：普通用户资费无作用角色限制 Modified by jlxu@neimeng 2010-1-6 18:16:52
    	    	   	    (r_rateTpInfo.m_pUserTp->m_idType==(char)ACCT_ID_TYPE&&
    	  	          r_roleCode==ROLECODE_WIDECARD&&r_rateTpInfo.m_execMode!=EXEC_METHOD_ACCTPARTINHERIT))	//用户+资费为自身的（0或特殊的）,群/账户继承
    	    	   {	  	    
    	  	          m_pComBill->m_discountVector.push_back(l_discount);
    	  	     }    
    	  	     else if(r_rateTpInfo.m_pUserTp->m_idType==(char)ACCT_ID_TYPE&&
    	  	            r_roleCode==ROLECODE_SELF)              //账户优惠
    	  	      {  	               
    	  	         m_pComBill->m_acctDiscountVector.push_back(l_discount);
    	  	      }
    	  	      //虚拟用户单独触发群总量优惠
    	  		  else if (r_rateTpInfo.m_pUserTp->m_idType==(char)USER_ID_TYPE&&
    	  		          m_pRateInfo->m_idInfo.m_idAttr[IdInfo::ID_TYPE_CODE][0]==(char)GROUP_ID_TYPE&&
    	  		           m_pRateInfo->m_bizType==BIZ_TYPE_GRPBILL) //只有25事件才能触发 add by xuf 2010-3-2 22:39:46
    	  		 {
    	  			if (r_roleCode==ROLECODE_SELF||((r_roleCode>0||r_roleCode==ROLECODE_ENUM)&&r_rateTpInfo.m_execMode==EXEC_METHOD_GRPDISPATCH))//群总量优惠(分摊模式)
    	  			{
				//modify by xuf 20091122	 	  			        	  		
        	  	//	l_discount.m_rateTpInfo.m_pUserTp->m_idType = (char)GROUP_ID_TYPE;	
        	  		
        	  		if (r_rateTpInfo.m_execMode==EXEC_METHOD_GRPDISPATCH) //对部分角色用户总量优惠
        	  			 m_pComBill->m_grpDiscntForAllUsers = false;
        	  			 	
                m_pComBill->initUserGroup(m_id);
               	m_pComBill->m_userGrpDiscountVector.push_back(l_discount);	
               	        				 	  		
    	  	  	}
//    	  	  	else if ((r_roleCode==ROLECODE_WIDECARD&&r_rateTpInfo.m_pUserTp->m_idType==(char)USER_ID_TYPE)||((r_roleCode>0||r_roleCode==ROLECODE_ENUM)&&r_rateTpInfo.m_execMode!=EXEC_METHOD_GRPDISPATCH))//由群下用户继承 -- 此处不需要add by xuf 2010-3-2 22:40:19
//    	  	  	{
//    		  		//	l_discount.m_rateTpInfo.m_pUserTp->m_userId = m_id;
//    	  		 // 	l_discount.m_rateTpInfo.m_rateRoleCode      = ROLECODE_SELF; 
//    	  	 	  	m_pComBill->m_discountVector.push_back(l_discount);
//    	  	  	}
    	  	  	else if((m_pRateInfo->m_idInfo.m_idAttr[IdInfo::ID_TYPE_CODE][0]==(char)GROUP_ID_TYPE&&
    	  	         		 r_roleCode==ROLECODE_VIRTUAL))//虚拟用户+角色为-2
    	  	     {
    	  	        m_pComBill->m_discountVector.push_back(l_discount);
    	  	     }
    	  	 }
    	  	   else if ((r_roleCode==ROLECODE_WIDECARD&&r_rateTpInfo.m_execMode==EXEC_METHOD_ACCTPARTINHERIT&&
    	  	  	       r_rateTpInfo.m_pUserTp->m_idType==(char)ACCT_ID_TYPE&&
    	  	  	       m_acctBillMgr.getRoleBill(r_rateTpInfo.m_pUserTp->m_userId,l_discount.m_roleCode,ACCT_ID_TYPE)!=NULL&&
    	  	  	       m_pComBill->ifSpecialUserId(r_rateTpInfo.m_pUserTp->m_userId))
    	  	   	       )//由账户下USER_ID最小的用户（非停保）继承
    	  	   {
    	  	   	 m_pComBill->m_discountVector.push_back(l_discount);
    	  	   }         
    	    	}        	  	       	  	    
    	    }
    	    
	return true;
}

////modify 2009-8-3 10:01:17
bool DiscntBase::isFeePolicyValid(const T_DATE_TIME &r_beginTime,
  	                   						const T_DATE_TIME &r_endTime,
  	                   						const CycParam  &r_cycParam,
  	                   						const T_BIZ_TYPE &r_eventType)
{

 	if (r_eventType==BIZ_TYPE_USERDIS2)
	{
	 if(r_beginTime<r_cycParam.m_endTime&&
 	  	r_endTime>=r_cycParam.m_beginTime&&
 	  	r_beginTime<r_endTime)
 	  return true;
 return false;	
	}
 if(r_beginTime<r_cycParam.m_endTime&&
 	  r_endTime>=r_cycParam.m_endTime&&
 	  r_beginTime<r_endTime)
 	  return true; 	  
 return false;

}

bool DiscntBase::getDiscount(const vector<Discount>     &r_discountVector)
{
  m_discountVector = r_discountVector;
  
  return !(m_discountVector.empty()); 
}

void DiscntBase::getRateTpInfo(vector<UserTp> &t_userTpVector,const ID_TYPE &r_idType)
{
	vector<DiscountInfo>    t_discntInfo;
  RateTpInfo      t_rateTpInfo;
  RateTpParam     t_rateTpParam;
  int             t_rateTpNum = 0;
  const FeePolicyComp  *t_feePolicyComp=NULL;
  const PriceParamImpl *t_priceParam=NULL;
	int             t_iRet = 0; 
	T_TP_ID         t_tpId;
	char 						t_errMsg[256];
	
  for(vector<UserTp>::iterator t_itr=t_userTpVector.begin();
      t_itr!=t_userTpVector.end();++t_itr)
  {
  	//非用户级(或账户级）或无效资费
    if(t_itr->m_idType != r_idType)
     	continue;	    	
     	    	
    t_discntInfo.clear();   
    t_iRet=m_pAccountParam->getDiscountInfo(t_itr->m_tpId,t_discntInfo);
    if (t_iRet < 0)
    {
#ifdef _DEBUG_
    	cout<<__FILE__<<":"<<__LINE__<<",资费参数在td_b_discnt中未定义!资费ID="<<t_itr->m_tpId<<endl;
#endif
      m_pComBill->setErrorCode(m_id,E_ACCT_TP_DISCNT,"0",m_idTypeCode,(long)(t_itr->m_tpId));
      
      continue;
    }
    else if (t_iRet == 0)//作用时间不满足
     	continue;

    for (int idisc=0;idisc<t_iRet;++idisc)
    {  
      if( t_discntInfo[idisc].m_bdiscountId == 0 ||
         !(t_discntInfo[idisc].m_discountType== DiscountInfo::DISCNT_RATE ||
           t_discntInfo[idisc].m_discountType== DiscountInfo::DISCNT_COMMON ))
      { //非批价模块使用的资费,则跳过
        continue;
      }
      
		t_tpId = t_discntInfo[idisc].m_bdiscountId;

         
    t_itr->m_subTpType    = TP_TYPE_VPN;
    t_rateTpInfo.m_rateTpParam.clear();
    t_rateTpInfo.m_pUserTp = &(*t_itr);
    t_rateTpInfo.m_rateTpId= t_tpId; // 资费ID
    //1.根据资费政策标识求取资费构成标识
    t_feePolicyComp = m_pAccountParam->getFeePolicy(t_tpId);
    if(t_feePolicyComp == NULL)
    {
#ifdef _DEBUG_
        cout<<__FILE__<<":"<<__LINE__<<",批价资费构成未定义!资费ID="<<t_tpId<<endl;
#endif
      m_pComBill->setErrorCode(m_id,E_ACCT_TP_COMP,"0",m_idTypeCode,(long)(t_tpId));
      continue;
    } 
        
    //2.根据资费序号标识求取对应的参数信息
    while(t_feePolicyComp!=NULL)
    { 
      //系统条件满足条件，除部分资费外，其他所有资费不作用。add by xuf 2010-1-7 23:41:35
      // modify by daizq@20101125 不在排除的资费列表， 也不在排除的事件资费列表里 ,批价时不执行该事件资费
      if(m_pComBill->m_needGetPartTp)
      {
    	  strstream ss;
        string t_strTp;
        ss << t_itr->m_tpId;
        ss >> t_strTp;
      			
        if(!binary_search(m_pAccountParam->m_exTpVec.begin(),m_pAccountParam->m_exTpVec.end(),t_strTp))
        {
      				strstream ss1;
       				string t_strEventTp;
       				ss1 << t_feePolicyComp->m_eventPolicyId;
       				ss1>> t_strEventTp;
       			
      		  	if(!binary_search(m_pAccountParam->m_exEventTpVec.begin(),m_pAccountParam->m_exEventTpVec.end(),t_strEventTp))
      		  	{
      					#ifdef _DEBUG_
      						cout<<__FILE__<<__LINE__<<" event_feepolicy_id = "<<t_feePolicyComp->m_eventPolicyId<<"满足 DISCNT_INVALID_COND， 不执行"<<endl;
      					#endif
      		  		t_feePolicyComp = t_feePolicyComp->m_next;
      		  		continue;
      		  	}
        }		      	  	 	
      }    	
    	
    	t_rateTpInfo.m_rateTpParam.clear();
      t_rateTpInfo.m_eventType= t_feePolicyComp->m_eventTypeId;
      //非帐务优惠的,validFlag无效的跳过
      if( t_itr->m_validFlag == false &&
        !(t_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS ||
          t_rateTpInfo.m_eventType == BIZ_TYPE_ACCTDIS  ||
          t_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS2 ||
          t_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS3 )) //added on 20090505
      {
        t_feePolicyComp = t_feePolicyComp->m_next;
        continue;
      }
         
      t_rateTpInfo.m_rateTpNo     = t_feePolicyComp->m_compId;
      t_rateTpInfo.m_rateRoleCode = t_feePolicyComp->m_roleCode;
      t_rateTpInfo.m_roleCodeVector = t_feePolicyComp->m_roleCodeVector;
      t_rateTpInfo.m_roleCodeMap = t_feePolicyComp->m_roleCodeMap;
     
      //仅仅角色大于0，或枚举角色， 才取 add by xuf 2009-11-17 8:40:38
      if (t_itr->m_idType==(char)USER_ID_TYPE&&
      	  (t_rateTpInfo.m_rateRoleCode.value()>0||
		  		t_rateTpInfo.m_rateRoleCode==ROLECODE_ENUM))
      {
      	if (m_pComBill->m_groupUserIdMap.find(t_itr->m_userId)==m_pComBill->m_groupUserIdMap.end())	
     		{
      		//增加调用取成员信息
  					if(!m_pComBill->GetUserInfoByGrpId(t_itr->m_userId))
  					{
#ifdef _DEBUG_
	    		cout<<__FILE__<<":"<<__LINE__<<"GetUserInfoByGrpId false!userId="<<t_itr->m_userId<<endl;
#endif
		 					m_pComBill->setErrorCode(m_pComBill->m_userId,E_ACCT_GET_GROUPTP,"0",USER_ID_TYPE,t_itr->m_userId.value());  	
  					}
     		}	
      }
      
      //作用角色过滤(角色相匹配或者统配才有效)
      if(!(t_rateTpInfo.m_rateRoleCode == ROLECODE_WIDECARD ||
           t_rateTpInfo.m_rateRoleCode == ROLECODE_SELF     ||
           (t_itr->m_idType==(char)USER_ID_TYPE&&
           ((m_pRateInfo->m_idInfo.m_idAttr[IdInfo::ID_TYPE_CODE][0]==(char)USER_ID_TYPE&&
           	 ((t_rateTpInfo.m_rateRoleCode==ROLECODE_ENUM&&m_pComBill->judgeRoleCodeIn(t_rateTpInfo.m_roleCodeVector,t_rateTpInfo.m_roleCodeMap,m_pComBill->m_groupUserIdMap[t_itr->m_userId][m_id].m_memberRole) )
           	  ||t_rateTpInfo.m_rateRoleCode== m_pComBill->m_groupUserIdMap[t_itr->m_userId][m_id].m_memberRole))||
              (m_pRateInfo->m_idInfo.m_idAttr[IdInfo::ID_TYPE_CODE][0]==(char)GROUP_ID_TYPE&&(t_rateTpInfo.m_rateRoleCode >ROLECODE_SELF||t_rateTpInfo.m_rateRoleCode==ROLECODE_ENUM) )
           	)))) //modify 20090917 ))
      {
        t_feePolicyComp = t_feePolicyComp->m_next;
        continue;
      }
      
      t_rateTpInfo.m_pEventTp 	= t_feePolicyComp->m_eventFeePolicy;
      t_rateTpInfo.m_priority 	= t_feePolicyComp->m_priority;
      t_rateTpInfo.m_compMethod	=	t_feePolicyComp->m_cdrCompFlag;
      t_rateTpInfo.m_execMode  	=	t_feePolicyComp->m_execMode;
      
      //获取资费模板定义参数
      t_priceParam = t_feePolicyComp->m_priceParamImpl;
      while(t_priceParam!=NULL)
      {
        t_rateTpParam.m_rateTpNo    = t_priceParam->m_id;
        t_rateTpParam.m_rateParamId = t_priceParam->m_priceParamId;
        t_rateTpParam.m_source      = t_priceParam->m_implMethod;
        t_rateTpParam.m_valueType   = t_priceParam->m_paramValueType;
        t_rateTpParam.m_userParamId = t_priceParam->m_feePolicyParamId;
        t_rateTpParam.m_value       = t_priceParam->m_fixedValue;
        t_rateTpParam.m_pUserParam  = NULL;
        //if(t_priceParam->m_implMethod == PARAM_SOURCE_CRM)//用户资料
        //if(t_priceParam->m_implMethod == PARAM_SOURCE_CRM1)//用户资料
        if(t_priceParam->m_implMethod == PARAM_SOURCE_CRM1 ||
                (t_priceParam->m_implMethod == PARAM_SOURCE_CRM2))//用户资料

        {
         T_DATE_TIME t_beginTime="19000101010101";
                            
          for(vector<UserTpParam>::iterator t_uTpParam =
               t_itr->m_paramList.begin();
              t_uTpParam!= t_itr->m_paramList.end();
              ++t_uTpParam)
          {
          	if(t_rateTpParam.m_userParamId == t_uTpParam->m_tpParamId)
          	{    
          	    //取本月生效的参数
				  		if(t_uTpParam->m_beginTime <= m_cycParam.m_endTime &&
				  			 t_uTpParam->m_endTime >= m_cycParam.m_beginTime 
				  			 )
          	  {
          	    if (t_uTpParam->m_beginTime>t_beginTime)
          	    {
          	    	t_beginTime = t_uTpParam->m_beginTime;
          	    	t_rateTpParam.m_value = t_uTpParam->m_tpValue;
									t_rateTpParam.m_pUserParam = &(*t_uTpParam);
          	    }
          	  }                     
				   	}  
          }//for
          
          
          if(t_rateTpParam.m_pUserParam==NULL)
          {
#ifdef _DEBUG_
             cout<<__FILE__<<":"<<__LINE__<<",参数没有实例化：tpobjId="<<t_itr->m_tpObjId
                  <<" tpNo = "<<t_priceParam->m_id
                  <<" userParamId="<<t_priceParam->m_feePolicyParamId<<endl;
#endif
            m_pComBill->setErrorCode(m_id,E_ACCT_TP_PARAM,"0",m_idTypeCode,(long)(t_itr->m_tpObjId));
            t_priceParam = t_priceParam->m_next;
            continue;
          }
        } 
        else if(m_pRateInfo->m_bizType!=BIZ_TYPE_ACCTBILL&&
        	      (t_priceParam->m_implMethod == PARAM_SOURCE_USERITEM||
        	      t_priceParam->m_implMethod == PARAM_SOURCE_USERITEM_VPN))//通过用户属性表获取参数
        {
            	if (t_priceParam->m_implMethod == PARAM_SOURCE_USERITEM)//用户自身属性
            	{
            	    m_pRateInfo->m_idInfo.init(m_cycParam.m_beginTime,m_cycParam.m_endTime,t_rateTpInfo.m_eventType);
            	    const char *t_pStrValue = m_idInfo.getParamValue((long)t_rateTpParam.m_userParamId);
            	    t_rateTpParam.m_value = atol(t_pStrValue);
				    			if(t_pStrValue==NULL)
                    {
                     #ifdef _DEBUG_
                       cout<<"参数没有实例化：tpobjId="<<t_itr->m_tpObjId
                           <<" tpNo = "<<t_priceParam->m_id
                           <<" userParamId="<<t_priceParam->m_feePolicyParamId
                           <<" "<<__FILE__<<__LINE__<<endl;
                     #endif
                     sprintf(t_errMsg,"参数没有实例化：tpobjId=%ld,tpNo=%d,userParamId=%d",
                                       long(t_itr->m_tpObjId),t_priceParam->m_id,
                                       t_priceParam->m_feePolicyParamId);
                     m_pRateInfo->setErrInfo(E_USER_TPPARAM,t_errMsg);
                     t_priceParam = t_priceParam->m_next;
                     continue;// 记录错单，批价，帐务不互相影响
                    }
              }
           		else if(t_priceParam->m_implMethod == PARAM_SOURCE_USERITEM_VPN) //虚拟用户属性
           		{
           			if(m_pRateInfo->m_grpIdInfo.m_idAttr[IdInfo::ID_ATTR_ID]!=t_rateTpInfo.m_pUserTp->m_userId)
           			{
           				if(!m_pComBill->getUserGroupInfo(t_rateTpInfo.m_pUserTp->m_userId,m_pRateInfo->m_grpIdInfo))
           				{	
           					t_priceParam = t_priceParam->m_next;
           					continue;
           	  		}	        	  
           			}	
           			//非虚拟用户
           			if (m_pRateInfo->m_grpIdInfo.m_idAttr[IdInfo::ID_TYPE_CODE][0]!=(char)GROUP_ID_TYPE)
           			{
#ifdef _DEBUG_
                	cout<<t_rateTpInfo.m_pUserTp->m_userId<<"非虚拟用户!["<<__FILE__<<":"<<__LINE__<<"]"<<endl;
#endif
           		  	t_priceParam = t_priceParam->m_next;
           				continue;
           			}	
           	
           			m_pRateInfo->m_grpIdInfo.init(m_cycParam.m_beginTime,m_cycParam.m_endTime,t_rateTpInfo.m_eventType);
         				const char *t_pStrValue = m_pRateInfo->m_grpIdInfo.getParamValue((long)t_rateTpParam.m_userParamId);
            		t_rateTpParam.m_value = atol(t_pStrValue);
            
            		if (t_pStrValue==NULL)
            		{
            	 		#ifdef _DEBUG_
                    cout<<"参数没有实例化：tpobjId="<<t_itr->m_tpObjId
                        <<" tpNo = "<<t_priceParam->m_id
                        <<" userParamId="<<t_priceParam->m_feePolicyParamId
                        <<" "<<__FILE__<<__LINE__<<endl;
              		#endif
              		sprintf(t_errMsg,"参数没有实例化：tpobjId=%ld,tpNo=%d,userParamId=%d",
                                    long(t_itr->m_tpObjId),t_priceParam->m_id,
                                    t_priceParam->m_feePolicyParamId);
              			m_pRateInfo->setErrInfo(E_USER_TPPARAM,t_errMsg);
              			t_priceParam = t_priceParam->m_next;
              			continue;// 记录错单，批价，帐务不互相影响		
            		}
           		}
        }
        t_rateTpInfo.m_rateTpParam.push_back(t_rateTpParam);
        t_priceParam = t_priceParam->m_next;
      }
         
      //为帐务处理事件
      if(t_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS ||
         t_rateTpInfo.m_eventType == BIZ_TYPE_ACCTDIS ||
         t_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS2||
         t_rateTpInfo.m_eventType == BIZ_TYPE_USERDIS3  ) //added on 20090505
      {
        //if(本月有效)
        if (isFeePolicyValid(t_itr->m_beginTime,
    	                      t_itr->m_endTime,
    	                      m_cycParam,
							  						t_rateTpInfo.m_eventType))
        { 
      		if(!getEffectDiscntByTariffPolicy(t_rateTpInfo.m_pUserTp->m_userId,
      		                            			t_rateTpInfo.m_rateRoleCode,
      		                            			(long)t_rateTpInfo.m_pUserTp->m_tpObjId, //资费构成->资费实例
      		                            			t_rateTpInfo.m_pUserTp->m_beginTime,
      		                            			t_rateTpInfo.m_pUserTp->m_endTime,
      		                            			t_rateTpInfo.m_priority,
      		                            			t_rateTpInfo,
      		                            			&(t_discntInfo[idisc])
      		                            			))
      		 {
      		   DEBUG_PRINT("[%s:%d] get Discount by Traiff Comp [%d] error!\n",__FILE__,__LINE__,(long)t_rateTpInfo.m_rateTpNo);
      		   t_feePolicyComp = t_feePolicyComp->m_next;
      		   continue;
      		 }
         
        }
      }
      
      t_feePolicyComp = t_feePolicyComp->m_next;
    }//while
    }//for (int idisc=0;idisc<t_iRet;++idisc) 	
  }//for
  
}

//##ModelId=48B21A0F011C
void DiscntBase::specialDeal()
{
	//TODO
}
//

//add by xuf 2009-11-14 17:46:33 由于批价的参数求取已经从PM 移到 TpProc
void DiscntBase::setTpParamInfo(vector<UserTp> &r_userTpVector)
{
  vector<PO_UserTpParam_def*> t_userTpParamDefs;

  //1. 设置政策参数中的额外属性
  for(vector<UserTp>::iterator t_tpItr=r_userTpVector.begin();
      t_tpItr!=r_userTpVector.end();++t_tpItr)
  {
      //2.求取资费政策参数定义
    t_userTpParamDefs.clear();
    if(m_pAccountParam->getUserTpParamDefInfo(t_tpItr->m_tpId,t_userTpParamDefs)==true)
    { //有用户资费参数定义，需要将用户资费参数实例求取出来
      //3.求取用户资费参数实例
      if(m_pAccountParam->getUserTpParam(t_tpItr->m_tpObjId,t_tpItr->m_paramList)<0)
      {
#ifdef _DEBUG_
				cout<<"["<<__FILE__<<","<<__LINE__<<"]"<<"资费参数没有实例化"<<endl;
#endif        
        m_pComBill->setErrorCode(t_tpItr->m_userId,E_ACCT_TP_PARAM,"0",ACCT_ID_TYPE);
        continue;
      }
      
      for(vector<PO_UserTpParam_def*>::iterator r_paramDefItr=t_userTpParamDefs.begin();
          r_paramDefItr!=t_userTpParamDefs.end();++r_paramDefItr)
      {
        //4.根据资费政策参数定义设置用户资费参数实例一些属性
        for(vector<UserTpParam>::iterator r_paramItr=t_tpItr->m_paramList.begin();
            r_paramItr!=t_tpItr->m_paramList.end();++r_paramItr)
        {
          if(r_paramItr->m_tpParamId == (*r_paramDefItr)->m_record.m_tpParamId )
          {
            r_paramItr->m_relatinType = (*r_paramDefItr)->m_record.m_relatinType;
            r_paramItr->m_valueType   = (*r_paramDefItr)->m_record.m_valueType;
            break;
          }
        }

      }
    }
  }
  
}

