#include "RateGlobalInfo.h"
//#include "TpInfo.h"
#include "base/exceptions.h"
#include <bitset>
//#include "FeePolicyComp.h"
#include "base/TimeUtil.h"


DECLARE_EXCEPTION(AccountException, BusinessException);

//##ModelId=48D6F61703E4
ComBill::ComBill()
{
	m_pBillMgr         = NULL;
	m_pAccountParam    = NULL;  
	m_pAddBillVector   = NULL;
	m_userId           = "";
	m_pErrMsg          = NULL;
	m_insNo            = 0;
	m_netTypeCode      = "10";	
} 		

//##ModelId=48D6F62E036F
ComBill::~ComBill()
{   
	m_userIdSet.clear();
	m_acctIdVector.clear();
	m_acctIdSet.clear();
	m_userGroupIdSet.clear();
	m_groupUserIdMap.clear();
//	m_userAccountMap.clear();
	m_userPaylationMap.clear();
	m_detailBillVector.clear();
	//(*m_pUserServStateVector).clear();
	m_payRelationVector.clear();
	(*m_pAddBillVector).clear();
	m_otherDetailBill.m_detailBillVector.clear();
 	m_detailBill.m_detailBillVector.clear();
	m_acctIds.clear();// add by xuf 2009-8-17 16:53:47
}

//##ModelId=48DB3EEE01B8
/**
* initialize 初始化
* @param  r_channleNo         通道号
* @param  r_provinceCode      省份代码
* @param  r_pBillMgr          账单管理对象指针
* @param  r_pAccountParam     资料管理对象指针
* @param  r_pErrorInfoVector  帐务处理错误信息
* @param  r_pCdr              话单指针
* @paran  r_pErrMsg           错误信息指针
* @paran  r_pUserServStateVector   用户服务状态容器指针
* @paran  r_pUserServVector        用户服务容器指针
* 返回值 :true 成功，false 失败 
*/
bool ComBill::initialize(const int &r_channleNo, 
	                       const string &r_provinceCode, 
	                       BillManager *r_pBillMgr, 
	                       PM_Account  *r_pAccountParam,
	                       vector<AdditionalBill> *r_pAddBillVector,
	                       vector<DiscntBill>  *r_pDiscntBillVector,
	                       const PCDR          &r_pCdr,
	                       PString          *r_pErrMsg,
	                       vector<UserServState>  *r_pUserServStateVector,
	                       vector<UserServ>    *r_pUserServVector 
	                       )
{	
  m_channelNo             = r_channleNo;
  m_provinceCode          = r_provinceCode;
  m_pBillMgr              = r_pBillMgr;
  m_pAccountParam         = r_pAccountParam; 
  m_pAddBillVector        = r_pAddBillVector;
  m_pDiscntBillVector     = r_pDiscntBillVector;
  m_pCdr                  = r_pCdr;
  m_pErrMsg               = r_pErrMsg;
  m_isMonthEnd            = false;
  m_errCode               = 0;
  m_ifExistUserInfo       = false;
 // m_carryMode             = r_pAccountParam->m_carryMode;
  m_divisor   			  = r_pAccountParam->m_divisor;

  m_pUserServStateVector = r_pUserServStateVector ;
  m_pUserServVector      = r_pUserServVector ;
  
  	   //获取帐目信息
   if (!getDetailItems() )
   {
#ifdef _DEBUG_
		cout<<"getItemInfo false! "<<__FILE__<<__LINE__<<endl;
#endif 
    if(m_pErrMsg!=NULL)
     	*m_pErrMsg = "取明细帐目信息错误,表td_b_detailitem为空!";
     return false;                              
   }	
      
   return true;  
}


