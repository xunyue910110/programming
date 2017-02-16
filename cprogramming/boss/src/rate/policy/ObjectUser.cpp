#include "ObjectUser.h"
#include "base/exceptions.h"
#include "base/TimeUtil.h"
#include "RateGlobalInfo.h"

extern RateGlobalInfo g_globalInfo;
IdInfo * ObjectUser::m_idInfo = &g_globalInfo.m_idInfo;

FuncGetAttr ObjectUser::m_funcGetAttr[] =
{  
	ObjectUser::getCycleId       ,      //ATTR_CYCLE_ID        
	ObjectUser::getMonthEndTag   ,      //ATTR_MOTNTH_END_TAG  
	ObjectUser::getMonthHalfTime ,      //ATTR_MONTH_HALF_TIME 
	ObjectUser::getMonthBeginTime,      //ATTR_MONTH_BEGIN_TIME
	ObjectUser::getMonthEndTime  ,      //ATTR_MONTH_ENN_TIME  
	ObjectUser::getFirstCallTime ,      //ATTR_FIRST_CALL_TIME 
	ObjectUser::getTpBeginTime   ,      //ATTR_TP_BEGIN_TIME   
	ObjectUser::getTpEndTime     ,      //ATTR_TP_END_TIME     
	ObjectUser::getGroupMemberNum,      //ATTR_GROUP_MEMBER_NUM
	ObjectUser::getUserNumByAcctId,     //ATTR_ACCOUNT_USER_NUM
	ObjectUser::getCurStateSet,         //ATTR_CUR_STATESET 
	ObjectUser::isMonthEnd,             //ATTR_IS_MOTNTH_END
	ObjectUser::tpDurationMonths,       //ATTR_TP_DURATION_MONTHS
	ObjectUser::getLastState,            //ATTR_LAST_TIME_STATE
  // add by yuxk begin 2009-9-27 20:45:27          
  	ObjectUser::getLastITime,            //ATTR_LASTSTOP_TIME_I
  	ObjectUser::IDurationMonths,          //ATTR_I_DURATION_MONTHS
  	ObjectUser::getLastStopOpenTime,            //ATTR_LASTSTOPOPEN_TIME
	// add by yuxk end 2009-9-27 20:45:27       
	ObjectUser::getUserNumByDetailBill,   //ATTR_MEMBER_NUM  
	ObjectUser::lastStopDurationMonths,	    //ATTR_LASTSTOP_DURATION_MONTHS
	ObjectUser::stopStateDurationMonths,    //ATTR_STOPSTATE_DURATION_TIME
	ObjectUser::adjustDurationMonths,        //ATTR_ADJUST_DURATION_MONTHS
//add by yuxk for shanxi begin
	ObjectUser::getMonthDayNum,	    //ATTR_MONTH_DAY_NUM
	ObjectUser::getActualNumberOfDays,	    //ATTR_ACTUAL_NUMBER_OF_DAYS
	ObjectUser::getCurrentActualNumberOfDays,	    //ATTR_CURRENT_ACTUAL_NUMBER_OF_DAYS
//add by yuxk for shanxi end 
	ObjectUser::getOnlineMonths,        //ATTR_ONLINE_MONTHS	  
	ObjectUser::getValidTpDays,        //ATTR_VALID_TP_DAYS add by taijq 2009-12-15 
	ObjectUser::getTpValidMonthNum, //ATTR_TP_MONTHS add by xuf 2009-12-19 10:52:35
	ObjectUser::getFinishDelayMonths, //ATTR_FINISH_DELAY_MONTHS add by xuf 2010-1-3 22:14:28
	ObjectUser::getFinishValidDays,   //ATTR_FINISH_VALID_DAYS add by xuf 2010-1-3 22:14:28	
  ObjectUser::getPostPaidAcctTag,    //ATTR_POSTPAID_ACCT_TAG 
  	ObjectUser::getLastITimenew,            //ATTR_LASTSTOP_TIME_I_NEW
  	ObjectUser::getLastStopOpenTimenew,            //ATTR_LASTSTOPOPEN_TIME_NEW
  ObjectUser::getMinUserIdByRole, //ATTR_MINUSERID_BYROLE	
  	ObjectUser::judgeGroupUsersStatsIsStop,  //ATTR_GROUPSTATS_ISSTOP add by yangyb 2010-10-25
  	ObjectUser::getGroupMemberNumByROle  //ATTR_GROUP_MEMBER_NUM_ROLE add by yangyb 20101101	
};  
    
