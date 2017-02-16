#include "AddupItem.h"
#include "AddupCycleRule.h"
#include "RateGlobalInfo.h"
#include "RateError.h"
#include "DetailBillMgr.h"
#include "NodeBase.h"

//对于累计只能参考套餐实例对应的三户ID本身
//获取资费实例累计累计值
bool AddupItem::getAddupValue(RateGlobalInfo   *(&pGlobalInfo),
                              RateTpInfo       *(&r_rateTpInfo),
                              const      long  &sumBeginTime,
                              const      long  &sumEndTime,               
                                         long  &addupValue,
                                         short startNum,
                                         short endNum) const
{
  addupValue = 0;
  
  if (m_cycleRuleId==CYCLERULEID_COMMONMONTH &&
     !(r_rateTpInfo->m_pUserTp->m_subTpType==TP_TYPE_VPN  &&
       r_rateTpInfo->m_rateRoleCode == ROLECODE_SELF))     //从累计量1里取
  {
    UserSumBill1 t_userSumBill1;
    const UserSumBill1 *t_pUserSumBill1;
    //t_userSumBill1.m_userId  = r_rateTpInfo->m_pUserTp->m_userId;
    //应该根据个人UserId来取.比如挂在集团上，所有个人独立继承 
    //2009-9-22 17:54:56 modified by gaojf
    t_userSumBill1.m_userId  = pGlobalInfo->m_userId;
    t_userSumBill1.m_tpObjId = r_rateTpInfo->m_pUserTp->m_tpObjId;
    t_userSumBill1.m_billId  = m_id;
    
    if (pGlobalInfo->getBillItem<UserSumBill1>(
        pGlobalInfo->m_userSumBill1s_new,t_userSumBill1, t_pUserSumBill1) )
    {
      addupValue = t_pUserSumBill1->m_value;
    }
  }else    //从累计量2里取
  {
    UserSumBill2 userSumBill2;
    const UserSumBill2 *pUserSumBill2;
    char cycleType;
    
    if (m_cycleRule!=NULL)
    {
      cycleType = m_cycleRule->m_cycleType;
    }else
    { //累计规则周期未定义
      char     t_errMsg[64];
      sprintf(t_errMsg,"累计规则周期ID=%d 未定义!",m_cycleRuleId);
      #ifdef _DEBUG_
        cout<<t_errMsg<<__FILE__<<__LINE__<<endl;
      #endif
      pGlobalInfo->setErrInfo(E_ADDITEM_CYCRULE,t_errMsg);
      return false;
    }
    userSumBill2.m_beginTime = sumBeginTime;
    userSumBill2.m_endTime   = sumEndTime;
    
    //Modified by yangyi 2009/9/8 10:22:17 都要根据账期来
    /*
    if (m_elemType==ADDUP_ELEM_TYPE_EVENT) //计费累计         
          userSumBill2.m_cycleId = 0;
        else //帐务累计累计
          userSumBill2.m_cycleId = pGlobalInfo->m_cycParam.m_cycleId;
    */
    //userSumBill2.m_cycleId = pGlobalInfo->m_cycParam.m_cycleId;
    //Modified By Sunam 2009/10/9 22:30:51 计费不能根据账期
    if (m_elemType==ADDUP_ELEM_TYPE_EVENT) //计费累计         
        userSumBill2.m_cycleId = 0;
    else //帐务累计累计
        userSumBill2.m_cycleId = pGlobalInfo->m_cycParam.m_cycleId;
    
    userSumBill2.m_tpObjId = r_rateTpInfo->m_pUserTp->m_tpObjId;
    userSumBill2.m_billId  = m_id;  

    if(r_rateTpInfo->m_pUserTp->m_subTpType==TP_TYPE_VPN  &&
       r_rateTpInfo->m_rateRoleCode == ROLECODE_SELF)
    {
      userSumBill2.m_userId  = r_rateTpInfo->m_pUserTp->m_userId;
    }else
    {
      userSumBill2.m_userId = pGlobalInfo->m_billUserId;  
    }
    
    if (m_elemType!=ADDUP_ELEM_TYPE_EVENT &&   //帐务累计
        m_cycleRule->m_cycleType>=ADDUP_CYCLE_TYPE_MONTH)     //跨月累计
    {
//      if (pGlobalInfo->getLastSumBill2(pGlobalInfo->m_userSumBill2s_new,
//                                       userSumBill2, pUserSumBill2) )
//      {
//        addupValue = pUserSumBill2->m_value/pGlobalInfo->m_comBill.m_divisor;
//      }

		  //帐务多个月费用的累计，一般用于参考n个月总消费额的情况  
      if(m_cycleRule->m_cycleNum>1 || 
         m_cycleRule->m_cycleType>ADDUP_CYCLE_TYPE_MONTH)
      {
        if (pGlobalInfo->getLastSumBill2(pGlobalInfo->m_userSumBill2s_new,
                                         userSumBill2, pUserSumBill2) )
        {
          addupValue = pUserSumBill2->m_value/pGlobalInfo->m_comBill.m_divisor;;
        }
      }
      else //帐务1个月费用的累计，一般用于当月参考前n个月消费额的情况
      {
      	//帐期置为前n个月的帐期
      	long sumbillcycleId = userSumBill2.m_cycleId;
      	pGlobalInfo->addMonth( (long)startNum,sumbillcycleId);
      	int acycNum = endNum - startNum + 1;
      	for(int i=0; i<acycNum;++i)
      	{      		
      		userSumBill2.m_cycleId = sumbillcycleId;      		
      		userSumBill2.m_beginTime = userSumBill2.m_cycleId * 100000000 + 
      		                           userSumBill2.m_beginTime % 100000000;
      		if (pGlobalInfo->getBillItem<UserSumBill2>(
        	  pGlobalInfo->m_userSumBill2s_new,userSumBill2,pUserSumBill2) )    
        	{
        	  addupValue += pUserSumBill2->m_value/pGlobalInfo->m_comBill.m_divisor;;
        	}
        	//资费帐期往前推一个月        	
        	pGlobalInfo->addMonth( 1,sumbillcycleId);
        }
      }
    }else //计费累计 或者 帐务按天累计
    {
      if (pGlobalInfo->getBillItem<UserSumBill2>(
          pGlobalInfo->m_userSumBill2s_new,userSumBill2,pUserSumBill2) )    
      {
      	if(ADDUP_ELEM_TYPE_EVENT!=m_elemType)
           addupValue = pUserSumBill2->m_value/pGlobalInfo->m_comBill.m_divisor;
        else
        	addupValue = pUserSumBill2->m_value;
      }
    }
  }
  return true;
}