bool ComBill::initUser(const T_USER_ID &r_userId,const CycParam &r_cycParam)
{

	if (r_userId=="") 
	{
		DEBUG_PRINT("[%s:%d] get userId error!\n",__FILE__,__LINE__);
		m_errCode = E_ACCT_USERID_NULL;
  		setErrorCode(r_userId,E_ACCT_USERID_NULL,"0");
		return false;
	}
	
	m_userId  = r_userId; 
  m_errCode = m_pCdr[ATTRI_ERROR_CODE].value();  		
	m_cycleId = r_cycParam.m_cycleId;
	m_cycParam= r_cycParam; 
	 
	m_userIdSet.clear();
	m_acctIdSet.clear();
 	m_groupUserIdMap.clear();
//(*m_pUserServStateVector).clear();
	m_detailBillVector.clear();
// 	(*m_pDiscntBillVector).clear();
//	(*m_pAddBillVector).clear();
	m_userGroupIdSet.clear();
	m_acctIdVector.clear();    
	m_payRelationVector.clear();
    //m_userAccountMap.clear();
	m_acctIdVector.clear();
	m_userGroupIdSet.clear();
	m_otherDetailBill.m_detailBillVector.clear();
	m_detailBill.m_detailBillVector.clear();
	m_userPaylationMap.clear();
	m_isExistGrpTotalDiscnt = false; 
	m_isExistAcctDiscnt     = false;
	m_acctIds.clear(); //add by xuf 2009-8-17 16:54:02
	m_ifExistUserInfo       = false;
  //add by xuf 2009-10-3 12:10:45
	//(*m_pUserServVector).clear(); 
	m_discountVector.clear(); 
	m_adjustBeforeVector.clear();
	m_userGrpDiscountVector.clear();
	m_acctDiscountVector.clear();
	m_acctsDiscountMap.clear();  
	isMonthEnd();
	m_acctDiscntForAllUsers = true;
	m_grpDiscntForAllUsers  = true;
	m_commonDealFlag    = 0;
	m_userGrpBills_new.clear();
	m_userTpVector.clear();
	m_acctTpMap.clear();
  	m_userNeedAcctDealTag = true;
  m_needGetPartTp = false;// add by xuf 2010-1-7 23:22:15
  
//		//某些用户不出帐erased by xuf 2009-12-27 21:37:12
//    CondBase *l_pCondBase = m_pAccountParam->getCondition(m_pAccountParam->m_isNeedAcctDealCondId);
//	 if(l_pCondBase!=NULL)
//    {
//    	if(l_pCondBase->execute())
//    		m_userNeedAcctDealTag=false;
//    }	
//
//	if(!ifNeedAccountDeal())
//  {
//  		DEBUG_PRINT("[%s:%d] can not do account process!\n",__FILE__,__LINE__);
//  		return false;	
//  }
  
   if (m_errCode!=0)
  {
  		DEBUG_PRINT("[%s:%d] can not do account process!\n",__FILE__,__LINE__);
		setErrorCode(r_userId,E_ACCT_INITIALIZE,"0",USER_ID_TYPE,m_errCode);	
		m_errCode=0;
  }
  
   
  	return true;
}

//##ModelId=48D6F9930062
void ComBill::initAcct(const T_ACCT_ID &r_acctId)
{
	m_acctId = r_acctId;
  m_unipayPhone = "";
  m_unionPayType = 0;   
}

//##ModelId=48D6F9A70060
void ComBill::initUserGroup(const T_USER_ID &r_id)
{
  m_userGrpId 						= r_id;
  m_isExistGrpTotalDiscnt = true;	
  
	//25事件中不需要
	if ((*m_pAccountParam).m_isNeedRealGrpDiscntTag&&
		 m_pCdr[ATTRI_BIZ_TYPE]!=BIZ_TYPE_GRPBILL)
	{	 	   
  		T_VFILE t_vfile;
  		t_vfile.m_cycParam = m_cycParam;
    	t_vfile.m_errno    =  0;
  	    if (m_vUserFileMap.find(m_userGrpId)==m_vUserFileMap.end())
  			m_vUserFileMap.insert(make_pair<T_SERVER_ID,T_VFILE>(m_userGrpId,t_vfile));
 	}
  
}

//##ModelId=48D6F68C0099
bool ComBill::getUserInfo(const T_SERVER_ID &r_id,IdInfo  &r_idInfo)
{
    //Added By Sunam 2009/10/27 18:12:53固网共享时长话单不查
    if (strncmp(m_pCdr[ATTRI_FILE_NAME].c_str(),"TTD",3) == 0)
    {
        return true;
    }
			
    //用户资料不存在
    if ((*m_pAccountParam).getUserInfo(r_id, r_idInfo,m_cycParam)<=0)
    {
         //基本资料有问题
         DEBUG_PRINT("[%s:%d] 用户[%s]资料不存在!\n",__FILE__,__LINE__,r_id.c_str());
         setErrorCode(r_id,E_ACCT_GET_USERINFO,"0");
         if (m_userId==r_id) 	
         	m_ifExistUserInfo = false;
         return false;
    }    
    
    //设置网别
    m_netTypeCode = r_idInfo.m_idAttr[IdInfo::ID_NETTYPE_CODE];
   
   if (m_userId==r_id) 	
    	m_ifExistUserInfo = true;	
    return true;
}


//##ModelId=48D6F7B801DB
bool ComBill::getUserGroupInfo(const T_SERVER_ID &r_id,IdInfo   &r_idInfo)
{
	  //获取用户基本信息
    //群资料存在
    if ((*m_pAccountParam).getUserInfo(r_id, r_idInfo,m_cycParam)>0)
    {
			m_ifExistGrpInfo = true;
    }    
    else
    {
    	m_ifExistGrpInfo = false;
    	DEBUG_PRINT("[%s:%d]用户群[%s]资料不存在!\n",__FILE__,__LINE__,r_id.c_str());
			setErrorCode(r_id,E_ACCT_GET_VPNINFO,"0",GROUP_ID_TYPE);	
    }
    	
    return m_ifExistGrpInfo;
}