bool ObjectUser::getObjectValue(T_OBJECT_ATTR &value)
{	
	if (m_key<0 || m_key>=ATTR_MAX)
	{
		THROW_C_P3(BusinessException, -1, "用户对象错误,非法的下标值!"
    	"object_id=%d,attr_id=%d,key=%d", m_id, m_attrId, m_key);		
	}
	
	if (m_key<IdInfo::ID_ATTR_MAX)
	{
		if (m_key==IdInfo::ID_LASTSTOP_TIME)
			getLastStopTime();
			
		m_idInfo->getIdAttr(m_key, value);
	}
	else if(m_key>ATTR_MIN)
	{
		m_funcGetAttr[m_key-ATTR_MIN-1](value,this);
	}
	else
	{
		THROW_C_P3(BusinessException, -1, "用户对象错误,非法的下标值!"
    	"object_id=%d,attr_id=%d,key=%d", m_id, m_attrId, m_key);		
	}
	return true;
}

bool ObjectUser::getObjectValue(long &value)
{
	T_OBJECT_ATTR attrValue;
	if (m_key<0 || m_key>=ATTR_MAX)
	{
		THROW_C_P3(BusinessException, -1, "用户对象错误,非法的下标值!"
    	"object_id=%d,attr_id=%d,key=%d", m_id, m_attrId, m_key);		
	}
	
	if (m_key<IdInfo::ID_ATTR_MAX)
	{
		if(m_key==IdInfo::ID_LASTSTOP_TIME)
			getLastStopTime();
			
		m_idInfo->getIdAttr(m_key, attrValue);
	}
	else if(m_key>ATTR_MIN)
	{
		m_funcGetAttr[m_key-ATTR_MIN-1](attrValue,this);
	}
	else
	{
		THROW_C_P3(BusinessException, -1, "用户对象错误,非法的下标值!"
    	"object_id=%d,attr_id=%d,key=%d", m_id, m_attrId, m_key);		
	}
	value = attrValue.value();
	return true;	
}

void ObjectUser::dump(char *prefix)
{
	char myPrefix[256];	
	strcpy(myPrefix, prefix);	
	strcat(myPrefix,"  ");
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"---- ObjectUser : " <<m_id<<" ----" <<endl;	
	ObjectBase::dump(myPrefix);
	cout <<myPrefix<<"m_key        = "<< m_key <<endl;	
	cout <<myPrefix<<"}" <<endl;
}

void ObjectUser::getCycleId   (T_OBJECT_ATTR &attrValue, 
                               const ObjectUser *objectUser)
{
	attrValue = m_globalInfo->m_cycParam.m_cycleId;
}

void ObjectUser::getMonthEndTag   (T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	attrValue = m_globalInfo->m_cycParam.m_addTag;
}

void ObjectUser::getMonthHalfTime (T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	attrValue = m_globalInfo->m_cycParam.m_halfTime;
}

void ObjectUser::getMonthBeginTime(T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	attrValue = m_globalInfo->m_cycParam.m_beginTime;
}

void ObjectUser::getMonthEndTime  (T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	attrValue = m_globalInfo->m_cycParam.m_endTime;
}

void ObjectUser::getFirstCallTime (T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	attrValue = m_globalInfo->m_firstCallTime;
}

void ObjectUser::getTpBeginTime   (T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	attrValue = m_policyInfo->m_rateTpInfo->m_pUserTp->m_beginTime;
}

void ObjectUser::getTpEndTime     (T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	attrValue = m_policyInfo->m_rateTpInfo->m_pUserTp->m_endTime;
}

