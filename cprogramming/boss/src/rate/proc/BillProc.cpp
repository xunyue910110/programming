#include "BillProc.h"
#include "RateError.h"
#include "BillManager.h"
#include "RateConstDef.h"
#include "RateGlobalInfo.h"
#include <algorithm>

INSTALL_PROCEDURE_INSTANCE(GetBillProc);

INSTALL_PROCEDURE_INSTANCE(UpdateBillProc);

INSTALL_PROCEDURE_INSTANCE(RollBackBillProc);

//Modified By Sunam 2009/8/10 13:49:46
/*
int GetBillProc::execute()
{
   #ifdef FP_DEBUG
    FP_BEGIN(GetBillProc)
  #endif
  T_SERVER_ID &t_userId=m_pRateInfo->m_billUserId;
  
  //根据合帐UserId来合帐
  if(t_userId == INVALID_SERVER_ID)
  {
    return 0;
  }
	//1.取计费帐单
	if(m_billMgr->getUserBill(m_pRateInfo->m_cycParam.m_cycleId,t_userId,m_pRateInfo->m_userBills)==false)
	{
	  (*m_pcdr).m_errorCode=E_GET_BILL;
	  return 1;
  }	
  m_pRateInfo->setFirstCallDate();
	//2.取月为单位累计量
	if(m_billMgr->getUserBill(m_pRateInfo->m_cycParam.m_cycleId,t_userId,m_pRateInfo->m_userSumBill1s)==false)
	{
	  (*m_pcdr).m_errorCode=E_GET_BILL;
	  return 1;
  }	
	//3.取非月为单位累计量(非话单对应帐期的，在用到的时候求取)
	if(m_billMgr->getUserBill(m_pRateInfo->m_cycParam.m_cycleId,t_userId,m_pRateInfo->m_userSumBill2s)==false)
	{
	  (*m_pcdr).m_errorCode=E_GET_BILL;
	  return 1;
  }
  //4. 取计费优惠信息
  if(m_billMgr->getUserBill(m_pRateInfo->m_cycParam.m_cycleId,t_userId,m_pRateInfo->m_rateDiscnts)==false)
	{
	  (*m_pcdr).m_errorCode=E_GET_BILL;
	  return 1;
  }
   
  ::sort(m_pRateInfo->m_userBills.begin(),m_pRateInfo->m_userBills.end());
  ::sort(m_pRateInfo->m_userSumBill1s.begin(),m_pRateInfo->m_userSumBill1s.end());
  ::sort(m_pRateInfo->m_userSumBill2s.begin(),m_pRateInfo->m_userSumBill2s.end());
  ::sort(m_pRateInfo->m_rateDiscnts.begin(),m_pRateInfo->m_rateDiscnts.end());
  
  //将原始账单赋值给最新的账单
  m_pRateInfo->m_userBills_new     = m_pRateInfo->m_userBills;
  m_pRateInfo->m_userSumBill1s_new = m_pRateInfo->m_userSumBill1s;
  m_pRateInfo->m_userSumBill2s_new = m_pRateInfo->m_userSumBill2s;
  
  #ifdef _DEBUG_
    //m_pRateInfo->dumpBillInfo();
  #endif
   #ifdef FP_DEBUG
    FP_END(GetBillProc)
  #endif
  return 0;
}; 
*/