//##ModelId=48D6F893006D
bool ComBill::getAcctInfo(const T_SERVER_ID &r_id,IdInfo    &r_idInfo)
{
    //帐户基本资料不存在
    if ((*m_pAccountParam).getAcctInfo(r_id,r_idInfo)<=0)
    {
    		DEBUG_PRINT("[%s:%d]账户[%s]资料不存在!\n",__FILE__,__LINE__,r_id.c_str());
  			setErrorCode(r_id,E_ACCT_GET_ACCTINFO,"0",ACCT_ID_TYPE);     
        return false;
    }
    //帐户基本资料存在
    else
    {
       m_unionPayType = 0;
       m_unipayPhone = "";

    }
    
    return true;
}


/**
* getGroupInfoByUserId       获取用户群
* @return true: 是  false:否
*/
bool ComBill::getGroupInfoByUserId()
{	 

#ifdef FP_DEBUG
    FP_BEGIN(ComBill_getGroupsByUserId)
#endif			   
  //1.根据出帐用户寻找出帐用户直接关联的用户群 
	 if((*m_pAccountParam).getGroupsByUserId(m_userId,m_cycParam,m_userGroupIdSet)<0)
   {
     DEBUG_PRINT("[%s:%d] Get user group error! userId =%s",__FILE__,__LINE__,m_userId.c_str()); 
     setErrorCode(m_userId,E_ACCT_GET_USERVPN,"0"); 
     return false;	
   }

#ifdef FP_DEBUG
    FP_END(ComBill_getGroupsByUserId)
#endif		

	return  true;
}

bool ComBill::GetUserInfoByGrpId(const T_SERVER_ID &r_groupId)
{ 
	map<T_SERVER_ID,T_MEMBERINFO_TYPE> l_userIdMap; 
 	 
	if((*m_pAccountParam).getUserInfoByVpnId(r_groupId, m_cycParam,VPN_MEMTYPE_NULL,l_userIdMap)<0)
	{
		DEBUG_PRINT("[%s:%d] Get group's users error! userId =%s.",__FILE__,__LINE__,m_userId.c_str()); 
    setErrorCode(m_userId,E_ACCT_GET_USERMEM,"0");
		return false;
	}   
      
	m_groupUserIdMap[r_groupId]= l_userIdMap;
	
	return true;    	
}

//在系统初始化的时候取，needupdate
bool ComBill::getDetailItems( )
{    
	memset(m_detailItem, 0, MAX_ITEM*sizeof(DetailItemInfo));
    memset(m_existFlag, '0', MAX_ITEM*sizeof(unsigned char));
    memset(m_carryModes, '0', MAX_ITEM*sizeof(unsigned char));
    if(!(*m_pAccountParam).getDetailItem(m_detailItem, m_existFlag, m_carryModes))
    {
		 DEBUG_PRINT("[%s:%d] get DETAIL_ITEM error!\n",__FILE__,__LINE__); 
		 return false;
    }
     
   return true; 	
}

//add for round 2009-9-9 13:53:13
  /**
    * carry: 进位.
    * @param  帐目.
    * @param  值.
    * @return 进位后的值.
    */     
long ComBill::carry(const int &r_itemCode, const long &r_value)
{
	   char l_pErrMsg[99999];
      if (r_itemCode>MAX_ITEM||r_itemCode<0)
      {
       	sprintf(l_pErrMsg,"[Error(%s:%d):itemCode(%d)，帐目超出范围!]\n",
            __FILE__,__LINE__,r_itemCode);
     			DEBUG_PRINT("%s",l_pErrMsg);
     			THROW(BusinessException, l_pErrMsg);  
      }	
       	
     switch (m_carryModes[r_itemCode])
     { 	
        //四舍五入法进位
     case  CARRY_MODE_ROUND :
       
     	 if (r_value>=0)              
				return  (r_value+m_divisor/2)/m_divisor; 
			else
				return  (r_value-m_divisor/2)/m_divisor;
         break;
     //去尾法进位
     case  CARRY_MODE_CUT :

     	 return  r_value/m_divisor; 
     	  break;
     //直接进位
     case  CARRY_MODE_ADD :
     
     	 if (r_value>=0)   
        	return  r_value/m_divisor+1; 
       else
       		return  r_value/m_divisor-1; 
        break;    
     default : //默认四舍五入
     	  if (r_value>=0)  
     	          return   (r_value+m_divisor/2)/m_divisor; 
     		else 
     			  return   (r_value-m_divisor/2)/m_divisor;
     			  
     } 
}