void ObjectUser::getGroupMemberNum(T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	int MemberNum;
	set<T_SERVER_ID> userIds;
	T_SERVER_ID groupId;
	
	if (objectUser==NULL)
		THROW_C(BusinessException, -1, "用户对象错误,getGroupMemberNum(),objectUser is NULL!");
	
	if (m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType == (char)GROUP_ID_TYPE)
	{
		groupId = m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId;
	}
	else if (m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType == (char)USER_ID_TYPE)
	{
		IdInfo r_userInfo;
			if(m_globalInfo->m_paramMgr->getUserInfo(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId,r_userInfo)>0)
			{
				 if(r_userInfo.m_idAttr[IdInfo::ID_TYPE_CODE]==3)
				 {
				 	  groupId = m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId;
				 }
				 else
				 {
				    THROW_C(BusinessException, -1, "用户对象错误,getGroupMemberNum(),资费实例挂的ID不是虚拟用户!");
        }
			}
	}
	else if(m_policyInfo->m_rateTpInfo->m_pUserTp->m_relationVpnId != 
	        NULL_VPN_ID)
	{
		groupId = m_policyInfo->m_rateTpInfo->m_pUserTp->m_relationVpnId;
	}
	else
	{
		THROW_C_P3(BusinessException, -1, "用户对象错误,getGroupMemberNum(),groupId is NULL!"
		"object_id=%d,attr_id=%d,key=%d", objectUser->m_id, objectUser->m_attrId, objectUser->m_key);		
	}
	
	map<T_SERVER_ID,T_MEMBERINFO_TYPE>  userIdMap;
	T_MEMBER_TYPE memberRoleCode = "0";
	MemberNum = m_globalInfo->m_paramMgr->getUserInfoByVpnId(groupId,					   
	                   m_globalInfo->m_cycParam, 
	                   memberRoleCode, userIdMap);
    #ifdef _DEBUG_
  	cout<<"集团下成员数MemberNum="<<MemberNum<<endl;
  	#endif
  	if (MemberNum==-1)
  	{
  		if (objectUser!=NULL)
		{
  			THROW_C_P3(BusinessException, -1, "用户对象错误,getGroupMemberNum.getUserInfoByVpnId(),MDB异常!"
  			"object_id=%d,attr_id=%d,key=%d", objectUser->m_id, objectUser->m_attrId, objectUser->m_key);
  		}
  		else
  			THROW_C(BusinessException, -1, "用户对象错误,getGroupMemberNum.getUserInfoByVpnId(),groupId is NULL!");
  	}
	attrValue = MemberNum;
}

void ObjectUser::getCurStateSet   (T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	m_globalInfo->m_comBill.getCurStateSet(attrValue);
}

void ObjectUser::getLastState   (T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	m_globalInfo->m_comBill.getLastState(attrValue);
}

void ObjectUser::getUserNumByAcctId(T_OBJECT_ATTR &attrValue, 
                                    const ObjectUser *objectUser)
{
	int MemberNum;
	map<T_SERVER_ID,int> userOrderMap;
	
	if (m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType == (char)ACCT_ID_TYPE)
	{
		MemberNum = m_globalInfo->m_paramMgr->getUsersByAcctId(
	    m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId, 
	    	m_globalInfo->m_cycParam.m_cycleId, userOrderMap);
	}	
	else
	{
		if (objectUser!=NULL)
			THROW_C_P3(BusinessException, -1, "用户对象错误,getUserIdByAcctId(),非账户级TP!"
    		"object_id=%d,attr_id=%d,key=%d", objectUser->m_id, objectUser->m_attrId, objectUser->m_key);
    	else
    		THROW_C(BusinessException, -1, "用户对象错误,getUserIdByAcctId(),非账户级TP!");
	}
	
  	
  	if (MemberNum==-1)
  	{
  		if (objectUser!=NULL)
			THROW_C_P3(BusinessException, -1, "用户对象错误,getUserIdByAcctId(),MDB异常!"
    		"object_id=%d,attr_id=%d,key=%d", objectUser->m_id, objectUser->m_attrId, objectUser->m_key);
    	else
    		THROW_C(BusinessException, -1, "用户对象错误,getUserIdByAcctId(),MDB异常!");
  	}
	attrValue = MemberNum;
}

void ObjectUser::getLastStopTime  ()
{
	m_globalInfo->m_comBill.getLastStopTime(m_globalInfo->m_idInfo);
}

void ObjectUser::isMonthEnd(T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	if(m_globalInfo->m_comBill.m_isMonthEnd)
		attrValue = '1';
	else
		attrValue = '0';
}

void ObjectUser::tpDurationMonths(T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	attrValue = (long) (TimeUtil::monthdiff(
		m_policyInfo->m_rateTpInfo->m_pUserTp->m_beginTime.substr(0,6).c_str(),
		m_globalInfo->m_cycParam.m_beginTime.substr(0,6).c_str() ) + 1);
}