int GetBillProc::execute()
{
	#ifdef FP_DEBUG
		FP_BEGIN(GetBillProc)
	#endif
	T_SERVER_ID &t_userId=m_pRateInfo->m_billUserId;
  
	//根据合帐UserId来合帐
	if(t_userId == INVALID_SERVER_ID)
	{
		return 0;
	}
	
	//Added By Sunam 2009/8/10 13:53:13
  	//取UserBill
  	if (!m_pRateInfo->getUserBill(m_pRateInfo->m_cycParam.m_cycleId,t_userId,m_pRateInfo->m_userBills))
  	{
  		(*m_pcdr).m_errorCode=E_GET_BILL;
		return 1;
  	}
  	//UserSum2
  	if (!m_pRateInfo->getUserBill(m_pRateInfo->m_cycParam.m_cycleId,t_userId,m_pRateInfo->m_userSumBill2s))
  	{
  		(*m_pcdr).m_errorCode=E_GET_BILL;
		return 1;
  	}
	//UserSum1,考虑历史话单回收，如果该话单的rateTimes=99，则认为该话单需要使用历史账单的累计量，否则使用当前账期累计量
	/*
	if (m_pcdr[ATTRI_RATE_TIMES].value() == 99)
	{
		long sysBillCycle = m_pcdr[ATTRI_CYCLE_DATE].value();
		//假设最多只充许使用当前账期前4个历史账期的免费资源，需要完善成配置方式实现
		m_pRateInfo->minusMonth(4,sysBillCycle);
		//如时话单账期早于当前账前前4个月的，则使前4个月的
		if (sysBillCycle > m_pcdr[ATTRI_CDR_CYCLE_ORG].value())
		{
			//保存累计量1的账期Index，方便在updateBillProc中使用
  			m_pRateInfo->m_sum1BillCycle = sysBillCycle;
		}
		//否则使用话单账期
		else
		{
  			m_pRateInfo->m_sum1BillCycle = m_pcdr[ATTRI_CDR_CYCLE_ORG].value();
		}
  	}
  	//非历史话单回收，则使用系统账期累计量
  	else
  	{
  		m_pRateInfo->m_sum1BillCycle = m_pcdr[ATTRI_CYCLE_DATE].value();
  	}
  	*/
  	
  	long sysBillCycle = m_pcdr[ATTRI_CYCLE_DATE].value();
		//假设最多只充许使用当前账期前4个历史账期的免费资源，需要完善成配置方式实现
		/*
		m_pRateInfo->minusMonth(4,sysBillCycle);
		//如时话单账期早于当前账前前4个月的，则使前4个月的
		if (sysBillCycle > m_pcdr[ATTRI_CDR_CYCLE_ORG].value())
		{
			//保存累计量1的账期Index，方便在updateBillProc中使用
  			m_pRateInfo->m_sum1BillCycle = sysBillCycle;
		}
		//否则使用话单账期
		else
		{
  			m_pRateInfo->m_sum1BillCycle = m_pcdr[ATTRI_CDR_CYCLE_ORG].value();
		}*/
		
		m_pRateInfo->m_sum1BillCycle = m_pcdr[ATTRI_CDR_CYCLE_ORG].value();
		
  	//UserSum1
  	if (!m_pRateInfo->getUserBill(m_pRateInfo->m_sum1BillCycle,t_userId,m_pRateInfo->m_userSumBill1s))
  	{
  		(*m_pcdr).m_errorCode=E_GET_BILL;
		return 1;
  	}
  	//RateDiscnt
  	if (!m_pRateInfo->getUserBill(m_pRateInfo->m_cycParam.m_cycleId,t_userId,m_pRateInfo->m_rateDiscnts))
  	{
  		(*m_pcdr).m_errorCode=E_GET_BILL;
		return 1;
  	}
	 //4.UserSum3
   	if (!m_pRateInfo->getUserBill(m_pRateInfo->m_cycParam.m_cycleId,t_userId,m_pRateInfo->m_userSumBill3s))
  	{
  	  (*m_pcdr).m_errorCode=E_GET_BILL;
  	  return 1;
   }
  //5.UserMonfeeBill
  	if (!m_pRateInfo->getUserBill(m_pRateInfo->m_cycParam.m_cycleId,t_userId,m_pRateInfo->m_userMonfeeBills))
	{
	  (*m_pcdr).m_errorCode=E_GET_BILL;
	  return 1;
   }
  
	
	m_pRateInfo->setFirstCallDate();
  
	//将原始账单赋值给最新的账单
	m_pRateInfo->m_userBills_new     = m_pRateInfo->m_userBills;
	m_pRateInfo->m_userSumBill1s_new = m_pRateInfo->m_userSumBill1s;
	m_pRateInfo->m_userSumBill2s_new = m_pRateInfo->m_userSumBill2s;
	m_pRateInfo->m_userSumBill3s_new   = m_pRateInfo->m_userSumBill3s;
  m_pRateInfo->m_userMonfeeBills_new = m_pRateInfo->m_userMonfeeBills;
	//优惠账单在new不保存
	//m_pRateInfo->m_rateDiscnts_new = m_pRateInfo->m_rateDiscnts;
  
	#ifdef FP_DEBUG
		FP_END(GetBillProc)
	#endif
	return 0;
}; 

void UpdateBillProc::addRollBackBill()
{
}

void UpdateBillProc::printInterBill()
{
  //vector<UserBill>::const_iterator t_itr1;
  //vector<UserSumBill1>::const_iterator t_itr2;
  //vector<UserSumBill2>::const_iterator t_itr3;
  //vector<AccountBill>::const_iterator t_itr4;
  //int i;
  //for(i=0;i<2;++i)
  //{
  //  cout<<"m_userBills["<<i<<"]----------"<<endl;
  //  for(t_itr1=m_userBills[i].begin();
  //      t_itr1!=m_userBills[i].end();++t_itr1)
  //  {
  //    cout<<*t_itr1<<endl;
  //  }    
  //}
  //for(i=0;i<2;++i)
  //{
  //  cout<<"m_userSumBill1s["<<i<<"]----------"<<endl;
  //  for(t_itr2=m_userSumBill1s[i].begin();
  //      t_itr2!=m_userSumBill1s[i].end();++t_itr2)
  //  {
  //    cout<<*t_itr2<<endl;
  //  }    
  //}
  //for(i=0;i<2;++i)
  //{
  //  cout<<"m_userSumBill1s["<<i<<"]----------"<<endl;
  //  for(t_itr3=m_userSumBill2s[i].begin();
  //      t_itr3!=m_userSumBill2s[i].end();++t_itr3)
  //  {
  //    cout<<*t_itr3<<endl;
  //  }    
  //}
  //
  //for(i=0;i<2;++i)
  //{
  //  cout<<"m_userSumBill1s["<<i<<"]----------"<<endl;
  //  for(t_itr4=m_accountBills[i].begin();
  //      t_itr4!=m_accountBills[i].end();++t_itr4)
  //  {
  //    cout<<*t_itr4<<endl;
  //  }    
  //}
}

