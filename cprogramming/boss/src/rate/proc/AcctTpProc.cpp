#include "AcctTpProc.h"
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"
#include "CondBase.h"
//#include "AccountCommon.h"
#include "DiscntBase.h"
#include "base/TimeUtil.h"
#include "PO_FeepolicyMutexRela.h"

INSTALL_PROCEDURE_INSTANCE(UserAcctTpProc);
INSTALL_PROCEDURE_INSTANCE(UserGrpTpProc);
INSTALL_PROCEDURE_INSTANCE(AccountTpProc);
INSTALL_PROCEDURE_INSTANCE(UserAfterAcctTpProc);//add by xuzy 2011.2.7

//求用户级资费
int UserAcctTpProc::execute()
{
	try
	{
		m_pComBill=&m_pRateInfo->m_comBill;

    if (!m_pComBill->m_ifExistUserInfo)
    	return 1;
    	
		//获取用户及用户群优惠资费
		if(!getTpInfo())
		{
			DEBUG_PRINT("[%s:%d] get user:[%s] getUserTpInfo error!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str()); 
			return 1; 

		}  
		
#ifdef _DEBUG_
	cout<<"用户级资费:"<<endl;
	for(vector<Discount>::iterator t_it = m_pComBill->m_discountVector.begin();
		  t_it != m_pComBill->m_discountVector.end();++t_it )
		  cout<<*t_it<<endl;	  
#endif
		
	}	
	catch(exception &e)
 	{
#ifdef _DEBUG_		
		cout<<"["<<__FILE__<<":"<<__LINE__<<"]用户优惠资费求取错误!原因:"<<e.what()<<endl;
#endif		 	
  		theErrorMessages->insert(E_USER_DISCNT_PARAM, m_pComBill->m_userId + ": user discount  error! error reason= " + string(e.what()) ); 
  		return 1;
  	}	
				
		return 0;
}


bool UserAcctTpProc::getTpInfo()
{
	//根据用户取资费和帐前调帐
	m_pComBill->m_discountVector.clear();
	m_pComBill->m_adjustBeforeVector.clear();
	
	vector<Discount>::iterator  iterd;

	//成员变量初始化
   m_userDiscnt.initialize(m_pComBill->m_userId,
                           USER_ID_TYPE,
                           m_pRateInfo, 
                           m_pmManager,
                           m_pComBill);
    //根据资费获取资费构成 	
	
	  bool t_needGetTp = true;
	  m_pComBill->m_needGetPartTp = false; //需要获取部分资费ID add by xuf 2010-1-7 21:13:01
	  
	   //优惠在某些状态下全失效modify by xuf 2009-12-29 19:15:22
   	//CondBase *l_pCondBase = m_pmManager->getCondition(CONDID_STATE_DISCNT_INVALID);
   	if(m_pmManager->m_pCondBaseDiscntInValid!=NULL)
    {
    	if(m_pmManager->m_pCondBaseDiscntInValid->execute())
    	{  
    		if ((m_pmManager->m_exTpVec.empty())&&(m_pmManager->m_exEventTpVec.empty())) 		
             t_needGetTp = false;
        else 
        	  m_pComBill->m_needGetPartTp = true;
        	       
    	}
   	}
	
     if(t_needGetTp)
     {	
     	getPolicyCompByTp();  
     	
     	ProcedureHelper::ExecuteProcedure("UserGrpTpProc"); 
     } 
     	//取总量优惠群下用户
			if(!m_pComBill->getTotalDiscntGroupUsers())
			{
				DEBUG_PRINT("[%s:%d] 不存在群总量优惠用户!\n"__FILE__,__LINE__);	
				return 1;
			}
	
	    	 
    	m_pComBill->getUserPayInfo(m_pComBill->m_userIdSet);	
    		
     if(t_needGetTp)
     {		
     	ProcedureHelper::ExecuteProcedure("AccountTpProc"); 	
     }	
     
//#ifdef  __PROV019__  //如果是山西		 
    //add by cuiyw at 2009-12-13 22:05:06 
    //优惠排序
    ::partial_sort (m_pComBill->m_discountVector.begin(),m_pComBill->m_discountVector.end(),
                   m_pComBill->m_discountVector.end());

    vector<PO_FeepolicyMutexRela > t_mutexTp;
    vector <Discount> t_tmpDiscountVector;
    set   <T_TP_ID> t_DeleteTp;   
    bool t_isEedFollow = false;
    t_mutexTp.clear();
    t_DeleteTp.clear();
    t_tmpDiscountVector.clear();   

    t_tmpDiscountVector.swap(m_pComBill->m_discountVector);

    for(vector <Discount> ::iterator t_mutexiter = t_tmpDiscountVector.begin();
        t_mutexiter!= t_tmpDiscountVector.end();++t_mutexiter )
    {   	
    	  //如果找不到与其互斥的优先级高的资费
        if(m_pRateInfo->m_paramMgr->getFeepolicyMutex(((*t_mutexiter).m_rateTpInfo).m_rateTpId,t_mutexTp)==0)
        {
            m_pComBill->m_discountVector.push_back(*t_mutexiter);
        }
        else//如果有互斥资费
        {
        	  t_isEedFollow = false; 
        	  
            //循环比当前资费优先级高的资费，看是否与当前资费互斥
            for(vector <Discount> ::iterator iters = m_pComBill->m_discountVector.begin();
                                iters != m_pComBill->m_discountVector.end();++iters)
            {  
    	  		      
    	  		      PO_FeepolicyMutexRela t_mutexTpPo;
    	  		      t_mutexTpPo.m_record.m_lowpriorId=((*t_mutexiter).m_rateTpInfo).m_rateTpId;  
    	  		      t_mutexTpPo.m_record.m_highpriorId=((*iters).m_rateTpInfo).m_rateTpId;  
    	  		      
    	  		      //存在优先级高的资费互斥 
    	  		      vector<PO_FeepolicyMutexRela >::iterator t_mutexTpItr;
    	  		      t_mutexTpItr=lower_bound(t_mutexTp.begin(),t_mutexTp.end(),t_mutexTpPo);	

#ifdef DEBUG
		cout<<"((*iters).m_rateTpInfo).m_rateTpId:"<<((*iters).m_rateTpInfo).m_rateTpId<<endl;
		cout<<"t_mutexTpPo.m_record.m_lowpriorId:"<<t_mutexTpPo.m_record.m_lowpriorId<<endl;
		cout<<"t_mutexTpPo:"<<endl;
		for(vector<PO_FeepolicyMutexRela >::iterator t_itr=t_mutexTp.begin();
			 t_itr!=t_mutexTp.end();t_itr++)
		{
		  	cout<<(*t_itr)<<endl;
		}
		
		if(t_mutexTpItr!=t_mutexTp.end())
			cout<<"t_mutexTpItr:"<<*t_mutexTpItr<<endl;
#endif      	  		     
    	  		     
    	  		      if (t_mutexTpItr!=t_mutexTp.end()&&(*t_mutexTpItr==t_mutexTpPo))	
    	  		      {
    	  		      	   //当前资费加入到删除列表
    	  		      	   if(t_DeleteTp.find(t_mutexiter->m_discountId) == t_DeleteTp.end())
                      {
                            t_DeleteTp.insert(t_mutexiter->m_discountId);
                      }
                     
                     //A互斥B，如果A资费结束时间在本帐期，把A的结束时间+1天作为B的起始时间
                    if ((*t_mutexTpItr).m_record.m_mutexType == MUTEX_TYPE_ENDFOLLOW)
                    {    
                    	//资费结束时间在本帐期
                        if(((*iters).m_rateTpInfo).m_pUserTp->m_endTime<m_pRateInfo->m_pSysCycParam->m_endTime)
                        {
                            char t_strBeginTime[8+1];
                            memset(t_strBeginTime,0,9);
                            //资费结束时间+1天
                            TimeUtil::addDays(t_strBeginTime,((*iters).m_rateTpInfo).m_pUserTp->m_endTime.substr(0,8).c_str(),1);

                            ((*t_mutexiter).m_rateTpInfo).m_pUserTp->m_beginTime = (string(t_strBeginTime)+string("000000"));
                             t_isEedFollow = true;
                        }
                    } 
                        
                    break; //只要存在一条互斥 ，本资费即被互斥掉，无需再循环  
    	  		  }  

    	  		}//for 
    	  		
    	  		//如果资费未被互斥,添加到资费列表 
    	  		if(t_DeleteTp.find(t_mutexiter->m_discountId) == t_DeleteTp.end())
           {
            m_pComBill->m_discountVector.push_back(*t_mutexiter);
           }
           else  if (t_isEedFollow) //或者本月与其互斥的资费月中失效，本资费从互斥资费结束日+1 开始生效
           {
            m_pComBill->m_discountVector.push_back(*t_mutexiter);
           }
        }          
    }                                                 
    //add by cuiye end
//#endif		 
	 
	 
    Discount     l_discount;

   l_discount.m_userDiscntMode = USER_DISCNTMODE_NORMAL;//标识非账户级优惠后的用户级优惠处理   add by xuzy 2011.2.7

    //获取帐前调整信息
    if (m_pmManager->getAdjustBefore(m_pComBill->m_userId,
                                     USER_ID_TYPE, 
                                     m_pComBill->m_cycleId, 
                                     m_pComBill->m_adjustBeforeVector)<0)
    {
    		DEBUG_PRINT("[%s:%d] get user:[%s] adjust berore error!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str());
    		m_pComBill->setErrorCode(m_pComBill->m_userId,E_ACCT_GET_ADJUSTBEFORE,"0",USER_ID_TYPE);
     		return false;		
    }	
    		

    
    for (vector<AdjustBefore>::iterator itera=m_pComBill->m_adjustBeforeVector.begin();
         itera!=m_pComBill->m_adjustBeforeVector.end(); itera++)
    {
   	  if ( !m_userDiscnt.getEffDiscntByAdjustBefore(*itera, l_discount) )
        {
            continue;
        }
            
        //排重插入
        for (iterd=m_pComBill->m_discountVector.begin();
             iterd!=m_pComBill->m_discountVector.end(); iterd++)
        {
          	if (iterd->m_adjustInsId == l_discount.m_adjustInsId &&
                iterd->m_idTypeCode == l_discount.m_idTypeCode)
             {
                break;
             }
        }

        if (iterd == m_pComBill->m_discountVector.end())
        {
            m_pComBill->m_discountVector.push_back(l_discount);
        }
    }

    //优惠排序
    ::partial_sort (m_pComBill->m_discountVector.begin(),m_pComBill->m_discountVector.end(),
                   m_pComBill->m_discountVector.end());

	return true;
}

//获取资费构成
bool UserAcctTpProc::getPolicyCompByTp()
{	   
		int l_bRet = false;
		vector<int>::iterator t_IndexItr1;
		vector<int> *l_pAcctTpList= &m_pRateInfo->m_pAcctTpList;
		for(t_IndexItr1 = l_pAcctTpList->begin();
				t_IndexItr1!= l_pAcctTpList->end();
				t_IndexItr1++)
	 {
	 		RateTpInfo &t_rateTp = (m_pRateInfo->m_tpInfoList)[*t_IndexItr1];
	 		 //为帐务处理事件
      #ifdef _DEBUG_
      	cout<<__FILE__<<__LINE__<<" event_feepolicy_id = "<<t_rateTp.m_pEventTp->m_id<<endl;
      #endif
       
       if(t_rateTp.m_eventType == BIZ_TYPE_USERDIS||
       		t_rateTp.m_eventType == BIZ_TYPE_ACCTDIS||
       		t_rateTp.m_eventType == BIZ_TYPE_USERDIS2)//added on 20090505
       { 
      		l_bRet = true;  
      		

      		
      		
      		//需要获取部分资费 add by xuf 2010-1-7 21:18:08
      		if (m_pComBill->m_needGetPartTp)
      		{     			
      			strstream ss;
       			string t_strTp;
       			ss << t_rateTp.m_pUserTp->m_tpId;
       			ss >> t_strTp;
      			// modify by daizq@20100714 不在排除的资费列表， 
      			// 也不在排除的事件资费列表里,批价时不执行该事件资费。
      		  if(!binary_search(m_pmManager->m_exTpVec.begin(),m_pmManager->m_exTpVec.end(),t_strTp))
      		  {
      				strstream ss1;
       				string t_strEventTp;
       				ss1 << t_rateTp.m_pEventTp->m_id;
       				ss1>> t_strEventTp;
       			
      		  	if(!binary_search(m_pmManager->m_exEventTpVec.begin(),m_pmManager->m_exEventTpVec.end(),t_strEventTp))
      		  	{
      					#ifdef _DEBUG_
      						cout<<__FILE__<<__LINE__<<" event_feepolicy_id = "<<t_rateTp.m_pEventTp->m_id<<"满足 DISCNT_INVALID_COND， 不执行"<<endl;
      					#endif
      		  		continue;
      		  	} 

      		  }
      		  
      			      		  		  		
      		}	
      		     		
      		if(!m_userDiscnt.getEffectDiscntByTariffPolicy(t_rateTp.m_pUserTp->m_userId,
      		                            t_rateTp.m_rateRoleCode,
      		                            (long)t_rateTp.m_pUserTp->m_tpObjId, //资费构成->资费实例
      		                            t_rateTp.m_pUserTp->m_beginTime,
      		                            t_rateTp.m_pUserTp->m_endTime,
      		                            t_rateTp.m_priority,
      		                            t_rateTp
      		                            ))
      		 {
      		   DEBUG_PRINT("[%s:%d] get Discount by Traiff Comp [%d] error!\n",__FILE__,__LINE__,(long)t_rateTp.m_rateTpNo);
      		   continue;
      		 }
       }//if 帐务事件
	 	
	 }//for 	
     	         	      
		return l_bRet;
}

//求取用户群资费
int UserGrpTpProc::execute()
{

	try
	{
		m_pComBill=&m_pRateInfo->m_comBill;

    if (!m_pComBill->m_ifExistUserInfo)
    	return 1;
    	
		//获取用户及用户群优惠资费
		if(!getTpInfo())
		{
			return 1; 
		}
   		
	}	
	catch(exception &e)
 	{
#ifdef _DEBUG_		
		cout<<"["<<__FILE__<<":"<<__LINE__<<"]用户优惠资费求取错误!原因:"<<e.what()<<endl;
#endif		 	
  		theErrorMessages->insert(E_USER_DISCNT_PARAM, m_pComBill->m_userId + ": user discount  error! error reason= " + string(e.what()) ); 
  		return 1;
  	}	
	
#ifdef _DEBUG_		  
	cout<<"用户群资费:"<<endl;
		for(vector<Discount>::iterator t_it = m_pComBill->m_userGrpDiscountVector.begin();
		  t_it != m_pComBill->m_userGrpDiscountVector.end();++t_it )
		  cout<<*t_it<<endl;		  		  
#endif	
				
		return 0;
}

bool UserGrpTpProc::getTpInfo()
{
	if(m_pComBill->m_userGroupIdSet.empty())
	{
		return true;	
	}
			
	bool l_flag=true;
 //根据用户取资费和帐前调帐
	m_pComBill->m_userGrpDiscountVector.clear(); 
	vector<Discount>::iterator  iterd;

	m_pComBill->m_userTpVector.clear();
	int t_iRet = 0;
	//1. 取群政策
	for (set<T_USER_ID>::iterator l_it = m_pComBill->m_userGroupIdSet.begin();  
		  l_it != m_pComBill->m_userGroupIdSet.end();++l_it)
	{
  	if((t_iRet=m_pComBill->m_pAccountParam->getUserTp(*l_it,
                           		 		NULL_PARTY,
                            			m_pComBill->m_cycParam.m_endTime,
                            			m_pComBill->m_userTpVector,
                            			l_flag))<0)
		{//本条资费错误，记录错误信息，取下一条资费
#ifdef _DEBUG_
	    cout<<__FILE__<<":"<<__LINE__<<"getUserTp false!userId="<<*l_it<<endl;
#endif
			m_pComBill->setErrorCode(m_pComBill->m_userId,E_ACCT_GET_GROUPTP,"0",USER_ID_TYPE,atol((*l_it).c_str()));
	  	continue;
		}
		
//		if (t_iRet==0)
//		{
//			continue;	
//		}
		
//		//增加调用取成员信息
//  	if(!m_pComBill->GetUserInfoByGrpId(*l_it))
//  	{
//#ifdef _DEBUG_
//	    cout<<__FILE__<<":"<<__LINE__<<"GetUserInfoByGrpId false!userId="<<*l_it<<endl;
//#endif
//		 m_pComBill->setErrorCode(m_pComBill->m_userId,E_ACCT_GET_GROUPTP,"0",USER_ID_TYPE,atol((*l_it).c_str()));  	
//  	}

	}
 
	if(m_pComBill->m_userTpVector.empty())
	{
 		return true;;	
	}
	  //成员变量初始化
   m_userDiscnt.initialize(m_pComBill->m_userId,
                           USER_ID_TYPE,
                           m_pRateInfo, 
                           m_pmManager,
                           m_pComBill);	
 	
		//取参数信息、参数定义
		m_userDiscnt.setTpParamInfo(m_pComBill->m_userTpVector);
 	
 		//2.取群资费信息
  	m_userDiscnt.getRateTpInfo(m_pComBill->m_userTpVector,USER_ID_TYPE);

    ::partial_sort (m_pComBill->m_userGrpDiscountVector.begin(),m_pComBill->m_userGrpDiscountVector.end(),
                    m_pComBill->m_userGrpDiscountVector.end());	
	return true;
}

//求取帐户用户资费
int AccountTpProc::execute()
{

	try
	{
		m_pComBill=&m_pRateInfo->m_comBill;

     if (!m_pComBill->m_ifExistUserInfo)
    	return 1;
    	
		//获取账户级优惠
		if(!getTpInfo())
		{
			DEBUG_PRINT("[%s:%d] get user:[%s] tpinfo error!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str()); 
			return 1; 

		}
					
	}
	
	catch(exception &e)
 	{
		#ifdef DEBUG		
		cout<<"["<<__FILE__<<":"<<__LINE__<<"]帐户优惠资费求取错误!原因:"<<e.what()<<endl;
		#endif		 	
  		theErrorMessages->insert(E_USER_DISCNT_PARAM, m_pComBill->m_userId + ": user discount  error! error reason= " + string(e.what()) ); 
  		return 1;
  	}	

#ifdef _DEBUG_		  
	cout<<"账户级资费:"<<endl;
	for(map<T_ACCT_ID,vector<Discount> >::iterator t_it = m_pComBill->m_acctsDiscountMap.begin();
		  t_it != m_pComBill->m_acctsDiscountMap.end();++t_it)
		 {
		 for(vector<Discount>::iterator t_itr = t_it->second.begin();
		  t_itr != t_it->second.end();++t_itr )
		  cout<<*t_itr<<endl;	   	
		 } 		  
#endif			
	
		return 0;
}

bool AccountTpProc::getTpInfo()
{
	  m_pComBill->m_acctsDiscountMap.clear();  
	  set<T_ACCT_ID> t_acctIds;
	  t_acctIds.clear();
	  t_acctIds.swap(m_pComBill->m_acctIds);
	  m_pComBill->m_isExistAcctDiscnt=false;
	  m_pComBill->m_acctTpMap.clear();

	  if (t_acctIds.empty())
	  {
	  	return false;	
	  }
	  
	  T_SERVER_ID t_id;
	  		  
	  //循环执行各账户优惠 
		for (set<T_ACCT_ID>::iterator iter=t_acctIds.begin();
         iter!=t_acctIds.end();iter++)
		{
        	t_id=*iter;
        	bool l_flag = true;
    			char l_billType = (char)ACCT_ID_TYPE;
	   			m_userTpVector.clear();
					m_pComBill->m_acctDiscountVector.clear();

					vector<Discount>::iterator  iterd;

	       if(!m_acctDiscnt.initialize(t_id,ACCT_ID_TYPE,m_pRateInfo,m_pmManager ,m_pComBill))
     		{
     			theErrorMessages->insert(E_INITIALIZE,t_id+" initialize account discount error ! account: " ); 
     			DEBUG_PRINT("帐户[%s]优惠初始化失败!",t_id.c_str());
     			continue;
   		   }	

    		//获取帐户普通优惠
    		if(m_pmManager->getUserTp(t_id,NULL_PARTY,
                                  m_pComBill->m_cycParam.m_endTime,
                                  m_userTpVector,l_flag,l_billType)<0)
   		  {
  	 		 	DEBUG_PRINT("[%s:%d] get account [%s]tariff policy error!\n",__FILE__,__LINE__,t_id.c_str());
  	 		 	m_pComBill->setErrorCode(t_id,E_ACCT_GET_ACCTTP,"0",ACCT_ID_TYPE);
     			return false;	
  		  }  
        
        if(!m_userTpVector.empty())
        {
        	//取参数信息、参数定义
  				m_acctDiscnt.setTpParamInfo(m_userTpVector);
  			
        	m_pComBill->m_acctTpMap.insert(make_pair<T_ACCT_ID,vector<UserTp> >(t_id,m_userTpVector));
         
   
       		//根据取到的资费实例绑定资费
        	//m_acctDiscnt.getRateTpInfo(m_userTpVector,ACCT_ID_TYPE);
         	m_acctDiscnt.getRateTpInfo(m_pComBill->m_acctTpMap[t_id],ACCT_ID_TYPE);
        }
                 
       Discount     l_discount;
       vector <AdjustBefore> t_adjustBefores;
    //获取帐前调整信息
    if (m_pmManager->getAdjustBefore(t_id,
                                     ACCT_ID_TYPE, 
                                     m_pComBill->m_cycleId, 
                                     t_adjustBefores)<0)
    {
    		DEBUG_PRINT("[%s:%d] get user:[%s] adjust before error!\n",__FILE__,__LINE__,t_id.c_str());
    		m_pComBill->setErrorCode(t_id,E_ACCT_GET_ADJUSTBEFORE,"0",ACCT_ID_TYPE);
     		return false;		
    }	
    
                                
    
    for (vector<AdjustBefore>::iterator itera=t_adjustBefores.begin();
         itera!=t_adjustBefores.end(); itera++)
    {
        if ( !m_acctDiscnt.getEffDiscntByAdjustBefore(*itera, l_discount) )
        {
            continue;
        }
         
         //作用角色过滤(角色相匹配或者统配才有效)
    		if(!(l_discount.m_roleCode   == ROLECODE_WIDECARD ||
         		 l_discount.m_roleCode   == ROLECODE_SELF     ))
     		{
     			continue;
     		}    

        //排重插入
        for (iterd=m_pComBill->m_acctDiscountVector.begin();
             iterd!=m_pComBill->m_acctDiscountVector.end(); iterd++)
        {
            if (iterd->m_adjustInsId == l_discount.m_adjustInsId &&
                iterd->m_idTypeCode == l_discount.m_idTypeCode)
            {
                break;
            }
        }

        if (iterd == m_pComBill->m_acctDiscountVector.end())
        {
           m_pComBill->m_acctDiscountVector.push_back(l_discount);
        }
    }

    for(vector<AdjustBefore>::iterator t_itr=t_adjustBefores.begin();t_itr!=t_adjustBefores.end();++t_itr)
    {	    	
      m_pComBill->m_adjustBeforeVector.push_back(*t_itr);
    }    
   		//优惠排序
  		::partial_sort (m_pComBill->m_acctDiscountVector.begin(),m_pComBill->m_acctDiscountVector.end(),
                     m_pComBill->m_acctDiscountVector.end());
	
		  	if(!(m_pComBill->m_acctDiscountVector.empty()))
		  	{
					m_pComBill->m_acctsDiscountMap.insert(map<T_ACCT_ID,vector<Discount> >::value_type(t_id, m_pComBill->m_acctDiscountVector));
					m_pComBill->m_acctIds.insert(*iter);
					m_pComBill->m_isExistAcctDiscnt=true;
		  	}
		}
     	return true;
}

//求账户优惠后的用户级资费add by xuzy 20110207
int UserAfterAcctTpProc::execute()
{
	try
	{
		m_pComBill=&m_pRateInfo->m_comBill;

    if (!m_pComBill->m_ifExistUserInfo)
    	return 1;
    	
		//获取账户级优惠后用户级资费（事件类型为27的资费）
		if(!getTpInfo())
		{
			DEBUG_PRINT("[%s:%d] get user:[%s] getUserTpInfo error!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str()); 
			return 1; 
		}  
		
#ifdef _DEBUG_
	cout<<"账户级优惠后用户级资费:"<<endl;
	for(vector<Discount>::iterator t_it = m_pComBill->m_userAfterAcctDiscntVec.begin();
		  t_it != m_pComBill->m_userAfterAcctDiscntVec.end();++t_it )
		  cout<<*t_it<<endl;	  
#endif
		
	}	
	catch(exception &e)
 	{
#ifdef _DEBUG_		
		cout<<"["<<__FILE__<<":"<<__LINE__<<"]账户级优惠后用户级资费求取错误!原因:"<<e.what()<<endl;
#endif		 	
  		theErrorMessages->insert(E_USER_DISCNT_PARAM, m_pComBill->m_userId + ": userAfterAcct discount  error! error reason= " + string(e.what()) ); 
  		return 1;
  	}	
				
		return 0;
}


bool UserAfterAcctTpProc::getTpInfo()
{
	//根据用户取资费和帐前调帐
	m_pComBill->m_userAfterAcctDiscntVec.clear();
	m_pComBill->m_adjustBeforeVector.clear();
	
	vector<Discount>::iterator  iterd;

	//成员变量初始化
   m_userDiscnt.initialize(m_pComBill->m_userId,
                           USER_ID_TYPE,
                           m_pRateInfo, 
                           m_pmManager,
                           m_pComBill);
    //根据资费获取资费构成 	
	
	  bool t_needGetTp = true;
	  m_pComBill->m_needGetPartTp = false; //需要获取部分资费ID add by xuf 2010-1-7 21:13:01
	  
	   //优惠在某些状态下全失效modify by xuf 2009-12-29 19:15:22
   	//CondBase *l_pCondBase = m_pmManager->getCondition(CONDID_STATE_DISCNT_INVALID);
   	if(m_pmManager->m_pCondBaseDiscntInValid!=NULL)
    {
    	if(m_pmManager->m_pCondBaseDiscntInValid->execute())
    	{  
    		if ((m_pmManager->m_exTpVec.empty())&&(m_pmManager->m_exEventTpVec.empty())) 		
             t_needGetTp = false;
        else 
        	  m_pComBill->m_needGetPartTp = true;
        	       
    	}
   	}
	
     if(t_needGetTp)
     {	
     	getPolicyCompByTp();  
     } 
     
//#ifdef  __PROV019__  //如果是山西		 
    //add by cuiyw at 2009-12-13 22:05:06 
    //优惠排序
    ::partial_sort (m_pComBill->m_userAfterAcctDiscntVec.begin(),m_pComBill->m_userAfterAcctDiscntVec.end(),
                   m_pComBill->m_userAfterAcctDiscntVec.end());

    vector<PO_FeepolicyMutexRela > t_mutexTp;
    vector <Discount> t_tmpDiscountVector;
    set   <T_TP_ID> t_DeleteTp;   
    bool t_isEedFollow = false;
    t_mutexTp.clear();
    t_DeleteTp.clear();
    t_tmpDiscountVector.clear();

    t_tmpDiscountVector.swap(m_pComBill->m_userAfterAcctDiscntVec);

    for(vector <Discount> ::iterator t_mutexiter = t_tmpDiscountVector.begin();
        t_mutexiter!= t_tmpDiscountVector.end();++t_mutexiter )
    {   	
    	  //如果找不到与其互斥的优先级高的资费
        if(m_pRateInfo->m_paramMgr->getFeepolicyMutex(((*t_mutexiter).m_rateTpInfo).m_rateTpId,t_mutexTp)==0)
        {
            m_pComBill->m_userAfterAcctDiscntVec.push_back(*t_mutexiter);
        }
        else//如果有互斥资费
        {
        	  t_isEedFollow = false; 
        	  
            //循环比当前资费优先级高的资费，看是否与当前资费互斥
            for(vector <Discount> ::iterator iters = m_pComBill->m_userAfterAcctDiscntVec.begin();
                                iters != m_pComBill->m_userAfterAcctDiscntVec.end();++iters)
            {  
    	  		      
    	  		      PO_FeepolicyMutexRela t_mutexTpPo;
    	  		      t_mutexTpPo.m_record.m_lowpriorId=((*t_mutexiter).m_rateTpInfo).m_rateTpId;  
    	  		      t_mutexTpPo.m_record.m_highpriorId=((*iters).m_rateTpInfo).m_rateTpId;  
    	  		      
    	  		      //存在优先级高的资费互斥 
    	  		      vector<PO_FeepolicyMutexRela >::iterator t_mutexTpItr;
    	  		      t_mutexTpItr=lower_bound(t_mutexTp.begin(),t_mutexTp.end(),t_mutexTpPo);	

#ifdef DEBUG
		cout<<"((*iters).m_rateTpInfo).m_rateTpId:"<<((*iters).m_rateTpInfo).m_rateTpId<<endl;
		cout<<"t_mutexTpPo.m_record.m_lowpriorId:"<<t_mutexTpPo.m_record.m_lowpriorId<<endl;
		cout<<"t_mutexTpPo:"<<endl;
		for(vector<PO_FeepolicyMutexRela >::iterator t_itr=t_mutexTp.begin();
			 t_itr!=t_mutexTp.end();t_itr++)
		{
		  	cout<<(*t_itr)<<endl;
		}
		
		if(t_mutexTpItr!=t_mutexTp.end())
			cout<<"t_mutexTpItr:"<<*t_mutexTpItr<<endl;
#endif      	  		     
    	  		     
    	  		      if (t_mutexTpItr!=t_mutexTp.end()&&(*t_mutexTpItr==t_mutexTpPo))	
    	  		      {
    	  		      	   //当前资费加入到删除列表
    	  		      	   if(t_DeleteTp.find(t_mutexiter->m_discountId) == t_DeleteTp.end())
                      {
                            t_DeleteTp.insert(t_mutexiter->m_discountId);
                      }
                     
                     //A互斥B，如果A资费结束时间在本帐期，把A的结束时间+1天作为B的起始时间
                    if ((*t_mutexTpItr).m_record.m_mutexType == MUTEX_TYPE_ENDFOLLOW)
                    {    
                    	//资费结束时间在本帐期
                        if(((*iters).m_rateTpInfo).m_pUserTp->m_endTime<m_pRateInfo->m_pSysCycParam->m_endTime)
                        {
                            char t_strBeginTime[8+1];
                            memset(t_strBeginTime,0,9);
                            //资费结束时间+1天
                            TimeUtil::addDays(t_strBeginTime,((*iters).m_rateTpInfo).m_pUserTp->m_endTime.substr(0,8).c_str(),1);

                            ((*t_mutexiter).m_rateTpInfo).m_pUserTp->m_beginTime = (string(t_strBeginTime)+string("000000"));
                             t_isEedFollow = true;
                        }
                    } 
                        
                    break; //只要存在一条互斥 ，本资费即被互斥掉，无需再循环  
    	  		  }  

    	  		}//for 
    	  		
    	  		//如果资费未被互斥,添加到资费列表 
    	  		if(t_DeleteTp.find(t_mutexiter->m_discountId) == t_DeleteTp.end())
           {
            m_pComBill->m_userAfterAcctDiscntVec.push_back(*t_mutexiter);
           }
           else  if (t_isEedFollow) //或者本月与其互斥的资费月中失效，本资费从互斥资费结束日+1 开始生效
           {
            m_pComBill->m_userAfterAcctDiscntVec.push_back(*t_mutexiter);
           }
        }          
    }                                                 
    //add by cuiyw end
//#endif		 
	 
	  
    Discount     l_discount;

    l_discount.m_userDiscntMode = USER_DISCNTMODE_AFERTACCT; //标识账户级优惠后的用户级优惠处理   add by xuzy 2011.2.7
    
    //获取帐前调整信息
    if (m_pmManager->getAdjustBefore(m_pComBill->m_userId,
                                     USER_ID_TYPE, 
                                     m_pComBill->m_cycleId, 
                                     m_pComBill->m_adjustBeforeVector)<0)
    {
    		DEBUG_PRINT("[%s:%d] get user:[%s] adjust berore error!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str());
    		m_pComBill->setErrorCode(m_pComBill->m_userId,E_ACCT_GET_ADJUSTBEFORE,"0",USER_ID_TYPE);
     		return false;		
    }	
    		

    
    for (vector<AdjustBefore>::iterator itera=m_pComBill->m_adjustBeforeVector.begin();
         itera!=m_pComBill->m_adjustBeforeVector.end(); itera++)
    {
   	  if ( !m_userDiscnt.getEffDiscntByAdjustBefore(*itera, l_discount))
        {
            continue;
        }

           
        //排重插入
        for (iterd=m_pComBill->m_userAfterAcctDiscntVec.begin();
             iterd!=m_pComBill->m_userAfterAcctDiscntVec.end(); iterd++)
        {
          	if (iterd->m_adjustInsId == l_discount.m_adjustInsId &&
                iterd->m_idTypeCode == l_discount.m_idTypeCode)
             {
                break;
             }
        }

        if (iterd == m_pComBill->m_userAfterAcctDiscntVec.end())
        {
            m_pComBill->m_userAfterAcctDiscntVec.push_back(l_discount);
        }
    }
    
    //优惠排序
    ::partial_sort (m_pComBill->m_userAfterAcctDiscntVec.begin(),m_pComBill->m_userAfterAcctDiscntVec.end(),
                   m_pComBill->m_userAfterAcctDiscntVec.end());

	return true;
}

//获取资费构成
bool UserAfterAcctTpProc::getPolicyCompByTp()
{	   
		int l_bRet = false;
		vector<int>::iterator t_IndexItr1;
		vector<int> *l_pAcctTpList= &m_pRateInfo->m_pAcctTpList;
		for(t_IndexItr1 = l_pAcctTpList->begin();
				t_IndexItr1!= l_pAcctTpList->end();
				t_IndexItr1++)
	 {
	 		RateTpInfo &t_rateTp = (m_pRateInfo->m_tpInfoList)[*t_IndexItr1];
	 		 //为帐务处理事件
      #ifdef _DEBUG_
      	cout<<__FILE__<<__LINE__<<" event_feepolicy_id = "<<t_rateTp.m_pEventTp->m_id<<endl;
      #endif
       
       if(t_rateTp.m_eventType == BIZ_TYPE_USERDIS3)
       { 
      		l_bRet = true;  
      		

      		
      		
      		//需要获取部分资费 add by xuf 2010-1-7 21:18:08
      		if (m_pComBill->m_needGetPartTp)
      		{     			
      			strstream ss;
       			string t_strTp;
       			ss << t_rateTp.m_pUserTp->m_tpId;
       			ss >> t_strTp;    
      			// modify by daizq@20100714 不在排除的资费列表， 
      			// 也不在排除的事件资费列表里,批价时不执行该事件资费。
      		  if(!binary_search(m_pmManager->m_exTpVec.begin(),m_pmManager->m_exTpVec.end(),t_strTp))
      		  {
      				strstream ss1;
       				string t_strEventTp;
       				ss1 << t_rateTp.m_pEventTp->m_id;
       				ss1>> t_strEventTp;
       			
      		  	if(!binary_search(m_pmManager->m_exEventTpVec.begin(),m_pmManager->m_exEventTpVec.end(),t_strEventTp))
      		  	{
      					#ifdef _DEBUG_
      						cout<<__FILE__<<__LINE__<<" event_feepolicy_id = "<<t_rateTp.m_pEventTp->m_id<<"满足 DISCNT_INVALID_COND， 不执行"<<endl;
      					#endif
      		  		continue;
      		  	} 

      		  }
      		  
      			      		  		  		
      		}	
      		     		
      		if(!m_userDiscnt.getEffectDiscntByTariffPolicy(t_rateTp.m_pUserTp->m_userId,
      		                            t_rateTp.m_rateRoleCode,
      		                            (long)t_rateTp.m_pUserTp->m_tpObjId, //资费构成->资费实例
      		                            t_rateTp.m_pUserTp->m_beginTime,
      		                            t_rateTp.m_pUserTp->m_endTime,
      		                            t_rateTp.m_priority,
      		                            t_rateTp
      		                            ))
      		 {
      		   DEBUG_PRINT("[%s:%d] get Discount by Traiff Comp [%d] error!\n",__FILE__,__LINE__,(long)t_rateTp.m_rateTpNo);
      		   continue;
      		 }
       }//if 帐务事件
	 	
	 }//for 	
     	         	      
		return l_bRet;
}