void ObjectUser::getUserNumByDetailBill(T_OBJECT_ATTR &attrValue, 
                                    const ObjectUser *objectUser)
{
	int MemberNum=0;
	
	MemberNum=m_globalInfo->m_comBill.m_detailBill.usernum();
	if (MemberNum<=0)
		THROW_C(BusinessException, -1, "用户对象错误,getUserNumByDetailBill(),群或账户下用户数<=0!");
	
	attrValue = MemberNum;
}
// add by yuxk begin 2009-9-27 20:45:27          
  void ObjectUser::getLastITime   (T_OBJECT_ATTR &attrValue, 
                               const ObjectUser *objectUser)
{
  m_globalInfo->m_comBill.getLastITime(attrValue,true);
}

  void ObjectUser::getLastITimenew   (T_OBJECT_ATTR &attrValue, 
                               const ObjectUser *objectUser)
{
  m_globalInfo->m_comBill.getLastITimenew(attrValue);
}


void ObjectUser::IDurationMonths(T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	m_globalInfo->m_comBill.getLastITime(attrValue);
	attrValue = (long) (TimeUtil::monthdiff(
		attrValue.substr(0,6).c_str(),
		m_globalInfo->m_cycParam.m_beginTime.substr(0,6).c_str() )+1);
	if (attrValue < 0)
	attrValue = -1;	
}

  void ObjectUser::getLastStopOpenTime   (T_OBJECT_ATTR &attrValue, 
                               const ObjectUser *objectUser)
{
  m_globalInfo->m_comBill.getLastStopOpenTime(attrValue,true);//modify by xuf 2010-1-13 23:34:47
}

void ObjectUser::getLastStopOpenTimenew   (T_OBJECT_ATTR &attrValue, 
                               const ObjectUser *objectUser)
{
  m_globalInfo->m_comBill.getLastStopOpenTime(attrValue);//modify by xuf 2010-1-13 23:34:47
}


void ObjectUser::lastStopDurationMonths(T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	attrValue = (long) (TimeUtil::monthdiff(
		m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_LASTSTOP_TIME).substr(0,6).c_str(),
		m_globalInfo->m_cycParam.m_beginTime.substr(0,6).c_str() ) + 1);
}

// add by yuxk end 2009-9-27 20:45:27          
//added by dingr  2009-10-21

void ObjectUser::stopStateDurationMonths(T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	m_globalInfo->m_comBill.getLastServStopTime(attrValue);
	attrValue = (long)TimeUtil::monthdiff(
		attrValue.substr(0,6).c_str(),
		m_globalInfo->m_cycParam.m_beginTime.substr(0,6).c_str());
	if (attrValue < 0)
	attrValue = -1;	
	#ifdef DEBUG 
	cout<<"attrValue:"<<attrValue<<endl;
    #endif
} 

void ObjectUser::adjustDurationMonths(T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	attrValue = (long) (TimeUtil::monthdiff(
		m_globalInfo->m_comBill.m_discntInfo.m_startDate.substr(0,6).c_str(),
		m_globalInfo->m_cycParam.m_beginTime.substr(0,6).c_str() ) + 1);
}

//add by yuxk for shanxi begin
void ObjectUser::getMonthDayNum  (T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	attrValue = m_globalInfo->m_cycParam.m_endTime.substr(6,2).c_str() ;
}

