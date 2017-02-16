#include "AcctAttribProc.h"

#include "RateGlobalInfo.h"
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

INSTALL_PROCEDURE_INSTANCE(AcctAttribProc);
//##ModelId=4A5E9D300248
int AcctAttribProc::execute()
{
	try
	{	
	     
#ifdef FP_DEBUG
    FP_BEGIN(AcctAttribProc)
#endif
    m_errCode               = 0;
		//初始化帐务处理公有属性对象
		m_pComBill      = &m_pRateInfo->m_comBill;
    m_pAccountParam = m_pRateInfo->m_paramMgr;

//移到RateGlobalInfo中初始化，move by xuf 2009-12-27 21:27:19
//#ifdef FP_DEBUG
//    FP_BEGIN(AcctAttribProc_InitUser)
//#endif	
//		//初始化用户                      
//		if(!initUser(m_pRateInfo->m_billUserId,m_pRateInfo->m_cycParam))
//		{
//			return -1;
//		}
//#ifdef FP_DEBUG
//    FP_END(AcctAttribProc_InitUser)
//#endif

#ifdef FP_DEBUG
    FP_BEGIN(AcctAttribProc_getUserInfo)
#endif		         
    //取用户信息  
		if (!getUserInfo(m_pComBill->m_userId,m_pRateInfo->m_idInfo))
		{    
			//无资料用户             
			return -1;
		}
    
#ifdef FP_DEBUG
    FP_END(AcctAttribProc_getUserInfo)    
#endif  

 		//取用户群信息
    if(!getGroupInfoByUserId( ))
    {            
			return -1;
    }
  
#ifdef FP_DEBUG
    FP_BEGIN(AcctAttribProc_getServState)
#endif    
    //取用户服务状态
  	if(!getUserServState())
  	{
  		return -1;	
  	}
  	
#ifdef FP_DEBUG
    FP_END(AcctAttribProc_getServState)
#endif    	
  	
#ifdef FP_DEBUG
    FP_BEGIN(AcctAttribProc_getServ)
#endif
  	//取用户服务信息
  	if(!getUserServ())
  	{
  		return -1;	
  	}
#ifdef FP_DEBUG
    FP_END(AcctAttribProc_getServ)
#endif  	
  	

  	
  	//add by xuf 2009-12-27 21:40:53
   //某些用户不出帐
	 if(m_pAccountParam->m_pCondBaseNoAcct!=NULL)
    {
    	if(m_pAccountParam->m_pCondBaseNoAcct->execute())
    		m_pComBill->m_userNeedAcctDealTag=false;
    }	
  	  		
#ifdef FP_DEBUG
    FP_END(AcctAttribProc)
#endif  		   
   
  }
	catch (LINKAGE_MDB::Mdb_Exception &e)
	{
	 	 DEBUG_PRINT("账户优惠错误,原因:[%s]!\n",e.GetString());	
	 	theErrorMessages->insert(E_ACCT_DISCNT,string("账户优惠错误,原因:") + string(e.GetString())); 
	}
    	  
	return 0;   
}


//##ModelId=4A64378C02D0
bool AcctAttribProc::initUser(const T_USER_ID &r_userId,const CycParam &r_cycParam)
{
   		//初始化用户                      
		if(!m_pComBill->initUser(r_userId,r_cycParam))
	  {
			DEBUG_PRINT("[%s:%d] Initialize comBill error!\n",__FILE__,__LINE__);
			return false;
		}
  	return true;
}

bool AcctAttribProc::getUserInfo(const T_SERVER_ID &r_id,IdInfo  &r_idInfo)
{
  return m_pComBill->getUserInfo(r_id,r_idInfo);
}

//##ModelId=4A6437B901E4
bool AcctAttribProc::getUserServState()
{
	 //获取用户状态异动信息
	 if ((*m_pAccountParam).getUserServState(m_pComBill->m_userId, m_pRateInfo->m_vecUserServState)<=0)
	 {
    	 //基本资料有问题
    	 DEBUG_PRINT("[%s:%d]用户[%s]状态不存在!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str());
    	 m_pComBill->setErrorCode(m_pComBill->m_userId,E_ACCT_GET_USERSERV,"0");
    	 
    	 return false;
   }    
  
  m_pRateInfo->m_isExistServState = true;
  
  return true;
}
//##ModelId=4A6437C300DA
bool AcctAttribProc::getUserServ()
{
     //获取用户服务信息
     if ((*m_pAccountParam).getUserServ(m_pComBill->m_userId, m_pRateInfo->m_vecUserServ)<=0)
     {
         //基本资料有问题
         DEBUG_PRINT("[%s:%d]用户[%s]服务不存在!\n",__FILE__,__LINE__,m_pComBill->m_userId.c_str());
         m_pComBill->setErrorCode(m_pComBill->m_userId,E_ACCT_GET_USERSERV,"0");
         
         return false;
    }    
    
  m_pRateInfo->m_isExistServ = true;			
   
  return true;
}

bool AcctAttribProc::getUserGroupInfo(const T_SERVER_ID &r_id,IdInfo   &r_idInfo)
{
	//获取用户基本信息
    //群资料存在
    if ((*m_pAccountParam).getUserInfo(r_id, r_idInfo)>0)
    {
			r_idInfo.m_idAttr[IdInfo::ID_TYPE_CODE]= GROUP_ID_TYPE;
			m_pComBill->m_ifExistGrpInfo = true;
    }    
    else
    {
    	m_pComBill->m_ifExistGrpInfo = false;
    	DEBUG_PRINT("[%s:%d]用户群[%s]资料不存在!\n",__FILE__,__LINE__,r_id.c_str());
			m_pComBill->setErrorCode(r_id,E_ACCT_GET_VPNINFO,"0",GROUP_ID_TYPE);	
    }
    	
    return m_pComBill->m_ifExistGrpInfo;
}

/**
* getGroupInfoByUserId       获取用户群
* @return true: 是  false:否
*/
bool AcctAttribProc::getGroupInfoByUserId()
{	 

#ifdef FP_DEBUG
    FP_BEGIN(AcctAttribProc_getGroupsByUserId)
#endif			   
  //1.根据出帐用户寻找出帐用户直接关联的用户群 
	if((*m_pAccountParam).getGroupsByUserId(m_pComBill->m_userId,m_pComBill->m_cycParam,m_pComBill->m_userGroupIdSet)<0)
  	{
		DEBUG_PRINT("[%s:%d] Get user group error! userId =%s",__FILE__,__LINE__,m_pComBill->m_userId.c_str()); 
		m_pComBill->setErrorCode(m_pComBill->m_userId,E_ACCT_GET_USERVPN,"0"); 
		return false;	
	}

#ifdef FP_DEBUG
    FP_END(AcctAttribProc_getGroupsByUserId)
#endif		

	return  true;
}