//Modified By Sunam 2009/8/10 16:22:01
/*
int UpdateBillProc::execute() 
{
   #ifdef FP_DEBUG
    FP_BEGIN(UpdateBillProc)
  #endif
  T_SERVER_ID &t_userId=m_pRateInfo->m_billUserId;
  if(t_userId == INVALID_SERVER_ID ||
     (*m_pcdr).m_errorCode != E_DEFAULT)
  {
    return 0;
  }
  m_pRateInfo->generateRollBill();
  m_userBills[0].clear();    m_userBills[1].clear();
  m_userSumBill1s[0].clear();m_userSumBill1s[1].clear();
  m_userSumBill2s[0].clear();m_userSumBill2s[1].clear();
  m_accountBills[0].clear(); m_accountBills[1].clear();
  m_rateDiscnts[0].clear();  m_rateDiscnts[1].clear();
  //准备各项更新的帐单项和新增的帐单项
  for(vector<RollBackBill>::iterator t_billItr = m_pRateInfo->m_rollBackBills.begin();
      t_billItr != m_pRateInfo->m_rollBackBills.end();++t_billItr)
  {
    switch(t_billItr->m_billType)
    {
      case BILL_TYPE_USER : //UserBill
        setBill(m_pRateInfo->m_userBills_new,m_userBills,*t_billItr);
        break;
      case BILL_TYPE_SUM1 : //UserSumBill1
        setBill(m_pRateInfo->m_userSumBill1s_new,m_userSumBill1s,*t_billItr);
        break;
      case BILL_TYPE_SUM2 : //UserSumBill2
        setBill(m_pRateInfo->m_userSumBill2s_new,m_userSumBill2s,*t_billItr);
        break;
      case BILL_TYPE_ACCT : //AccountBill
        setBill(m_pRateInfo->m_accountBills_new,m_accountBills,*t_billItr);
        break;
      case BILL_TYPE_RATEDISCNT:
        {  //计费优惠特殊处理
          RateDiscntBill  t_ratebill;
          const RateDiscntBill *t_pRateBill=NULL;
          t_ratebill = *t_billItr;
          if(t_billItr->m_operFlag == BILL_OPERATE_INSERT)
          {
            m_rateDiscnts[1].push_back(t_ratebill);
          }else
          {
            if(m_pRateInfo->getBillItem<RateDiscntBill>(
                  m_pRateInfo->m_rateDiscnts,t_ratebill,t_pRateBill)==false)
            {
              m_rateDiscnts[1].push_back(t_ratebill);
            }else
            {
              t_ratebill+=*t_pRateBill;
              m_rateDiscnts[0].push_back(t_ratebill);
            }
          }
        }
        break;
      default:
        break;
    };
  }

  bool t_bRet = true;
  //1.更新计费帐单
  if(!(m_userBills[0].empty()))
  {
    t_bRet = m_billMgr->updateUserBill(m_pRateInfo->m_cycParam.m_cycleId,m_userBills[0]);
    if(t_bRet == false)
    {
      m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
      sprintf(m_errMsg,"%d userid=%s update UserBill failed! %s.",
              E_UPDATE_BILL,t_userId.c_str(),
              m_billMgr->getErrorMsg().c_str());
      m_pRateInfo->m_fatalEMsg=m_errMsg;
      return 1;
    }
  }

  if(!(m_userBills[1].empty()))
  {
    t_bRet = m_billMgr->insertUserBill(m_pRateInfo->m_cycParam.m_cycleId,m_userBills[1]);
    if(t_bRet == false)
    {
      m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
      sprintf(m_errMsg,"%d userid=%s update UserBill failed! %s.",
              E_UPDATE_BILL,t_userId.c_str(),
              m_billMgr->getErrorMsg().c_str());
      m_pRateInfo->m_fatalEMsg=m_errMsg;
      return 1;
    }
  }

  //2.更新月为单位累计量
  if(!(m_userSumBill1s[0].empty()))
  {
    t_bRet = m_billMgr->updateUserBill(m_pRateInfo->m_cycParam.m_cycleId,m_userSumBill1s[0]);
    if(t_bRet == false)
    {
      m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
      sprintf(m_errMsg,"%d userid=%s update UserSum1Bill failed! %s.",
              E_UPDATE_BILL,t_userId.c_str(),
              m_billMgr->getErrorMsg().c_str());
      m_pRateInfo->m_fatalEMsg=m_errMsg;
      return 1;
    }
  }

  if(!(m_userSumBill1s[1].empty()))
  {
    t_bRet = m_billMgr->insertUserBill(m_pRateInfo->m_cycParam.m_cycleId,m_userSumBill1s[1]);
    if(t_bRet == false)
    {
      m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
      sprintf(m_errMsg,"%d userid=%s update UserSum1Bill failed! %s.",
              E_UPDATE_BILL,t_userId.c_str(),
              m_billMgr->getErrorMsg().c_str());
      m_pRateInfo->m_fatalEMsg=m_errMsg;
      return 1;
    }
  }

  //3.更新非月为单位累计量(可能需要读取两月的值)
  if(!(m_userSumBill2s[0].empty()))
  {
    t_bRet = m_billMgr->updateUserBill(m_pRateInfo->m_cycParam.m_cycleId,m_userSumBill2s[0]);
    if(t_bRet == false)
    {
      m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
      sprintf(m_errMsg,"%d userid=%s update UserSum2Bill failed! %s.",
              E_UPDATE_BILL,t_userId.c_str(),
              m_billMgr->getErrorMsg().c_str());
      m_pRateInfo->m_fatalEMsg=m_errMsg;
      return 1;
    }
  }

  if(!(m_userSumBill2s[1].empty()))
  {
    t_bRet = m_billMgr->insertUserBill(m_pRateInfo->m_cycParam.m_cycleId,m_userSumBill2s[1]);
    if(t_bRet == false)
    {
      m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
      sprintf(m_errMsg,"%d userid=%s update UserSum2Bill failed! %s.",
              E_UPDATE_BILL,t_userId.c_str(),
              m_billMgr->getErrorMsg().c_str());
      m_pRateInfo->m_fatalEMsg=m_errMsg;
    }
  }

  //4.更新账户帐单
  if(!(m_accountBills[0].empty()))
  {
    t_bRet = m_billMgr->updateUserBill(m_pRateInfo->m_cycParam.m_cycleId,m_accountBills[0]);
    if(t_bRet == false)
    {
      m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
      sprintf(m_errMsg,"%d userid=%s update UserAcctBill failed! %s.",
              E_UPDATE_BILL,t_userId.c_str(),
              m_billMgr->getErrorMsg().c_str());
      m_pRateInfo->m_fatalEMsg=m_errMsg;
    }
  }

  if(!(m_accountBills[1].empty()))
  {
    t_bRet = m_billMgr->insertUserBill(m_pRateInfo->m_cycParam.m_cycleId,m_accountBills[1]);
    if(t_bRet == false)
    {
      m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
      sprintf(m_errMsg,"%d userid=%s update UserAcctBill failed! %s.",
              E_UPDATE_BILL,t_userId.c_str(),
              m_billMgr->getErrorMsg().c_str());
      m_pRateInfo->m_fatalEMsg=m_errMsg;
    }
  }

  //5.更新计费优惠信息
  if(!(m_rateDiscnts[0].empty()))
  {
    t_bRet = m_billMgr->updateUserBill(m_pRateInfo->m_cycParam.m_cycleId,m_rateDiscnts[0]);
    if(t_bRet == false)
    {
      m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
      sprintf(m_errMsg,"%d userid=%s update rateDiscnt failed! %s.",
              E_UPDATE_BILL,t_userId.c_str(),
              m_billMgr->getErrorMsg().c_str());
      m_pRateInfo->m_fatalEMsg=m_errMsg;
    }
  }

  if(!(m_rateDiscnts[1].empty()))
  {
    t_bRet = m_billMgr->insertUserBill(m_pRateInfo->m_cycParam.m_cycleId,m_rateDiscnts[1]);
    if(t_bRet == false)
    {
      m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
      sprintf(m_errMsg,"%d userid=%s update rateDiscnt failed! %s.",
              E_UPDATE_BILL,t_userId.c_str(),
              m_billMgr->getErrorMsg().c_str());
      m_pRateInfo->m_fatalEMsg=m_errMsg;
    }
  }
  //插入回滚信息
  m_billMgr->insertRollBackBill(m_pRateInfo->m_rollBackBills);
   #ifdef FP_DEBUG
    FP_END(UpdateBillProc)
  #endif
  
  return 0;
}; 
*/