void ObjectUser::getActualNumberOfDays  (T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{	  
	  //add by xuf 2010-1-15 22:05:41
    //防止数据导换表异动信息表未导全的bug
    //如果结束时间前已经拆机（拆机复装 拆机时间会清零) , 取拆机时间为结束时间。
    //防止主表装机时间和状态异动信息表不一致
    T_DATE_TIME   t_beginTime=m_globalInfo->m_cycParam.m_beginTime;
    T_DATE_TIME   t_endTime=m_globalInfo->m_cycParam.m_endTime;
    T_DATE_TIME   t_destroyDate;
    T_OBJECT_ATTR t_removeTag;
    T_DATE_TIME  t_openDate;
    t_openDate = m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
    t_removeTag = m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_REMOVE_TAG);	
    t_destroyDate = m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_REMOVE_DATE);	
    if (t_destroyDate.value()==0)	t_destroyDate = "20501231000000";  	
	  if (t_removeTag.value()!=0&&
	  	  t_destroyDate<m_globalInfo->m_cycParam.m_endTime&&
	  	  t_destroyDate>=m_globalInfo->m_cycParam.m_beginTime)//当月销户
	  	   t_endTime = t_destroyDate;
	   	
	   if (t_openDate > m_globalInfo->m_cycParam.m_beginTime)//当月装机
     {
        t_beginTime = t_openDate;
     } 	
	   	
	  int t_iDays = (m_globalInfo->m_comBill).getVaildMonthDays(t_beginTime,t_endTime);	
	
		 strstream t_ss;
     string t_sDays;
     t_ss << t_iDays;
     t_ss >> t_sDays;
        
     attrValue = t_sDays;
			
}
void ObjectUser::getCurrentActualNumberOfDays  (T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
    //add by xuf 2010-1-15 22:05:41
    //防止数据导换表异动信息表未导全的bug
    //如果结束时间前已经拆机（拆机复装 拆机时间会清零) , 取拆机时间为结束时间。
    T_DATE_TIME   t_beginTime=m_globalInfo->m_cycParam.m_beginTime;
    T_DATE_TIME   t_endTime=m_globalInfo->m_cycParam.m_endTime;
    T_DATE_TIME   t_destroyDate;
    T_OBJECT_ATTR t_removeTag;
    T_DATE_TIME  t_openDate;
    t_openDate = m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
    t_removeTag = m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_REMOVE_TAG);	
    t_destroyDate = m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_REMOVE_DATE);	
    if (t_destroyDate.value()==0)	t_destroyDate = "20501231000000";  	
	  if (t_removeTag.value()!=0&&
	  	  t_destroyDate<m_globalInfo->m_cycParam.m_endTime&&
	  	  t_destroyDate>=m_globalInfo->m_cycParam.m_beginTime)//当月销户
	  	   t_endTime = t_destroyDate;
    
    T_DATE_TIME   curDateTime;
    TimeUtil::now((char *)curDateTime.c_str());
   
    if (t_endTime > curDateTime)
    {
        t_endTime = curDateTime;
    }
    
     if (t_openDate > m_globalInfo->m_cycParam.m_beginTime)//当月装机
     {
        t_beginTime = t_openDate;
     } 	  
    
    if((m_globalInfo->m_paramMgr->m_stopCalcRentDate >= m_globalInfo->m_cycParam.m_beginTime)
     &&(m_globalInfo->m_paramMgr->m_stopCalcRentDate <= t_endTime))
    {
        //以stopDate为结束时间
        t_endTime = m_globalInfo->m_paramMgr->m_stopCalcRentDate;
        t_endTime=t_endTime.substr(0,8)+"235959";
    }
    	
    int t_iDays = (m_globalInfo->m_comBill).getVaildMonthDays(t_beginTime,t_endTime);	
	
		 strstream t_ss;
     string t_sDays;
     t_ss << t_iDays;
     t_ss >> t_sDays;
        
     attrValue = t_sDays;	
    	    
}
//add by yuxk for shanxi end
//在网月数 add by zhaogc                         
void ObjectUser::getOnlineMonths(T_OBJECT_ATTR &attrValue,
                                   const ObjectUser *objectUser)
{
  T_OBJECT_ATTR openDate = m_idInfo->getIdAttr(IdInfo::ID_OPEN_DATE);
  attrValue = m_idInfo->getIdAttr(IdInfo::ID_REMOVE_DATE);

  if(attrValue=="0" || attrValue>m_globalInfo->m_cycParam.m_endTime)
    attrValue = m_globalInfo->m_cycParam.m_endTime;

  attrValue = (long) (TimeUtil::monthdiff(
                openDate.substr(0,6).c_str(),   attrValue.substr(0,6).c_str() ) + 1);
}