/*@function 获取系统级累计账单信息
 *@param    r_pGlobalInfo  接口信息
 *@param    r_rateTpInfo   资费信息
 *@param    r_userIdList   ID列表
 *@param    r_cycleIdList  帐期列表
 *@param    r_cycleId      周期起始时间
 *@param    r_addupValue   值
 *@return  true OK ,false 失败
 */
bool AddupItem::getBillValue(RateGlobalInfo      *r_pGlobalInfo,
                             const RateTpInfo    *r_rateTpInfo,
                       const vector<T_SERVER_ID> &r_userIdList,
                       const vector<int>         &r_cycleIdList,
        					                 long          &r_addupValue) const
{
  r_addupValue = 0;
  UserSumBill2 t_userSumBill2;
  const UserSumBill2 *t_pUserSumBill2;
  long  int  t_beginTime;
  t_userSumBill2.m_tpObjId = 0; //系统级累计账单恒为0
  t_userSumBill2.m_billId  = m_id;
  for(vector<T_SERVER_ID>::const_iterator t_idItr=r_userIdList.begin();
      t_idItr!=r_userIdList.end();++t_idItr)
  {
    t_userSumBill2.m_userId = *t_idItr;
    for(vector<int>::const_iterator t_cycItr=r_cycleIdList.begin();
        t_cycItr!=r_cycleIdList.end();++t_cycItr)
    {
      t_userSumBill2.m_cycleId = *t_cycItr;
      //t_beginTime = (*t_cycItr)*100000000+1000000; //1号0点0分0秒
      t_userSumBill2.m_beginTime = (*t_cycItr)*100+1; //1号
      t_userSumBill2.m_beginTime = t_userSumBill2.m_beginTime*1000000;
      if(r_pGlobalInfo->getBillItem<UserSumBill2>(r_pGlobalInfo->m_userSumBill2s_new,
                                                  t_userSumBill2, t_pUserSumBill2))
      {
        r_addupValue+=t_pUserSumBill2->m_value/r_pGlobalInfo->m_comBill.m_divisor;
      }
    }
  }
  return true;
}