int UpdateBillProc::execute()
{
	if (m_arguments == "COMMITTOMDB1")
	{
		#ifdef FP_DEBUG
    		FP_BEGIN(COMMITTOMDB1)
  		#endif
  		
  		#ifdef _DEBUG_
  			cout << " COMMITTOMDB1 " << endl;
  		#endif
  		
		int ret = commitToMdb1();
		
		#ifdef FP_DEBUG
    		FP_END(COMMITTOMDB1)
  		#endif
  		return ret;
	}
	else if (m_arguments == "COMMITTOMDB2")
	{
		#ifdef FP_DEBUG
    		FP_BEGIN(COMMITTOMDB2)
  		#endif
  		
  		#ifdef _DEBUG_
  			cout << " COMMITTOMDB2 " << endl;
  		#endif
  		
		int ret = commitToMdb2();
		
		#ifdef FP_DEBUG
    		FP_END(COMMITTOMDB2)
  		#endif
  		return ret;
	}
	//Added by Sunam 2009/8/20 10:18:15 Commit
	else if (m_arguments == "COMMIT")
	{
	    if (commit())
	    {
	        return 0;
	    }
        return 1;
	}
	//Added by Sunam 2009/8/20 10:18:15 RollBack
	else if (m_arguments == "ROLLBACK")
	{
	    if (rollBack())
	    {
	        return 0;
	    }
        return 1;
	}
	else
	{
		return commitToLocal();
	}
}