//判断明细帐目是否存在
bool ComBill::ifItemExists(const int &r_detailItem)
{
	 if (r_detailItem>MAX_ITEM||r_detailItem<0)
	 {
#ifdef DEBUG
	cout<<__FILE__<<":"<<__LINE__<<" Item: "<<r_detailItem<<" length is out of range! MAX_ITEM="<<MAX_ITEM<<endl;
#endif	 	
		 setErrorCode(m_userId,E_ACCT_ITEM,"0",USER_ID_TYPE,r_detailItem);
	 	 return false;
	 }  
	 return  (m_existFlag[r_detailItem]=='1')?true:false;
}

 
bool ComBill::getLastStopTime(IdInfo &r_userInfo)
{
	  if((m_pAccountParam->m_openStateSet).empty())
	  {
	  	(m_pAccountParam->m_openStateSet).insert('0');
	  	(m_pAccountParam->m_openStateSet).insert('A');
	  	(m_pAccountParam->m_openStateSet).insert('B');
		(m_pAccountParam->m_openStateSet).insert('N');
	  }	
	  	
	  r_userInfo.m_idAttr[IdInfo::ID_LASTSTOP_TIME] = "20501231000000";
    vector<UserServState>::iterator  iter;
    for (iter=(*m_pUserServStateVector).begin(); 
         iter!=(*m_pUserServStateVector).end(); ++iter)
    {
        if (iter->m_mainTag=='1'    && 
            ((m_pAccountParam->m_openStateSet).find(iter->m_servStateCode)==(m_pAccountParam->m_openStateSet).end())&&
            iter->m_startDate<m_cycParam.m_endTime  &&
            iter->m_startDate<r_userInfo.m_idAttr[IdInfo::ID_LASTSTOP_TIME]&&
            judgeNotOpen(iter->m_startDate, 
                          m_cycParam.m_endTime,
                          (*m_pUserServStateVector)))
        {
            r_userInfo.m_idAttr[IdInfo::ID_LASTSTOP_TIME] = iter->m_startDate;
            
            return true;
        }
    }
    
    return false;
}

/**
* judgeNotOpen判断在时间段内用户的主服务是否开通过
* @param  startDate  开始时间
* @param  endDate    结束时间
* @param  endDate    用户当前的服务状态集
* @return true：没有开通 false：有开通  
*/       
bool ComBill::judgeNotOpen(const string& startDate,
                           const string& endDate,
                           const vector<UserServState>& userServStateVector)
{    
    bool     isValid = true;
    
    vector<UserServState>::const_iterator  iters;
    for (iters=userServStateVector.begin();
         iters!=userServStateVector.end(); iters++)
    {
        if (iters->m_mainTag=='1'           && 
           ((iters->m_servStateCode=='0')   ||
            (iters->m_servStateCode=='N') ) &&  
            iters->m_startDate<endDate  &&
            iters->m_endDate>startDate  &&
            iters->m_startDate<iters->m_endDate)
        {
           isValid = false;
           break;
        }
    }   
    
    return isValid;

}
//add by yuxk begin 2009-9-27 20:48:58
void ComBill::getLastITime(T_OBJECT_ATTR &r_attrValue,bool r_flag)
{
	  r_attrValue = "20501231000000";
    vector<UserServState>::iterator  iter;
//    char r_servstate ='I';
//    if (m_provinceCode == "019")
//    	{
//    		r_servstate='1';
//    	}
    	
   if (r_flag)//仅仅取最后一次停保时间 add  by xuf 2010-1-15 15:29:28
   {
   	 vector<UserServState>::reverse_iterator  iter_r;
   	 for (iter_r=(*m_pUserServStateVector).rbegin(); 
         iter_r!=(*m_pUserServStateVector).rend(); ++iter_r)
    {
        if (iter_r->m_mainTag=='1'    && 
//            iter_r->m_servStateCode==r_servstate  &&
(*m_pAccountParam).m_otherStateTagMap.find(iter_r->m_servStateCode)!=(*m_pAccountParam).m_otherStateTagMap.end() &&
            iter_r->m_startDate<m_cycParam.m_endTime  &&
            iter_r->m_startDate<r_attrValue)
        {
            r_attrValue = iter_r->m_startDate;
            return;
        }
    }  
	
	return ;   	 
   }  	
    	
    for (iter=(*m_pUserServStateVector).begin(); 
         iter!=(*m_pUserServStateVector).end(); ++iter)
    	{
        if (iter->m_mainTag=='1'    && 
//            iter->m_servStateCode==r_servstate  &&
(*m_pAccountParam).m_otherStateTagMap.find(iter->m_servStateCode)!=(*m_pAccountParam).m_otherStateTagMap.end() &&
            iter->m_startDate<m_cycParam.m_endTime  &&
            iter->m_startDate<r_attrValue&&
            judgeOnlyI(iter->m_startDate, 
                          m_cycParam.m_endTime,
                          (*m_pUserServStateVector)))
        {
            r_attrValue = iter->m_startDate;
        }
    }    
    	}
    	