//add by taijq begin 2009-12-15
void ObjectUser::getValidTpDays(T_OBJECT_ATTR &attrValue,const ObjectUser *objectUser)
{
				T_DATE_TIME beginTime = m_globalInfo->m_cycParam.m_beginTime,
                    endTime   = m_globalInfo->m_cycParam.m_endTime;
        
        T_OBJECT_ATTR & openDate = 
                 m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE);
        if(openDate > m_globalInfo->m_cycParam.m_beginTime&&
           openDate <= m_globalInfo->m_cycParam.m_endTime)
            beginTime = openDate;
        else if (openDate > m_globalInfo->m_cycParam.m_endTime) 
        {
        	beginTime = m_globalInfo->m_cycParam.m_endTime;
        }   
        else
          beginTime = m_globalInfo->m_cycParam.m_beginTime;

        T_OBJECT_ATTR & destroyDate = 
                 m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_PREDESTROY_TIME);
        if((destroyDate>m_globalInfo->m_cycParam.m_beginTime&&
            destroyDate<=m_globalInfo->m_cycParam.m_endTime))
            endTime = destroyDate; 
        else if(destroyDate<=m_globalInfo->m_cycParam.m_beginTime) 
        {
        	  endTime=m_globalInfo->m_cycParam.m_beginTime;
        }  
        else
            endTime = m_globalInfo->m_cycParam.m_endTime;
            
        //资费未生效的不会被取到         
        if(m_policyInfo->m_rateTpInfo->m_pUserTp->m_beginTime >
           beginTime)
            beginTime = m_policyInfo->m_rateTpInfo->m_pUserTp->m_beginTime;
         
        //资费失效的不会被取到      
        if(m_policyInfo->m_rateTpInfo->m_pUserTp->m_endTime <
           endTime)
            endTime = m_policyInfo->m_rateTpInfo->m_pUserTp->m_endTime;

         if (beginTime>endTime)
         { 
           beginTime = endTime;  
         }

			long daynum = (long) (TimeUtil::daydiff(beginTime.c_str(),endTime.c_str()))+1;
	 
	 	    strstream t_ss;
       	string t_sDayNum;
       	t_ss << daynum;
       	t_ss >> t_sDayNum;
       	
       	attrValue = t_sDayNum;
}
//add by taijq end 2009-12-15

//资费的周期数（结束月-起始月+1) add by xuf 2009-12-19 10:48:51
void ObjectUser::getTpValidMonthNum(T_OBJECT_ATTR &attrValue, 
                                   		const ObjectUser *objectUser)
{
		long 		t_attrValue = (long) (TimeUtil::monthdiff(
						m_policyInfo->m_rateTpInfo->m_pUserTp->m_beginTime.substr(0,6).c_str(),
						m_policyInfo->m_rateTpInfo->m_pUserTp->m_endTime.substr(0,6).c_str() ) + 1);
			
		 strstream t_ss;
     string t_sMonthNum;
     t_ss << t_attrValue;
     t_ss >> t_sMonthNum;
        
     attrValue = t_sMonthNum;
}

//完工工单延迟月数(工单录入时间所在月-竣工时间所在月) add by xuf 2010-1-3 22:22:19
void ObjectUser::getFinishDelayMonths(T_OBJECT_ATTR &attrValue,
                           						const ObjectUser *objectUser)
{
	 long 		t_attrValue = (long) (TimeUtil::monthdiff(
						m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE).substr(0,6).c_str(),
						m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_IN_DATE).substr(0,6).c_str()));
		
	 if (t_attrValue<0) t_attrValue =0; //防止资料异常	
			
		 strstream t_ss;
     string t_sMonthNum;
     t_ss << t_attrValue;
     t_ss >> t_sMonthNum;
     
     attrValue = t_sMonthNum;
	
}

//竣工当月用户使用的有效天数（月底时间所在日-竣工时间所在日+1）add by xuf 2010-1-3 22:23:10
void ObjectUser::getFinishValidDays(T_OBJECT_ATTR &attrValue,
												const ObjectUser *objectUser)
{
		long daynum = (long) (TimeUtil::daydiff(m_globalInfo->m_idInfo.getIdAttr(IdInfo::ID_OPEN_DATE).c_str(),m_globalInfo->m_cycParam.m_endTime.c_str()))+1;
	
	 	strstream t_ss;
    string t_sDayNum;
    t_ss << daynum;
    t_ss >> t_sDayNum;
       	
    attrValue = t_sDayNum;
	
}
	
//取后付费用户出帐标识
void ObjectUser::getPostPaidAcctTag(T_OBJECT_ATTR &attrValue,const ObjectUser *objectUser)
{
	attrValue = m_globalInfo->m_cycParam.m_fileCreateTag;
}