//获取帐户当月费用
bool AddupItem::getAcctCurValue(RateGlobalInfo * (&pGlobalInfo), 
                                long &curValue)
{
	curValue = 0;
	
	if (m_elemType==ADDUP_ELEM_TYPE_ITEM||
		m_elemType==ADDUP_ELEM_TYPE_CURFEE)
	{
		if (m_elemValueSrc==ADDUP_ELEM_VALUESRC_FACT)
			pGlobalInfo->m_detailBillMgr.getFactFee(m_elemId, curValue);
		else if (m_elemValueSrc==ADDUP_ELEM_VALUESRC_DISCNT)
			pGlobalInfo->m_detailBillMgr.getDiscntFee(m_elemId, curValue);
		else if (m_elemValueSrc==ADDUP_ELEM_VALUESRC_ORGIN)
			pGlobalInfo->m_detailBillMgr.getOriginFee(m_elemId, curValue);
		else
		{
			//累计的元素值来源未定义
  			char     t_errMsg[64];
  			sprintf(t_errMsg,"累计%d的元素值来源=%c 未定义!",m_cycleRuleId,m_elemValueSrc);
  			#ifdef _DEBUG_
  			  cout<<t_errMsg<<__FILE__<<__LINE__<<endl;
  			#endif
  			pGlobalInfo->setErrInfo(E_ADDITEM_CYCRULE,t_errMsg);
  			return false;
		}
	}
	else if(m_elemType==ADDUP_ELEM_TYPE_ACYCNUM)
	{
		curValue = 1;
	}
	else //if(m_elemType==ADDUP_ELEM_TYPE_EVENT)
	{
		//暂不支持,计费的累计目前还不从这条路走
		curValue = 0;
	}
	
	return true;
}

//更新累计值
bool AddupItem::updateAddupValue(RateGlobalInfo  *(&pGlobalInfo),
                                 RateTpInfo      *(&rateTpInfo),
                                 const      long  &sumBeginTime,
                                 const      long  &sumEndTime,
                                 const      long  &addupValue,
                                 const      char  addupMode)
{

	if (m_cycleRuleId==CYCLERULEID_COMMONMONTH)     //更新累计量1
	{
		//暂不支持
		
		//UserSumBill1 userSumBill1;
		//const UserSumBill1 *pUserSumBill1;
		//userSumBill1.m_userId = rateTpInfo.m_pUserTp->m_userId;
		//userSumBill1.m_tpObjId = rateTpInfo.m_pUserTp->m_tpObjId;
		//userSumBill1.m_billId = m_id;
		//userSumBill1.m_value = addupValue;
		//
		//pGlobalInfo->updateBillIncre<UserSumBill1>(
		//          pGlobalInfo->m_userSumBill1s_new, userSumBill1);				
	}
	else                             //更新累计量2
	{
		UserSumBill2 userSumBill2;
		userSumBill2.m_beginTime = sumBeginTime;
		userSumBill2.m_endTime = sumEndTime;
		userSumBill2.m_cycleId = pGlobalInfo->m_cycParam.m_cycleId;
		
		if(rateTpInfo->m_pUserTp->m_subTpType==TP_TYPE_VPN  &&
           rateTpInfo->m_rateRoleCode == ROLECODE_SELF)
    	{
    		userSumBill2.m_userId  = rateTpInfo->m_pUserTp->m_userId;
    		userSumBill2.m_type = 1;
    		userSumBill2.m_groupUserId = rateTpInfo->m_pUserTp->m_userId; 
    	}
    	else
    	{
    		userSumBill2.m_userId = pGlobalInfo->m_billUserId; 
    		userSumBill2.m_type = 0;     		
    		userSumBill2.m_groupUserId = 0; 
    	}
		
		if (addupMode=='1')//跟资费相关的累计		
			userSumBill2.m_tpObjId = rateTpInfo->m_pUserTp->m_tpObjId;
		else               //跟资费无关的累计
			userSumBill2.m_tpObjId = 0;
		userSumBill2.m_billId = m_id;		
		
//		if (m_elemType==ADDUP_ELEM_TYPE_EVENT)//计费累计 
//		{		
//			//暂不支持
//					
//			//userSumBill2.m_value = 0;
//			//userSumBill2.m_cycleId = 0;
//			//pGlobalInfo->updateBillIncre<UserSumBill2>(
//		    //pGlobalInfo->m_userSumBill2s_new, userSumBill2);
//		}
//		//帐务累计
//		else //if(m_elemType==ADDUP_ELEM_TYPE_ITEM || m_elemType==ADDUP_ELEM_TYPE_ACYCNUM)
//		{
//			//取上月的值			
//			const UserSumBill2 *pUserSumBill2;
//			if (pGlobalInfo->getLastSumBill2(
//				pGlobalInfo->m_userSumBill2s_new, userSumBill2, pUserSumBill2) )
//			{
//				userSumBill2.m_value = pUserSumBill2->m_value/pGlobalInfo->m_comBill.m_divisor;
//			}
//			else
//			{
//				userSumBill2.m_value = 0;				
//			}
//				
//			userSumBill2.m_value += addupValue*(pGlobalInfo->m_comBill.m_divisor);                         
//			userSumBill2.m_cycleId = pGlobalInfo->m_cycParam.m_cycleId;           
//                                                            
//			pGlobalInfo->updateBillTotal<UserSumBill2>(                 
//            pGlobalInfo->m_userSumBill2s_new, userSumBill2);			
//		}
			//帐务累计(累多个月的情况)
		if(m_elemType==ADDUP_ELEM_TYPE_ITEM || m_elemType==ADDUP_ELEM_TYPE_ACYCNUM)
		{
			//取上月的值			
			const UserSumBill2 *pUserSumBill2;
			if (pGlobalInfo->getLastSumBill2(
				pGlobalInfo->m_userSumBill2s_new, userSumBill2, pUserSumBill2) )
			{
				userSumBill2.m_value = pUserSumBill2->m_value;
			}
			else
			{
				userSumBill2.m_value = 0;				
			}
				
			userSumBill2.m_value += addupValue*(pGlobalInfo->m_comBill.m_divisor);                        
      		userSumBill2.m_cycleId = pGlobalInfo->m_cycParam.m_cycleId;
                                                            
			pGlobalInfo->updateBillTotal<UserSumBill2>(                 
            pGlobalInfo->m_userSumBill2s_new, userSumBill2);			
		}
		//帐务在累积量2中累积本月的费用,供下月优惠参考
		else if(m_elemType==ADDUP_ELEM_TYPE_CURFEE)
		{				
			userSumBill2.m_value = addupValue*(pGlobalInfo->m_comBill.m_divisor);                      
                                                            
			pGlobalInfo->updateBillTotal<UserSumBill2>(                 
            pGlobalInfo->m_userSumBill2s_new, userSumBill2);			
		}	
		else //if (m_elemType==ADDUP_ELEM_TYPE_EVENT)//计费累计 
		{		              
		    //暂不支持
		}
	}	
	return true;
}