void ComBill::getLastITimenew(T_OBJECT_ATTR &r_attrValue,bool r_flag)
{
	  r_attrValue = "20501231000000";
    vector<UserServState>::iterator  iter;
   if (r_flag)//仅仅取最后一次停保时间 add  by xuf 2010-1-15 15:29:28
   {
   	 vector<UserServState>::reverse_iterator  iter_r;
   	 for (iter_r=(*m_pUserServStateVector).rbegin(); 
         iter_r!=(*m_pUserServStateVector).rend(); ++iter_r)
    {
        if (iter_r->m_mainTag=='1'    && 
            (*m_pAccountParam).m_otherStateTagMap.find(iter_r->m_servStateCode)!=(*m_pAccountParam).m_otherStateTagMap.end() &&
            iter_r->m_startDate<m_cycParam.m_endTime  &&
            iter_r->m_startDate<r_attrValue)
        {
            r_attrValue = iter_r->m_startDate;
            return;
        }
    }  
	
	return ;   	 
   }  	
    	
    for (iter=(*m_pUserServStateVector).begin(); 
         iter!=(*m_pUserServStateVector).end(); ++iter)
    {
        if (iter->m_mainTag=='1'    && 
             (*m_pAccountParam).m_otherStateTagMap.find(iter->m_servStateCode)!=(*m_pAccountParam).m_otherStateTagMap.end() &&
            iter->m_startDate<m_cycParam.m_endTime  &&
            iter->m_startDate<r_attrValue&&
            judgeOnlyI(iter->m_startDate, 
                          m_cycParam.m_endTime,
                          (*m_pUserServStateVector),true))
        {
            r_attrValue = iter->m_startDate;
        }
    }    
}

/**
* judgeOnlyI判断在时间段内用户的主服务是否有停机保号以外的服务状态
* @param  startDate  开始时间
* @param  endDate    结束时间
* @param  endDate    用户当前的服务状态集
* @return true：没有其他状态 false：有其他状态  
*/       
bool ComBill::judgeOnlyI(const string& startDate,
                           const string& endDate,
                           const vector<UserServState>& userServStateVector,bool r_flag)
{    
    bool     isValid = true;
//    char r_servstate ='I';
//    if (m_provinceCode == "019")
//    	{
//    		r_servstate='1';
//    		}    
    vector<UserServState>::const_iterator  iters;
    for (iters=userServStateVector.begin();
         iters!=userServStateVector.end(); iters++)
    {
        if (iters->m_mainTag=='1'           && 
//           iters->m_servStateCode!=r_servstate   &&  
//           iters->m_servStateCode!='5'   &&  
//           iters->m_servStateCode!='7'   &&  
//           iters->m_servStateCode!='A'   &&  
//           iters->m_servStateCode!='B'   &&  
(*m_pAccountParam).m_otherStateTagMap.find(iters->m_servStateCode)==(*m_pAccountParam).m_otherStateTagMap.end() &&
(*m_pAccountParam).m_expStateTagMap.find(iters->m_servStateCode)==(*m_pAccountParam).m_expStateTagMap.end() &&
           (r_flag==false || (r_flag==true && iters->m_servStateCode!='6')) &&
            iters->m_startDate<endDate  &&
            iters->m_endDate>startDate  &&
            iters->m_startDate<iters->m_endDate)
        {
           isValid = false;
           break;
        }
    }   
    
    return isValid;

}


void ComBill::getLastStopOpenTime(T_OBJECT_ATTR &r_attrValue,bool r_isObjUse)
{
    T_DATE_TIME beginTime;
	  beginTime = "20501231000000";
	  r_attrValue = "20501231000000";
//    char r_servstate ='I';
//    if (m_provinceCode == "019")
//    	{
//    		r_servstate='1';//山西特殊，停机保号收月租是1状态
//    		}  
    vector<UserServState>::reverse_iterator  iter;
    for (iter=(*m_pUserServStateVector).rbegin(); 
         iter!=(*m_pUserServStateVector).rend(); ++iter)
    {
        if (iter->m_mainTag=='1'    && 
//            iter->m_servStateCode==r_servstate  &&
(*m_pAccountParam).m_otherStateTagMap.find(iter->m_servStateCode)!=(*m_pAccountParam).m_otherStateTagMap.end() &&
            iter->m_startDate<m_cycParam.m_endTime  &&
            iter->m_startDate<beginTime &&
						(r_isObjUse==true||
						(r_isObjUse==false&&
			 			iter->m_startDate<m_cycParam.m_beginTime
			 			))
			)//modify by xuf 2010-1-13 23:34:02
        {
            beginTime = iter->m_endDate;
            vector<UserServState>::iterator  iters;
            for (iters=(*m_pUserServStateVector).begin();
                 iters!=(*m_pUserServStateVector).end(); ++iters)
            {
                if (iters->m_mainTag=='1'           && 
//                   (iters->m_servStateCode=='0' || iters->m_servStateCode=='N') &&  
(*m_pAccountParam).m_stateTagMap.find(iters->m_servStateCode)!=(*m_pAccountParam).m_stateTagMap.end() &&
                    iters->m_startDate<m_cycParam.m_endTime  &&
                    iters->m_startDate>=beginTime  &&
                    iters->m_startDate<iters->m_endDate)
                {
                	 r_attrValue = iters->m_startDate;
                   break;
                }   
                else if
                	(iters->m_mainTag=='1' && 
//                   iters->m_servStateCode!='5' &&  
//                   iters->m_servStateCode!='7' &&  
//                   iters->m_servStateCode!='A' &&  
//                   iters->m_servStateCode!='B' &&  
(*m_pAccountParam).m_otherStateTagMap.find(iters->m_servStateCode)==(*m_pAccountParam).m_otherStateTagMap.end() &&//modify by yuxk 2010-2-25 9:40:16
(*m_pAccountParam).m_expStateTagMap.find(iters->m_servStateCode)==(*m_pAccountParam).m_expStateTagMap.end() &&
                    iters->m_startDate<m_cycParam.m_endTime  &&
                    iters->m_startDate>=beginTime  &&
                    iters->m_startDate<iters->m_endDate)
                {
                   break;
                }	
            }         
            
            break;            
        }
    } 
}