void ObjectUser::getMinUserIdByRole(T_OBJECT_ATTR &attrValue,const ObjectUser *objectUser)
{
	int MemberNum;
	T_SERVER_ID groupId, MinUserId;
	T_MEMBER_TYPE rateRoleCode;
	map<T_SERVER_ID,T_MEMBERINFO_TYPE>  userIdMap;	

#ifdef _DEBUG_
      cout<<__FILE__<<__LINE__<<
          " m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType="<<m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType<<
          ",m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId="<<m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId<<
          ",m_policyInfo->m_rateTpInfo->m_rateRoleCode ="<< m_policyInfo->m_rateTpInfo->m_rateRoleCode <<endl;
#endif	

	if (objectUser==NULL)
		THROW_C(BusinessException, -1, "用户对象错误,getMinUserIdByRole(),objectUser is NULL!");
	
	if (m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType != (char)USER_ID_TYPE)
		THROW_C(BusinessException, -1, "用户对象错误,UserTp->m_idType !=USER_ID_TYPE  !");
	
	groupId = m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId;
	
	MemberNum = m_globalInfo->m_paramMgr->getUserInfoByVpnId(groupId,					   
	                   m_globalInfo->m_cycParam, 
	                   m_policyInfo->m_rateTpInfo->m_rateRoleCode, userIdMap);

	if (MemberNum == 0)
		THROW_C(BusinessException, -1, "用户对象错误,getMinUserIdByRole(),Not the role member!");

	MinUserId= userIdMap.begin()->first;
	for (map<T_SERVER_ID,T_MEMBERINFO_TYPE>::iterator it = userIdMap.begin(); it != userIdMap.end();
		it++)
	{
		if (it->first < MinUserId)
			MinUserId=it->first;
	}


	attrValue=MinUserId;
}

void  ObjectUser::judgeGroupUsersStatsIsStop(T_OBJECT_ATTR &attrValue, 
                                   const ObjectUser *objectUser)
{
	  int MemberNum;
	  int IsAllStop=1;
	  T_OBJECT_ATTR attrVal;
	  T_SERVER_ID groupId;
	  if (objectUser==NULL)
		THROW_C(BusinessException, -1, "用户对象错误,judgeGroupUsersStatsIsStop(),objectUser is NULL!");
	  if (m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType == (char)USER_ID_TYPE)
		{
			IdInfo r_userInfo;
			if(m_globalInfo->m_paramMgr->getUserInfo(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId,r_userInfo)>0)
			{
				 if(r_userInfo.m_idAttr[IdInfo::ID_TYPE_CODE]==3)
				 {
				 	  groupId = m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId;
				 }
				 else
				 {
				    THROW_C(BusinessException, -1, "用户对象错误,judgeGroupUsersStatsIsStop(),资费实例挂的ID不是虚拟用户!");
        }
			}
			
		}
		else
		{
			THROW_C_P3(BusinessException, -1, "用户对象错误,judgeGroupUsersStatsIsStop(),m_pUserTp->m_idType error!"
			"object_id=%d,attr_id=%d,key=%d", objectUser->m_id, objectUser->m_attrId, objectUser->m_key);		
		}
		map<T_SERVER_ID,T_MEMBERINFO_TYPE>  userIdMap;
		T_MEMBER_TYPE memberRoleCode = "0";
		MemberNum = m_globalInfo->m_paramMgr->getUserInfoByVpnId(groupId,					   
	                   m_globalInfo->m_cycParam, 
	                   memberRoleCode, userIdMap);
    
  	if (MemberNum==-1)
  	{
  		if (objectUser!=NULL)
		  {
  			THROW_C_P3(BusinessException, -1, "用户对象错误,judgeGroupUsersStatsIsStop.getUserInfoByVpnId(),MDB异常!"
  			"object_id=%d,attr_id=%d,key=%d", objectUser->m_id, objectUser->m_attrId, objectUser->m_key);
  		}
  		else
  			THROW_C(BusinessException, -1, "用户对象错误,judgeGroupUsersStatsIsStop.getUserInfoByVpnId(),groupId is NULL!");
  	}
  	else if(MemberNum>=1)
  	{
  		 for(map<T_USER_ID,T_MEMBERINFO_TYPE>::iterator t_iterMap=userIdMap.begin();
			 t_iterMap!=userIdMap.end();++t_iterMap)
			{
					//m_globalInfo->m_comBill.getLastState(attrVal);
					vector<UserServState> r_userServStates;
	        #ifdef _DEBUG_
      			cout<<endl<<endl<<__FILE__<<__LINE__<<
          	" USER_ID="<<t_iterMap->first<<endl;
					#endif
	        if(m_globalInfo->m_paramMgr->getUserServState(t_iterMap->first,r_userServStates)>0)
	        {
	        	 for (int m=0; m < r_userServStates.size(); ++m)
  	  			{
	        		#ifdef _DEBUG_
      					cout<<__FILE__<<__LINE__<<
      					"  r_userServStates[m].m_servStateCode="<< r_userServStates[m].m_servStateCode << 
          			"  r_userServStates[m].m_startDate="<<r_userServStates[m].m_startDate<< 
          			"  r_userServStates[m].m_endDate="<<r_userServStates[m].m_endDate <<endl;
							#endif
  	  				 if (r_userServStates[m].m_startDate <= (m_globalInfo->m_cycParam).m_endTime
  	  				 	&& r_userServStates[m].m_endDate >= (m_globalInfo->m_cycParam).m_beginTime)
	      	     {
	      	     	   if(r_userServStates[m].m_servStateCode != '1' &&
	      	     	   	  r_userServStates[m].m_servStateCode != 'P' &&
	      	     	   	  r_userServStates[m].m_servStateCode != 'Q' &&
	      	     	   	  r_userServStates[m].m_servStateCode != '6' &&
	      	     	   	  r_userServStates[m].m_servStateCode != '9' &&
	      	     	   	  r_userServStates[m].m_servStateCode != '8' &&
	      	     	   	  r_userServStates[m].m_servStateCode != 'H'
	      	     	   	 )
									 {
						  				IsAllStop=0;
						  				break;
									 }
	      	     }
  	  			}
	        }
	        if (IsAllStop==0)
	        	break;
			}	
  	}
  	attrValue=IsAllStop;
}