void AddupItem::dump(char *prefix)
{
  char myPrefix[256] = "  ";
  strcat(myPrefix,prefix);
  cout <<myPrefix<<"{" <<endl;
  cout <<myPrefix<<"----AddupItem : " <<m_id<<" ----" <<endl;
  cout <<myPrefix<<"m_id           = "<< m_id <<endl;
  cout <<myPrefix<<"m_elemId       = "<< m_elemId<<endl;
  cout <<myPrefix<<"m_elemType     = "<< m_elemType<<endl;
  cout <<myPrefix<<"m_elemValueSrc = "<< m_elemValueSrc<<endl;
  cout <<myPrefix<<"m_cycleRuleId  = "<< m_cycleRuleId<<endl;
  if (m_cycleRule==0)
    cout <<myPrefix<<"m_cycleRule    = NULL"<<endl;
  else
  {
    cout <<myPrefix<<"m_cycleRule    != NULL"<<endl;
    m_cycleRule->dump(myPrefix);
  }
  cout <<myPrefix<<"}" <<endl;
}

//只为计费累计量使用
//根据资费信息和费用作用的累计信息取对应的累计ID信息
//Modified By Sunam 2009/12/29 10:24:05 如果是累计量2，则根据指定时间 r_cdrStartTime 求累计量周期
//bool AddupItem::getRateBillIdInfo(RateGlobalInfo   *r_pGlobalInfo,
//                                  const RateTpInfo *r_rateTpInfo,
//                                        RateBillId *r_rateBillId) const
bool AddupItem::getRateBillIdInfo(RateGlobalInfo   *r_pGlobalInfo,
                                  const RateTpInfo *r_rateTpInfo,
                                  const T_DATE_TIME &r_cdrStartTime,
                                        RateBillId *r_rateBillId) const
{
  const T_TPOBJ_ID &t_tpObjId = r_rateTpInfo->m_pUserTp->m_tpObjId;
  const T_TP_ID    &t_rateTpId= r_rateTpInfo->m_rateTpId;
  RateBillId & t_rateBillId = *r_rateBillId;
  
  t_rateBillId.m_tpObjId = t_tpObjId;
  t_rateBillId.m_billId  = m_id;
  //Added By Sunam 2009/8/8 9:58:54
  t_rateBillId.m_feePolicyId= r_rateTpInfo->m_pUserTp->m_tpId;

  //只有集团套餐，并且作用在角色是集团本身的方式才是
  // 累计到集团头上
  if(r_rateTpInfo->m_pUserTp->m_subTpType==TP_TYPE_VPN  &&
     r_rateTpInfo->m_rateRoleCode == ROLECODE_SELF)
  {
    t_rateBillId.m_type     = 1;
    t_rateBillId.m_billType = BILL_TYPE_SUM2;
    t_rateBillId.m_userId   = r_rateTpInfo->m_pUserTp->m_userId;
    t_rateBillId.m_groupId  = r_rateTpInfo->m_pUserTp->m_userId;
  }else 
  {
    t_rateBillId.m_type     = 0;
    t_rateBillId.m_groupId  = 0;
    t_rateBillId.m_userId   = r_pGlobalInfo->m_billUserId;
    if(m_cycleRuleId == CYCLERULEID_COMMONMONTH)
    { //只有普通月周期标识才作为累计量1
      t_rateBillId.m_billType = BILL_TYPE_SUM1;
      long  t_cdrCycle = r_pGlobalInfo->m_cycParam.m_cycleId;
      char  t_strDateTime[14+1];
      sprintf(t_strDateTime,"%06ld01000000",t_cdrCycle);
      t_rateBillId.m_sumBeginTime = atol(t_strDateTime);
      r_pGlobalInfo->addMonth(1,t_cdrCycle);
      sprintf(t_strDateTime,"%06ld01000000",t_cdrCycle);
      t_rateBillId.m_sumEndTime = atol(t_strDateTime);
      return true;
    }else
    {
      t_rateBillId.m_billType = BILL_TYPE_SUM2;
    }
  }
  //对于累计量2
  //根据话单开始时间，计算累计的起始时间和结束时间
  if(m_cycleRule==NULL)
  { //累计规则周期未定义
    char     t_errMsg[64];
    sprintf(t_errMsg,"累计规则周期ID=%d 未定义!",m_cycleRuleId);
    #ifdef _DEBUG_
      cout<<t_errMsg<<__FILE__<<__LINE__<<endl;
    #endif
    r_pGlobalInfo->setErrInfo(E_ADDITEM_CYCRULE,t_errMsg);
    return false;
  }
  //Modified By Sunam 2009/12/29 10:24:45 根据指定时间r_cdrStartTime 求累计量周期
  //if(getCyleTime(r_pGlobalInfo,r_rateTpInfo,t_rateBillId.m_sumBeginTime,t_rateBillId.m_sumEndTime)==false)
  if(getCyleTime(r_pGlobalInfo,r_rateTpInfo,r_cdrStartTime,t_rateBillId.m_sumBeginTime,t_rateBillId.m_sumEndTime)==false)
  {
    return false;
  }
  return true;
}

//Modified By Sunam 2009/12/29 10:26:02 根据指定时间r_cdrStartTime 求累计量周期
//bool AddupItem::getCyleTime(RateGlobalInfo   *r_pGlobalInfo,
//                            const RateTpInfo *r_rateTpInfo,
//                               long          &r_sumBeginTime,
//                               long          &r_sumEndTime) const
bool AddupItem::getCyleTime(RateGlobalInfo   *r_pGlobalInfo,
                            const RateTpInfo *r_rateTpInfo,
                            const T_DATE_TIME &r_cdrStartTime,
                               long          &r_sumBeginTime,
                               long          &r_sumEndTime) const
{
  //Modified By Sunam 2009/12/29 10:26:02 根据指定时间r_cdrStartTime 求累计量周期
  //return m_cycleRule->getSumTimeInfo(r_pGlobalInfo,r_rateTpInfo,r_sumBeginTime,r_sumEndTime);
  return m_cycleRule->getSumTimeInfo(r_pGlobalInfo,r_rateTpInfo,r_cdrStartTime,r_sumBeginTime,r_sumEndTime);
}