void ComBill::getFirstStopOpenTime(T_OBJECT_ATTR &r_attrValue,const T_DATE_TIME &r_beginDate,const T_DATE_TIME &r_endDate)
{
    T_DATE_TIME beginTime;
	  beginTime = "20501231000000";
	  r_attrValue = "20501231000000";
    vector<UserServState>::reverse_iterator  iter;
    for (iter=(*m_pUserServStateVector).rbegin(); 
         iter!=(*m_pUserServStateVector).rend(); ++iter)
    {
        if (iter->m_mainTag=='1'    && 
           (*m_pAccountParam).m_otherStateTagMap.find(iter->m_servStateCode)!=(*m_pAccountParam).m_otherStateTagMap.end() &&
            iter->m_startDate<m_cycParam.m_endTime  &&
            iter->m_startDate<beginTime&&
			 			iter->m_startDate<r_beginDate &&
			 			iter->m_startDate<iter->m_endDate
			)
        {
            beginTime = iter->m_endDate;
            vector<UserServState>::iterator  iters;
            for (iters=(*m_pUserServStateVector).begin();
                 iters!=(*m_pUserServStateVector).end(); ++iters)
            {
                if (iters->m_mainTag=='1'           && 
                   (*m_pAccountParam).m_stateTagMap.find(iters->m_servStateCode)!=(*m_pAccountParam).m_stateTagMap.end() &&
                    iters->m_startDate<m_cycParam.m_endTime  &&
                    iters->m_startDate>=beginTime  &&
                    iters->m_startDate<iters->m_endDate &&
                    iters->m_startDate<r_endDate)
                {
                	 r_attrValue = iters->m_startDate;
                   break;
                }   
                else if
                	(iters->m_mainTag=='1' && 
                (*m_pAccountParam).m_otherStateTagMap.find(iters->m_servStateCode)==(*m_pAccountParam).m_otherStateTagMap.end() &&//modify by yuxk 2010-2-25 9:41:08
                (*m_pAccountParam).m_expStateTagMap.find(iters->m_servStateCode)==(*m_pAccountParam).m_expStateTagMap.end() &&
                    iters->m_startDate<m_cycParam.m_endTime  &&
                    iters->m_startDate>=beginTime  &&
                    iters->m_startDate<iters->m_endDate)
                {
                   break;
                	}	
            }         
            
            break;            
        }
    } 
}