void ObjectUser::getGroupMemberNumByROle(T_OBJECT_ATTR &attrValue,const ObjectUser *objectUser)
{
	int MemberNum;
	set<T_SERVER_ID> userIds;
	T_SERVER_ID groupId;
	
	T_TP_ID m_paramId=11111111;
  const char *paramValue;
  paramValue = m_policyInfo->m_rateTpInfo->getParamValue(m_paramId);
  if (paramValue == NULL)
	{
			THROW_C_P2(BusinessException, -1, "条件配置错误,未找到替换参数："
			                                  "objectUser->m_id=%d,m_paramId=%s",
			                                   objectUser->m_id, m_paramId);
	}
	T_MEMBER_TYPE memberRoleCode = paramValue ;

	if (objectUser==NULL)
		THROW_C(BusinessException, -1, "用户对象错误,getGroupMemberNum(),objectUser is NULL!");
	
	if (m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType == (char)USER_ID_TYPE)
	{
		IdInfo r_userInfo;
			if(m_globalInfo->m_paramMgr->getUserInfo(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId,r_userInfo)>0)
			{
				 if(r_userInfo.m_idAttr[IdInfo::ID_TYPE_CODE]==3)
				 {
				 	  groupId = m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId;
				 }
				 else
				 {
				    THROW_C(BusinessException, -1, "用户对象错误,judgeGroupUsersStatsIsStop(),资费实例挂的ID不是虚拟用户!");
        }
			}
	}
	else
	{
		THROW_C_P3(BusinessException, -1, "用户对象错误,getGroupMemberNum(),groupId is NULL!"
		"object_id=%d,attr_id=%d,key=%d", objectUser->m_id, objectUser->m_attrId, objectUser->m_key);		
	}
	
	map<T_SERVER_ID,T_MEMBERINFO_TYPE>  userIdMap;
	//T_MEMBER_TYPE memberRoleCode = "0";
	MemberNum = m_globalInfo->m_paramMgr->getUserInfoByVpnId(groupId,					   
	                   m_globalInfo->m_cycParam, 
	                   memberRoleCode, userIdMap);
    
  	#ifdef _DEBUG_
  	cout<<"集团下指定角色成员数MemberNum="<<MemberNum<<endl;
  	#endif
  	if (MemberNum==-1)
  	{
  		if (objectUser!=NULL)
		{
  			THROW_C_P3(BusinessException, -1, "用户对象错误,getUserInfoByVpnId(),MDB异常!"
  			"object_id=%d,attr_id=%d,key=%d", objectUser->m_id, objectUser->m_attrId, objectUser->m_key);
  		}
  		else
  			THROW_C(BusinessException, -1, "用户对象错误,getUserInfoByVpnId(),groupId is NULL!");
  	}
	attrValue = MemberNum;
}