int UpdateBillProc::commitToLocal()
{
	#ifdef FP_DEBUG
		FP_BEGIN(CommitToLocal)
	#endif
	T_SERVER_ID &t_userId=m_pRateInfo->m_billUserId;
//Modified by xuf 2009-10-5 19:48:40	
//	if(t_userId == INVALID_SERVER_ID || (*m_pcdr).m_errorCode != E_DEFAULT)
	 if(t_userId.value()<=0 ||
     ((*m_pcdr).m_errorCode != E_DEFAULT &&
     (m_pRateInfo->m_bizType.value()<BIZ_TYPE_BILL.value()&& 
      m_pRateInfo->m_bizType.value()!= BIZ_TYPE_SPMONTH.value() 
      ))) 			
	{
		return 0;
	}
	m_pRateInfo->generateRollBill(true);
	for (int i = 0; i < 2; ++i)
	{
		m_userBills[i].clear();
		m_userSumBill1s[i].clear();
		m_userSumBill2s[i].clear();
		m_accountBills[i].clear(); 
		m_rateDiscnts[i].clear();
		m_userSumBill3s[i].clear();
		m_userMonfeeBills[i].clear();
	}
	
	//准备各项更新的帐单项和新增的帐单项
	for(vector<RollBackBill>::iterator t_billItr = m_pRateInfo->m_rollBackBills.begin();
			t_billItr != m_pRateInfo->m_rollBackBills.end();++t_billItr)
	{
		switch(t_billItr->m_billType)
		{
			case BILL_TYPE_USER : //UserBill
				setBill(m_pRateInfo->m_userBills_new,m_userBills,*t_billItr,t_billItr->m_userId);
				break;
			case BILL_TYPE_SUM1 : //UserSumBill1
				setBill(m_pRateInfo->m_userSumBill1s_new,m_userSumBill1s,*t_billItr,t_billItr->m_userId);
				//add by luxy 2011-02-13 begin
				m_pRateInfo->m_comBill.updateBalanceAcctId(m_pRateInfo->m_balAcctIdList, *t_billItr, BILL_TYPE_SUM1);
				//add by luxy 2011-02-13 end
				break;
			case BILL_TYPE_SUM2 : //UserSumBill2
				setBill(m_pRateInfo->m_userSumBill2s_new,m_userSumBill2s,*t_billItr,t_billItr->m_userId);
				//add by luxy 2011-02-13 begin
				m_pRateInfo->m_comBill.updateBalanceAcctId(m_pRateInfo->m_balAcctIdList, *t_billItr, BILL_TYPE_SUM2);
				//add by luxy 2011-02-13 end
				break;
			case BILL_TYPE_ACCT : //AccountBill
				setBill(m_pRateInfo->m_accountBills_new,m_accountBills,*t_billItr,t_billItr->m_accountId);
				//add by luxy 2011-02-13 begin
				m_pRateInfo->m_comBill. updateBalanceAcctId(m_pRateInfo->m_balAcctIdList, *t_billItr, BILL_TYPE_ACCT);
				//add by luxy 2011-02-13 end
				break;
                 case BILL_TYPE_MONFEEBILL : //UserMonfeeBill
				setBill(m_pRateInfo->m_userMonfeeBills_new,m_userMonfeeBills,*t_billItr,t_billItr->m_userId);
				break;
		  case BILL_TYPE_SUM3: //UserSumBill3
				setBill(m_pRateInfo->m_userSumBill3s_new,m_userSumBill3s,*t_billItr,t_billItr->m_userId);
				break;
			case BILL_TYPE_RATEDISCNT:
				{  //计费优惠特殊处理
					RateDiscntBill  t_ratebill;
					vector<RateDiscntBill> t_ratebillVec;
					map<T_SERVER_ID,vector<RateDiscntBill> >::iterator t_mItr;
					const RateDiscntBill *t_pRateBill=NULL;
					t_ratebill = *t_billItr;
					if(t_billItr->m_operFlag == BILL_OPERATE_INSERT)
					{
						//m_rateDiscnts[1].push_back(t_ratebill);
						t_mItr = m_rateDiscnts[1].find(t_billItr->m_userId);
						if (t_mItr == m_rateDiscnts[1].end())
						{
							t_ratebillVec.push_back(t_ratebill);
							m_rateDiscnts[1].insert(map<T_SERVER_ID,vector<RateDiscntBill> >::value_type(t_billItr->m_userId,t_ratebillVec));
						}
						else
						{
							t_mItr->second.push_back(t_ratebill);
						}
					}
					else
					{
						if (!m_pRateInfo->getBillItem<RateDiscntBill>(m_pRateInfo->m_rateDiscnts,
										t_ratebill,t_pRateBill))
						{
							//m_rateDiscnts[1].push_back(t_ratebill);
							t_mItr = m_rateDiscnts[1].find(t_billItr->m_userId);
							if (t_mItr == m_rateDiscnts[1].end())
							{
								t_ratebillVec.push_back(t_ratebill);
								m_rateDiscnts[1].insert(map<T_SERVER_ID,vector<RateDiscntBill> >::value_type(t_billItr->m_userId,t_ratebillVec));
							}
							else
							{
								t_mItr->second.push_back(t_ratebill);
							}
						}
						else
						{
							t_ratebill+=*t_pRateBill;
							//m_rateDiscnts[0].push_back(t_ratebill);
							t_mItr = m_rateDiscnts[0].find(t_billItr->m_userId);
							if (t_mItr == m_rateDiscnts[0].end())
							{
								t_ratebillVec.push_back(t_ratebill);
								m_rateDiscnts[0].insert(map<T_SERVER_ID,vector<RateDiscntBill> >::value_type(t_billItr->m_userId,t_ratebillVec));
							}
							else
							{
								t_mItr->second.push_back(t_ratebill);
							}
						}
					}
				}
				break;
			default:
        		break;
    	};
	}
	
	//Added By Sunam 2009/10/6 11:27:56
	if (!m_pRateInfo->m_groupUserList.empty())
    {
        vector<UserSumBill2> userSumBill2s[2];
        map<T_SERVER_ID,vector<UserSumBill2> >::iterator t_mItr;
        //遍历群用户列表
        for (int l=0; l<m_pRateInfo->m_groupUserList.size(); ++l)
        {
            for (int m=0; m<2; ++m)
            {
                //待更新账单，如果是群用户账单，不更新到本地，直接更新至altibase
                t_mItr = m_userSumBill2s[m].find(m_pRateInfo->m_groupUserList[l]);
                //是否找当前群用户账单
                if (t_mItr != m_userSumBill2s[m].end())
                {
                    //将其保存至更新数据中
                    for (int n=0; n<t_mItr->second.size(); ++n)
                    {
                        userSumBill2s[m].push_back(t_mItr->second[n]);
                    }
                    //将其从更新到本地的数据中删除
                    m_userSumBill2s[m].erase(t_mItr);
                }
            }
        }
        //插入群用户回滚账单
        if (!m_pRateInfo->m_rollBackBillsGroupUser.empty())
        {
            if (!m_billMgr->insertRollBackBill(m_pRateInfo->m_rollBackBillsGroupUser))
            {
                m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
		        sprintf(m_errMsg,"%d userid=xx update UserRollBackBill failed! %s.",__LINE__,m_billMgr->getErrorMsg().c_str());
		        m_pRateInfo->m_fatalEMsg = m_errMsg;
		        rollBack();
		        return 1;
            }
        }
        //更新到altibase
        if (!userSumBill2s[0].empty())
        {
		    if (!m_billMgr->updateUserBill(m_pRateInfo->m_cycParam.m_cycleId,userSumBill2s[0]))
		    {
		    	m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
		    	sprintf(m_errMsg,"%d userid=xx update UserSum2Bill failed! %s.",__LINE__,m_billMgr->getErrorMsg().c_str());
		    	m_pRateInfo->m_fatalEMsg = m_errMsg;
		    	rollBack();
		    	return 1;
		    }
        }
        //插入到altibase
        if (!userSumBill2s[1].empty())
        {
	    	if (!m_billMgr->insertUserBill(m_pRateInfo->m_cycParam.m_cycleId,userSumBill2s[1]))
	    	{
	    		m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
	    		sprintf(m_errMsg,"%d userid=xx update UserAcctBill failed! %s.",__LINE__,m_billMgr->getErrorMsg().c_str());
	    		m_pRateInfo->m_fatalEMsg = m_errMsg;
	    		rollBack();
	    		return 1;
	    	}
	    }
	    commit();
    }

	bool t_bRet = true;
	int cycleIndex = m_pcdr[ATTRI_CYCLE_DATE].value() % 100 -1;
	
	//1.更新计费帐单
	updateToBillMap(m_userBills[0],m_pRateInfo->m_tUserBill[cycleIndex]);
	insertToBillMap(m_userBills[1],m_pRateInfo->m_tUserBill[cycleIndex]);

	//2.更新非月为单位累计量(可能需要读取两月的值)
	updateToBillMap(m_userSumBill2s[0],m_pRateInfo->m_tUserSumBill2);
	insertToBillMap(m_userSumBill2s[1],m_pRateInfo->m_tUserSumBill2);
	
 	//4.更新计费优惠信息
 	updateToBillMap(m_rateDiscnts[0],m_pRateInfo->m_tRateDiscnts[cycleIndex]);
	insertToBillMap(m_rateDiscnts[1],m_pRateInfo->m_tRateDiscnts[cycleIndex]);
	
		//6.更新非月为单位累计量,包月费累计
	updateToBillMap(m_userSumBill3s[0],m_pRateInfo->m_tUserSumBill3);
	insertToBillMap(m_userSumBill3s[1],m_pRateInfo->m_tUserSumBill3);

  cycleIndex = m_pRateInfo->m_sum1BillCycle % 100 - 1;
	//5.更新月为单位累计量
	updateToBillMap(m_userSumBill1s[0],m_pRateInfo->m_tUserSumBill1[cycleIndex]);
	insertToBillMap(m_userSumBill1s[1],m_pRateInfo->m_tUserSumBill1[cycleIndex]);
	
	cycleIndex = m_pcdr[ATTRI_CYCLE_DATE].value() % 2;
	//3.更新账户帐单,账户可能是多个
 	updateToBillMap(m_accountBills[0],m_pRateInfo->m_tAccountBill[cycleIndex]);
	insertToBillMap(m_accountBills[1],m_pRateInfo->m_tAccountBill[cycleIndex]);
	
		
		//7.更新用户包月费账单
	updateToBillMap(m_userMonfeeBills[0],m_pRateInfo->m_tUserMonfeeBill[cycleIndex]);
	insertToBillMap(m_userMonfeeBills[1],m_pRateInfo->m_tUserMonfeeBill[cycleIndex]);
	
	#ifdef FP_DEBUG
		FP_END(CommitToLocal)
	#endif
	return 0;
}