bool ComBill::judgeServHalfMonth(const int &r_attrValue,const int &r_timeRangeFlag)
{

    T_OBJECT_ATTR laststopopentime;
	  T_DATE_TIME lastitime = "20501231000000";
	  T_DATE_TIME serv_beginTime = "20501231000000";
	  T_DATE_TIME serv_endTime = "19900101000000";
    T_DATE_TIME t_beginTime,t_endTime;
	  if (r_timeRangeFlag==0 || r_timeRangeFlag==1 || r_timeRangeFlag==2)
	  	{
//停保复话时间指：往月停保本月复话，默认时间20501231000000
      getLastStopOpenTime(laststopopentime);
      if (laststopopentime < m_cycParam.m_beginTime) laststopopentime="20501231000000";
//服务开始时间 当月有效的第一条
    vector<UserServ>::const_iterator  iter;
    for (iter=(*m_pUserServVector).begin(); iter!=(*m_pUserServVector).end(); iter++)
    {
        if (iter->m_servId==r_attrValue    &&
          iter->m_startDate<m_cycParam.m_endTime   &&
          iter->m_endDate>m_cycParam.m_beginTime   &&
          iter->m_startDate<iter->m_endDate
          )
      {
      if (lastTimesNotExistServer(r_attrValue,iter->m_startDate)==false && iter->m_startDate>=m_cycParam.m_beginTime)
       {
       serv_beginTime="19900101000000";
       }
       else
       {
       serv_beginTime=iter->m_startDate;
       }
       break;
      }
    }
    if (serv_beginTime=="20501231000000") return false;
    	
    if (laststopopentime!="20501231000000" && laststopopentime>serv_beginTime)
{
t_beginTime=laststopopentime;
}    
else
{
t_beginTime=serv_beginTime;
}
	
    }
	  if (r_timeRangeFlag==3 || r_timeRangeFlag==4 || r_timeRangeFlag==5)
	  	{
//停保时间指：当月最后一次停保，没有复话（可以直接紧跟拆机，但是不可以是拆机复装），默认时间20501231000000    
      vector<UserServState>::iterator  iter_r;  	
    for (iter_r=(*m_pUserServStateVector).begin(); 
         iter_r!=(*m_pUserServStateVector).end(); ++iter_r)
    {
        if (iter_r->m_mainTag=='1'    && 
           (*m_pAccountParam).m_otherStateTagMap.find(iter_r->m_servStateCode)!=(*m_pAccountParam).m_otherStateTagMap.end() &&
            iter_r->m_startDate<m_cycParam.m_endTime  &&
            iter_r->m_startDate>m_cycParam.m_beginTime  &&
            iter_r->m_startDate<lastitime&&
            judgeOnlyI(iter_r->m_startDate, 
                          m_cycParam.m_endTime,
                          (*m_pUserServStateVector),true))
        {
            lastitime = iter_r->m_startDate;
        }
    }    
//服务结束时间 当月有效的最后一条
    vector<UserServ>::reverse_iterator  iters;
    for (iters=(*m_pUserServVector).rbegin(); iters!=(*m_pUserServVector).rend(); iters++)
    {
        if (iters->m_servId==r_attrValue    &&
          iters->m_startDate<m_cycParam.m_endTime   &&
          iters->m_endDate>m_cycParam.m_beginTime   &&
          iters->m_startDate<iters->m_endDate
          )
      {
      if (nextTimesNotExistServer(r_attrValue,iters->m_endDate)==false && iters->m_endDate<=m_cycParam.m_endTime)
       {
       serv_endTime="20501231000000";
       }
       else
       {
       serv_endTime=iters->m_endDate;
       }
       break;
      }
    }
    if (serv_endTime=="19900101000000") return false;
   if (lastitime!="20501231000000" && lastitime<serv_endTime)
   {
   t_endTime=lastitime;
   }    
   else
   {
   t_endTime=serv_endTime;
   }

    }

    
 if (r_timeRangeFlag==1) 
 {
  if (t_beginTime>m_cycParam.m_beginTime && t_beginTime<m_cycParam.m_halfTime) return true;
  return false;
 }
 else if (r_timeRangeFlag==2) 
 {
  if (t_beginTime>=m_cycParam.m_halfTime && t_beginTime<m_cycParam.m_endTime) return true;
  return false;
 }
 else if (r_timeRangeFlag==3) 
 {
  if (t_endTime>m_cycParam.m_beginTime && t_endTime<m_cycParam.m_halfTime) return true;
  return false;
 }
 else if (r_timeRangeFlag==4) 
 {
  if (t_endTime>=m_cycParam.m_halfTime && t_endTime<m_cycParam.m_endTime) return true;
  return false;
 }
 else if (r_timeRangeFlag==0) 
 {
  if (t_beginTime>m_cycParam.m_beginTime && t_beginTime<m_cycParam.m_endTime) return true;
  return false;
 }
 else if (r_timeRangeFlag==5) 
 {
  if (t_endTime>m_cycParam.m_beginTime && t_endTime<m_cycParam.m_endTime) return true;
  return false;
 }
 else 
 {
 return false;
 }
}

//add by yuxk end 2009-9-27 20:49:12
//add by yuxk for shanxi begin
/**
* judgeServStart判断用户服务开始时间是否在本月上下半月
* 上半月开始 r_beginDate = m_cycParam.m_beginTime ,r_endDate = m_cycParam.m_halfTime
* 下半月开始 r_beginDate = m_cycParam.m_halfTime  ,r_endDate = m_cycParam.m_endTime  
* 且r_beginDate 的前一秒不存在结束的相同服务（为产品变更)
* @param  r_servId  服务ID
* @param  r_beginDate  开始时间
* @param  r_endDate    结束时间
* @return true：是 false：否 
*/     
bool ComBill::judgeServStart(const int &r_servId,const T_DATE_TIME &r_beginDate,const T_DATE_TIME &r_endDate)
{
      vector<UserServ>::const_iterator  iters;
        for (iters=(*m_pUserServVector).begin(); iters!=(*m_pUserServVector).end(); iters++)
    {
        if (iters->m_servId==r_servId    &&
          iters->m_startDate<r_endDate      &&
          iters->m_startDate>=r_beginDate   &&
          iters->m_endDate>r_beginDate      &&
          iters->m_startDate<iters->m_endDate&&
          lastTimesNotExistServer(r_servId,iters->m_startDate)
          )
      {
          return true;
      }
    }

  return false;
}

