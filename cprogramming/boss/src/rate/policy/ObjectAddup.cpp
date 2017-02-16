#include "ObjectAddup.h"
#include "AddupItem.h"
#include "PolicyInfo.h"
#include "RateGlobalInfo.h"
#include "base/exceptions.h"

void ObjectAddup::dump(char *prefix)
{
	char myPrefix[256] = "  ";	
	strcat(myPrefix,prefix);
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"---- ObjectAddup : " <<m_id<<" ----" <<endl;
	ObjectBase::dump(myPrefix);
	cout <<myPrefix<<"m_attrSource = "<< m_attrSource <<endl;	
	cout <<myPrefix<<"m_startNum   = "<< m_startNum <<endl;
	cout <<myPrefix<<"m_endNum     = "<< m_endNum<<endl;
	cout <<myPrefix<<"m_roleCode   = "<< m_roleCode<<endl;
	if (m_addupItem==0)
		cout <<myPrefix<<"m_addupItem     = NULL"<<endl;
	else
	{
		cout <<myPrefix<<"m_addupItem     != NULL"<<endl;
		m_addupItem->dump(myPrefix);
	}
	cout <<myPrefix<<"}" <<endl;
}

bool ObjectAddup::getObjectValue(T_OBJECT_ATTR &value)
{
	long intValue;
	getObjectValue(intValue);
	value = intValue;
	return true;
}

bool ObjectAddup::getObjectValue(long &value)
{
	if (m_addupItem != NULL)
	{
		long sumBeginTime = 0;
    long sumEndTime   = 0;
    if(m_attrType == '4' ) //系统级累计账单
    {
      vector<T_SERVER_ID> t_userIdList;
      vector<int>         t_cycleIdList;
      getUserIdList(m_globalInfo,m_policyInfo->m_rateTpInfo,m_roleCode,t_userIdList);
      getCycleIdList(m_globalInfo,m_startNum,m_endNum,t_cycleIdList);
      //根据角色获取ID列表
      //根据开始周期偏移和终止周期偏移获取帐期列表
      if (!m_addupItem->getBillValue(m_globalInfo,
                                    m_policyInfo->m_rateTpInfo, 
                                    t_userIdList, 
                                    t_cycleIdList, value) )
      {
        THROW_C_P2(BusinessException, -1, "累计对象错误,getBillValue return false"
         "object_id=%d,attr_Id=%d", m_id,m_attrId); 
      }
    }
    else if(m_attrType == '5' ) //累计1的数目
    {
      value = m_globalInfo->m_userSumBill1s_new.size();
    }
    else
    {
      //Modified By Sunam 2009/12/29 10:33:38
      //if(m_addupItem->getCyleTime(m_globalInfo,m_policyInfo->m_rateTpInfo,sumBeginTime,sumEndTime)==false)
      if(m_addupItem->getCyleTime(m_globalInfo,m_policyInfo->m_rateTpInfo,m_globalInfo->m_beginDateTime,sumBeginTime,sumEndTime)==false)
      {
        THROW_C_P2(BusinessException, -1, "累计对象错误,getCyleTime return false"
         "object_id=%d,attr_Id=%d", m_id,m_attrId); 
      }
  		if (!m_addupItem->getAddupValue(m_globalInfo,
                                      m_policyInfo->m_rateTpInfo, 
                                      sumBeginTime, sumEndTime, value,
                                      m_startNum, m_endNum ) )
      {
        THROW_C_P2(BusinessException, -1, "累计对象错误,getAddupValue return false"
         "object_id=%d,attr_Id=%d", m_id,m_attrId); 
      }
    }
	}
	else
	{
		THROW_C_P1(BusinessException, -1, "累计对象错误,累计帐目指针为空："
                    "object_id=%d", m_id);	
	}
	return true;
}

//根据系统帐期取前后几个月的帐期信息
void ObjectAddup::getCycleIdList(RateGlobalInfo *r_pGlobalInfo,
                                 const int      &r_startOffNum,
                                 const int      &r_endOffNum,
                                 vector<int>    &r_cycleIdList) const
{
  r_cycleIdList.clear();
  if(r_startOffNum>r_endOffNum) return;
  long  t_cdrCycle0 = r_pGlobalInfo->m_cycParam.m_cycleId;
  long  t_cdrCycle1 = t_cdrCycle0;
  for(int i=r_startOffNum;i<=r_endOffNum;++i)
  {
    t_cdrCycle1 = t_cdrCycle0;
    r_pGlobalInfo->addMonth(i,t_cdrCycle1);
    r_cycleIdList.push_back(t_cdrCycle1);
  }
}
void ObjectAddup::getUserIdList(RateGlobalInfo      *r_pGlobalInfo,
                                const RateTpInfo    *r_rateTpInfo,
                                const T_MEMBER_TYPE &r_roleCode,
                                vector<T_SERVER_ID> &r_userIdList) const
{
  r_userIdList.clear();
  T_SERVER_ID  &t_tpUserId=r_rateTpInfo->m_pUserTp->m_userId;
  vector<T_SERVER_ID>::const_iterator t_uItr;
  map<T_SERVER_ID,T_MEMBER_TYPE>      t_userMapList;
  if(r_rateTpInfo->m_pUserTp->m_subTpType == TP_TYPE_VPN)
  { //挂在群上的资费
    if(r_rateTpInfo->m_rateRoleCode == ROLECODE_SELF)
    { //群总量
      if(r_roleCode == ROLECODE_SELF)
      { 
        r_userIdList.push_back(t_tpUserId);
      }
    }else
    { //个人方式
      if(r_roleCode == ROLECODE_SELF)
      { //自身
        r_userIdList.push_back(r_pGlobalInfo->m_billUserId);
      }else if(r_roleCode == ROLECODE_WIDECARD)
      { //参考群总量
        r_userIdList.push_back(t_tpUserId);
      }else
      { //取群内所有角色为r_roleCode的所有用户r_userIdList
        //r_pGlobalInfo->m_paramMgr->getUserInfoByVpnId(t_tpUserId,)
        //t_userMapList
        for(t_uItr =r_userIdList.begin();
            t_uItr!=r_userIdList.end();++t_uItr)
        {
          if(*t_uItr == r_pGlobalInfo->m_billUserId) continue;
          r_pGlobalInfo->getRoleUserBill2(*t_uItr);  
        }
      }
    }
  }else //个人套餐
  {
    if(r_roleCode == ROLECODE_SELF)
    { 
      r_userIdList.push_back(m_globalInfo->m_billUserId);
    }else if(!(r_rateTpInfo->m_pUserTp->m_relationVpnId==NULL_VPN_ID))
    { //关联群
      //取群内角色为r_roleCode的所有用户列表
      //r_rateTpInfo->m_pUserTp->m_relationVpnId
      //r_roleCode r_userIdList
      for(t_uItr=r_userIdList.begin();
          t_uItr!=r_userIdList.end();++t_uItr)
      {
        if(*t_uItr == r_pGlobalInfo->m_billUserId) continue;
        r_pGlobalInfo->getRoleUserBill2(*t_uItr);  
      }
    }
  }
}