//Added By Sunam 2009/8/20 10:10:38
int UpdateBillProc::commit()
{
    if (m_billMgr->commit())
    {
        return 0;
    }
    return 1;
}

int UpdateBillProc::rollBack()
{
    if (m_billMgr->rollBack())
    {
        return 0;
    }
    return 1;
}

int UpdateBillProc::commitToMdb1()
{
	//插入回滚信息
    //if (m_billMgr->insertRollBackBill(m_pRateInfo->m_rollBackBills))
    if (m_billMgr->insertRollBackBill(m_pRateInfo->m_rollBackBillsAll))
	{
	    //返回0表示成功
	    if(commitToMdb2() == 0)
	    {
	        return commit();
	    }
	    else
	    {
	        return rollBack();
	    }
	}
	//插入回滚账单失败
	else
    {
        return rollBack();
    }
}

int UpdateBillProc::commitToMdb2()
{
	vector<UserBill >      userBills[12][2];
	vector<UserMonfeeBill> userMonfeeBills[2][2];
	vector<UserSumBill1>   userSumBill1s[12][2];
	vector<UserSumBill2>   userSumBill2s[2];
	vector<UserSumBill3>   userSumBill3s[2];//累积量3（包月费累积量定义）
	vector<AccountBill>    accountBills[2][2];
	vector<RateDiscntBill> rateDiscnts[12][2];
	
	int index;
	
	for (index = 0; index < 2; ++index)
	{
		//1.生成UserBill的增量账单
		//genIncreBill(m_pRateInfo->m_tUserBill_org[index],m_pRateInfo->m_tUserBill[index],userBills[index]);
		//2.生成AccountBill的增量账单
		genIncreBill(m_pRateInfo->m_tAccountBill_org[index],m_pRateInfo->m_tAccountBill[index],accountBills[index]);
 		//3.生成RateDiscntBill的增量账单
 		//genIncreBill(m_pRateInfo->m_tRateDiscnts_org[index],m_pRateInfo->m_tRateDiscnts[index],rateDiscnts[index]);
 		//生成UserMonfeeBill的增量账单                                                                                            
 	  genIncreBill(m_pRateInfo->m_tUserMonfeeBill_org[index],m_pRateInfo->m_tUserMonfeeBill[index],userMonfeeBills[index]);		
	
	}
	
	for (index = 0; index < 12; ++index)
	{
		//1.生成UserBill的增量账单
		genIncreBill(m_pRateInfo->m_tUserBill_org[index],m_pRateInfo->m_tUserBill[index],userBills[index]);
		//2.生成AccountBill的增量账单
		//genIncreBill(m_pRateInfo->m_tAccountBill_org[index],m_pRateInfo->m_tAccountBill[index],accountBills[index]);
 		//3.生成RateDiscntBill的增量账单
 		genIncreBill(m_pRateInfo->m_tRateDiscnts_org[index],m_pRateInfo->m_tRateDiscnts[index],rateDiscnts[index]);
 		//生成UserMonfeeBill的增量账单                                                                                            
 	  //genIncreBill(m_pRateInfo->m_tUserMonfeeBill_org[index],m_pRateInfo->m_tUserMonfeeBill[index],userMonfeeBills[index]);		
	}
	//4.生成UserSumBill1的增量账单
	for (index = 0; index < 12; ++index)
	{
		genIncreBill(m_pRateInfo->m_tUserSumBill1_org[index],m_pRateInfo->m_tUserSumBill1[index],userSumBill1s[index]);
	}
	//5.生成UserSumBill2的增量账单
	genIncreBill(m_pRateInfo->m_tUserSumBill2_org,m_pRateInfo->m_tUserSumBill2,userSumBill2s);
	
//6.生成UserSumBill3的增量账单
	genIncreBill(m_pRateInfo->m_tUserSumBill3_org,m_pRateInfo->m_tUserSumBill3,userSumBill3s);
	
	//Added By Sunam 2009/9/28 19:52:42 当前更新累计量 Begin 在OutPuter中清空
	for (index = 0; index < 12; ++index)
	{
	    if (userSumBill1s[index][0].size() > 0)
	    {
	        m_pRateInfo->m_userSumBill1Update[0].insert(map<int,vector<UserSumBill1> >::value_type(index+1,userSumBill1s[index][0]));
	    }
	    if (userSumBill1s[index][1].size() > 0)
	    {
	        m_pRateInfo->m_userSumBill1Update[1].insert(map<int,vector<UserSumBill1> >::value_type(index+1,userSumBill1s[index][1]));
	    }
	}
	for (int i=0; i<userSumBill2s[0].size(); ++i)
	{
	    m_pRateInfo->m_userSumBill2Update[0].push_back(userSumBill2s[0][i]);
	}
	for (int i=0; i<userSumBill2s[1].size(); ++i)
	{
	    m_pRateInfo->m_userSumBill2Update[1].push_back(userSumBill2s[1][i]);
	}
	//Added By Sunam 2009/9/28 19:52:42 当前更新累计量 End
	
	bool t_bRet = true;
 	
 	for (index = 0; index < 12; ++index)
 	{
 		//1.更新计费帐单
 		if(!(userBills[index][0].empty()))
 		{
			t_bRet = m_billMgr->updateUserBill(index+1,userBills[index][0]);
			if(t_bRet == false)
			{
				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d] userid=xx update UserBill failed! %s.",__FILE__,__LINE__,m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
			}
 		}
 		if(!(userBills[index][1].empty()))
 		{
			t_bRet = m_billMgr->insertUserBill(index+1,userBills[index][1]);
			if(t_bRet == false)
			{
				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d:%d] userid=xx insert UserBill failed! %s.",__FILE__,__LINE__,index,m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
			}
 		}
 		//3.更新计费优惠信息
 		if(!(rateDiscnts[index][0].empty()))
 		{
			t_bRet = m_billMgr->updateUserBill(index+1,rateDiscnts[index][0]);
			if(t_bRet == false)
			{
				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d] userid=xx update rateDiscnts failed! %s.",__FILE__,__LINE__,m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
			}
 		}
 		if(!(rateDiscnts[index][1].empty()))
 		{
			t_bRet = m_billMgr->insertUserBill(index+1,rateDiscnts[index][1]);
			if(t_bRet == false)
			{
				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d] userid=xx update rateDiscnts failed! %s.",__FILE__,__LINE__,m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
			}
 		}
 		//1.更新月为单位累计量
 		if(!(userSumBill1s[index][0].empty()))
 		{
 			//index需要+1，即真实的月份
			t_bRet = m_billMgr->updateUserBill(index+1,userSumBill1s[index][0]);
			if(t_bRet == false)
			{
				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d] userid=xx update userSumBill1 failed! %s.",__FILE__,__LINE__,m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
			}
 		}
 		if(!(userSumBill1s[index][1].empty()))
 		{
 			//index需要+1，即真实的月份
 			t_bRet = m_billMgr->insertUserBill(index+1,userSumBill1s[index][1]);
 			if(t_bRet == false)
 			{
 				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d] userid=xx insert userSumBill1 failed! %s.",__FILE__,__LINE__,m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
 			}
 		}
 	}
 	
 		for (index = 0; index < 2; ++index)
	{
 		//1.更新计费帐单
 		/*
 		if(!(userBills[index][0].empty()))
 		{
			t_bRet = m_billMgr->updateUserBill(index,userBills[index][0]);
			if(t_bRet == false)
			{
				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d] userid=xx update UserBill failed! %s.",__FILE__,__LINE__,m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
			}
 		}
 		if(!(userBills[index][1].empty()))
 		{
			t_bRet = m_billMgr->insertUserBill(index,userBills[index][1]);
			if(t_bRet == false)
			{
				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d:%d] userid=xx insert UserBill failed! %s.",__FILE__,__LINE__,index,m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
			}
 		}*/
 		//2.更新账户帐单
 		if(!(accountBills[index][0].empty()))
 		{
			t_bRet = m_billMgr->updateUserBill(index,accountBills[index][0]);
			if(t_bRet == false)
			{
				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d] userid=xx update acctountBill failed! %s.",__FILE__,__LINE__,m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
			}
 		}
 		if(!(accountBills[index][1].empty()))
 		{
			t_bRet = m_billMgr->insertUserBill(index,accountBills[index][1]);
			if(t_bRet == false)
			{
				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d] inputfile=[%s] insert acctountBill failed! %s.",__FILE__,__LINE__,m_pRateInfo->m_fileName.c_str(),m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
			}
 		}
 		
 		//7.更新包月费账单
 		if(!(userMonfeeBills[index][0].empty()))
 		{
			t_bRet = m_billMgr->updateUserBill(index,userMonfeeBills[index][0]);
			if(t_bRet == false)
			{
				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d] userid=xx update UserMonfeeBill failed! %s.",__FILE__,__LINE__,m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
			}
 		}
 		if(!(userMonfeeBills[index][1].empty()))
 		{
			t_bRet = m_billMgr->insertUserBill(index,userMonfeeBills[index][1]);
			if(t_bRet == false)
			{
				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d] userid=xx insert UserMonfeeBill failed! %s.",__FILE__,__LINE__,m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
			}
 		//3.更新计费优惠信息
 		/*
 		if(!(rateDiscnts[index][0].empty()))
 		{
			t_bRet = m_billMgr->updateUserBill(index,rateDiscnts[index][0]);
			if(t_bRet == false)
			{
				m_pRateInfo->m_fatalECode = E_UPDATE_BILL;
				sprintf(m_errMsg,"[%s:%d] userid=xx update rateDiscnts failed! %s.",__FILE__,__LINE__,m_billMgr->getErrorMsg().c_str());
				m_pRateInfo->m_fatalEMsg = m_errMsg;
				m_billMgr->rollBack();
				return 1;
			}
 		}
 		if(!(rateDiscnts[index][1].empty()))
 		{
			t_bRet = m_billMg