/**
* lastTimesNotExistServer判断前一秒是否存在相同服务
* @param  r_servId  服务ID
* @param  r_beginDate  开始时间
* @return true：不存在 false：存在 
*/  
bool  ComBill::lastTimesNotExistServer(const int &r_servId,const T_DATE_TIME &r_beginDate)
{
		T_DATE_TIME t_beginDate ;
		
		char t_strBeginTime[14+1];
     	memset(t_strBeginTime,0,14);
		//得到前一秒
		TimeUtil::addSeconds(t_strBeginTime,r_beginDate.c_str(),-1); 
		t_beginDate = t_strBeginTime;	
		 
      vector<UserServ>::const_iterator  iters;
        for (iters=(*m_pUserServVector).begin(); iters!=(*m_pUserServVector).end(); iters++)
    {
        if (iters->m_servId==r_servId    &&
          iters->m_startDate<t_beginDate   &&
          iters->m_endDate==t_beginDate     &&
          iters->m_startDate<iters->m_endDate
          )
      {
          return false;
      }
    }

  return true;
}

/**
* judgeServEnd判断用户服务结束时间是否在本月上下半月
* 上半月结束 r_beginDate = m_cycParam.m_beginTime ,r_endDate = m_cycParam.m_halfTime
* 下半月结束 r_beginDate = m_cycParam.m_halfTime  ,r_endDate = m_cycParam.m_endTime  
* 且r_endDate 的后一秒不存在开始的相同服务（为产品变更)
* @param  r_servId  服务ID
* @param  r_beginDate  开始时间
* @param  r_endDate    结束时间
* @return true：是 false：否 
*/     
bool ComBill::judgeServEnd(const int &r_servId,const T_DATE_TIME &r_beginDate,const T_DATE_TIME &r_endDate)
{
      vector<UserServ>::const_iterator  iters;
        for (iters=(*m_pUserServVector).begin(); iters!=(*m_pUserServVector).end(); iters++)
    {
        if (iters->m_servId==r_servId     &&
          iters->m_startDate<r_endDate    &&
          iters->m_endDate>=r_beginDate    &&
          iters->m_endDate<=r_endDate      &&
          iters->m_startDate<iters->m_endDate&&
          nextTimesNotExistServer(r_servId,iters->m_endDate)
          )
      {
          return true;
      }
    }

  return false;
}

/**
* nextTimesNotExistServer判断后一秒是否存在相同服务
* @param  r_servId  服务ID
* @param  r_beginDate  开始时间
* @return true：不存在 false：存在 
*/  
bool  ComBill::nextTimesNotExistServer(const int &r_servId,const T_DATE_TIME &r_beginDate)
{
		T_DATE_TIME t_beginDate ;
		char t_strBeginTime[14+1];
    	memset(t_strBeginTime,0,14);
		
		//得到后一秒
		TimeUtil::addSeconds(t_strBeginTime,r_beginDate.c_str(),1); 
		t_beginDate=t_strBeginTime;
		 
      vector<UserServ>::const_iterator  iters;
        for (iters=(*m_pUserServVector).begin(); iters!=(*m_pUserServVector).end(); iters++)
    {
        if (iters->m_servId==r_servId    &&
          iters->m_startDate==t_beginDate  &&
          iters->m_endDate>t_beginDate   &&
          iters->m_startDate<iters->m_endDate
          )
      {
          return false;
      }
    }

  return true;
}

//bool ComBill::judgeServStartFirstHalf(const int &r_attrValue)
//{
//    vector<UserServ>::const_iterator  iters;
//    for (iters=(*m_pUserServVector).begin(); iters!=(*m_pUserServVector).end(); iters++)
//    {
//        if (iters->m_servId==r_attrValue                &&
//          iters->m_startDate<m_cycParam.m_halfTime      &&
//          iters->m_startDate>=m_cycParam.m_beginTime    &&
//          iters->m_endDate>m_cycParam.m_beginTime       &&
//          iters->m_startDate<iters->m_endDate)
//      {
//          return true;
//      }
//    }
//
//  return false;
//}
//
//bool ComBill::judgeServStartLastHalf(const int &r_attrValue)
//{
//      vector<UserServ>::const_iterator  iters;
//        for (iters=(*m_pUserServVector).begin(); iters!=(*m_pUserServVector).end(); iters++)
//    {
//        if (iters->m_servId==r_attrValue            &&
//          iters->m_startDate>=m_cycParam.m_halfTime &&
//          iters->m_startDate<m_cycParam.m_endTime   &&
//          iters->m_endDate>m_cycParam.m_beginTime   &&
//          iters->m_startDate<iters->m_endDate)
//      {
//          return true;
//      }
//    }
//
//  return false;
//}
//
//bool ComBill::judgeServEndFirstHalf(const int &r_attrValue)
//{
//      vector<UserServ>::const_iterator  iters;
//      for (iters=(*m_pUserServVector).begin(); iters!=(*m_pUserServVector).end(); iters++)
//    {
//        if (iters->m_servId==r_attrValue            &&
//          iters->m_startDate<m_cycParam.m_endTime   &&
//          iters->m_endDate>m_cycParam.m_beginTime   &&
//          iters->m_endDate<m_cycParam.m_halfTime    &&
//          iters->m_startDate<iters->m_endDate)
//      {
//          return true;
//      }
//    }
//
//  return false;
//}
//
//bool ComBill::jud