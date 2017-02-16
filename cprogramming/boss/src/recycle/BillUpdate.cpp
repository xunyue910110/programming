#include "BillUpdate.h"
#include "BillManager.h"
#include <algorithm>

void BillUpdate::initialization(BillManager* r_billMgr,int& r_channelNo)
{
	m_billMgr = r_billMgr;
	m_channelNo = r_channelNo;//cout<<"m_channelNo:"<<m_channelNo<<endl;     
}

bool BillUpdate::update(const int& r_billCycle,T_SERVER_ID& r_userId, 
	                    char* r_billItem,vector<RollBackBill>& r_rollBackBillVec)
{
	splitBillItem(r_billCycle,r_userId,r_billItem,r_rollBackBillVec);
	UserBill     t_UserBill    ;
    UserSumBill1 t_UserSumBill1;
    UserSumBill2 t_UserSumBill2;
    vector<RollBackBill> &t_userRollBillVec = r_rollBackBillVec;
	for(int i = 0; i < t_userRollBillVec.size(); ++i)
	{
		switch(t_userRollBillVec[i].m_billType)
  	    {
  	      case 0 : //UserBill
  	      	t_UserBill = t_userRollBillVec[i];
  	      	updateToLocalBillMap(t_UserBill,m_tUserBill[t_userRollBillVec[i].m_billCycle%100-1]);
  	        break;
  	      case 1 : //UserSumBill1
  	      	t_UserSumBill1 = t_userRollBillVec[i];
  	      	updateToLocalBillMap(t_UserSumBill1,m_tUserSumBill1[t_userRollBillVec[i].m_billCycle%100-1]);
  	        break;
  	      case 2 : //UserSumBill2
  	      	t_UserSumBill2 = t_userRollBillVec[i];
  	      	updateToLocalBillMap(t_UserSumBill2,m_tUserSumBill2);
  	        break;
  	      default:
  	        break;
  	    }
	}
	
	return 1;
}

bool BillUpdate::splitBillItem(const int& r_billCycle,T_SERVER_ID& r_userId,
	                           char* r_billItem,vector<RollBackBill>& r_rollBackBill)
{
	//账单轨迹信息为:{用户ID|账单类型|帐期|资费实例|起始时间|账单项|值}
	// 多个信息间 以 分号 分割
	char cTmp[200],*pTmp;
	RollBackBill t_rollBackBill;
	vector<string> t_billItem;
	vector<string>::iterator iter;
	r_rollBackBill.clear();
	
	//Added By Sunam 2009/9/11 14:26:32 Begin截掉:及以后的内容
	pTmp = strtok(r_billItem,":");
	r_billItem = pTmp;
	//Added By Sunam 2009/9/11 14:26:32 End
	
	pTmp = strtok(r_billItem,";");
    while (pTmp != NULL)
    {
        t_billItem.push_back(pTmp);
	    pTmp = strtok(NULL,";");
    }
    
    for(iter=t_billItem.begin();iter!=t_billItem.end();++iter)
    {
    	sprintf(cTmp,"%s",(*iter).c_str());
    	pTmp = strtok(cTmp,"|");
		if( pTmp == NULL ) {
		    //m_theErrorMessages->insert(EAPPFRM_SYSVAL,path);
		    return false;
		}
		t_rollBackBill.m_userId = pTmp;
		if(t_rollBackBill.m_userId == 0)
		{//默认为清单中的userId
			t_rollBackBill.m_userId = r_userId;
		}
		else
		{
			r_userId = pTmp;
		}
		
		pTmp = strtok(NULL,"|");
		if( pTmp == NULL ) {
		    //m_theErrorMessages->insert(EAPPFRM_SYSVAL,path);
		    return false;
		}
		t_rollBackBill.m_billType = atoi(pTmp);
		
		pTmp = strtok(NULL,"|");
		if( pTmp == NULL ) {
		    //m_theErrorMessages->insert(EAPPFRM_SYSVAL,path);
		    return false;
		}
		t_rollBackBill.m_billCycle = atoi(pTmp);
		//Modified By Sunam 2009/10/10 10:54:33 计费的累计量2的cycleId总是0
		//if(t_rollBackBill.m_billCycle == 0)
		if ((t_rollBackBill.m_billCycle == 0) && (t_rollBackBill.m_billType != 2))
			t_rollBackBill.m_billCycle = r_billCycle;
		
		pTmp = strtok(NULL,"|");
		if( pTmp == NULL ) {
		    //m_theErrorMessages->insert(EAPPFRM_SYSVAL,path);
		    return false;
		}
		t_rollBackBill.m_tpObjId = atol(pTmp);
		
		pTmp = strtok(NULL,"|");
		if( pTmp == NULL ) {
		    //m_theErrorMessages->insert(EAPPFRM_SYSVAL,path);
		    return false;
		}
		t_rollBackBill.m_beginTime = atol(pTmp);
		
		pTmp = strtok(NULL,"|");
		if( pTmp == NULL ) {
		    //m_theErrorMessages->insert(EAPPFRM_SYSVAL,path);
		    return false;
		}
		t_rollBackBill.m_billId = atoi(pTmp);
		
		pTmp = strtok(NULL,"|");
		if( pTmp == NULL ) {
		    //m_theErrorMessages->insert(EAPPFRM_SYSVAL,path);
		    return false;
		}
		t_rollBackBill.m_value = atol(pTmp);
		
		t_rollBackBill.m_channelNo = m_channelNo;
		
		//一些从billitem中取不到的值的赋值
		t_rollBackBill.m_endTime = 0;
		t_rollBackBill.m_discntValue = 0;
		t_rollBackBill.m_a_discnt    = 0;
		t_rollBackBill.m_b_discnt    = 0;
		t_rollBackBill.m_accountId   = 0;
		t_rollBackBill.m_type = 0;
		t_rollBackBill.m_groupUserId = "0";
		r_rollBackBill.push_back(t_rollBackBill);//cout<<"t_rollBackBill:"<<t_rollBackBill<<endl;
    }
    
	return true;
}

bool BillUpdate::rollbackOldBill()
{
	vector<RollBackBill> t_rollBackBill;
	vector<RollBackBill> t_userRollBill; //一个用户的回退帐单
	m_billMgr->clearUserLock(m_channelNo);
	m_billMgr->getRollBackBill(m_channelNo,t_rollBackBill);
	UserBill     t_UserBill    ;
    UserSumBill1 t_UserSumBill1;
    UserSumBill2 t_UserSumBill2;
    vector<RollBackBill>& t_rollBackBillVec = t_rollBackBill;
  
	for(int i = 0; i < t_rollBackBillVec.size(); ++i)
	{
		switch(t_rollBackBillVec[i].m_billType)
  	{
  	  case 0 : //UserBill
  	  	t_UserBill = t_rollBackBillVec[i];
  	  	updateToLocalBillMap(t_UserBill,m_tUserBill[t_rollBackBillVec[i].m_billCycle%100-1]);
  	    break;
  	  case 1 : //UserSumBill1
  	  	t_UserSumBill1 = t_rollBackBillVec[i];
  	  	updateToLocalBillMap(t_UserSumBill1,m_tUserSumBill1[t_rollBackBillVec[i].m_billCycle%100-1]);
  	    break;
  	  case 2 : //UserSumBill2
  	  	t_UserSumBill2 = t_rollBackBillVec[i];
  	  	updateToLocalBillMap(t_UserSumBill2,m_tUserSumBill2);
  	    break;
  	  default:
  	    break;
  	}
	}
	
	for (int i = 0; i < 12; ++i)
	{
		m_tUserBill[i].clear();
		m_tUserSumBill1[i].clear();
		m_tUserBill_org[i].clear();
		m_tUserSumBill1_org[i].clear();
	}
	m_tUserSumBill2.clear();
	m_tUserSumBill2_org.clear();
	m_billMgr->clearRollBackBill(m_channelNo);
	return true;
}

bool BillUpdate::clearRollBackBill()
{
	return m_billMgr->clearRollBackBill(m_channelNo);
}

//Added By Sunam 2011/3/22 9:57:03	
template<class T>
void BillUpdate::updateToLocalBillMap(const T &r_userBill,map<T_SERVER_ID,vector<T> > &r_localBill)
{
	map<T_SERVER_ID,vector<T> >::iterator t_mItr;
	vector<T>::iterator t_vItr;
	//根据userId或acctId在r_localBill中查找
	t_mItr = r_localBill.find(r_userBill.m_userId);
	//该用户账单已存在
	if (t_mItr != r_localBill.end())
	{
		//查找当前账单在r_localBill中的位置
		t_vItr = ::lower_bound(t_mItr->second.begin(),t_mItr->second.end(),r_userBill);
		//如果未找到当前账单项则新增
		if (t_vItr == t_mItr->second.end() || (!(*t_vItr==r_userBill)))
		{
			t_mItr->second.insert(t_vItr,r_userBill);
		}
		//否则累加费用
		else
		{
			*t_vItr += r_userBill;
		}
	}
	//该用户账单不存在
	else
	{
		vector<T> t_vecUserBill;
		t_vecUserBill.push_back(r_userBill);
		r_localBill.insert(map<T_SERVER_ID,vector<T> >::value_type(r_userBill.m_userId,t_vecUserBill));
	}
}

//Added By Sunam 2011/3/23 9:29:32 根据需要回滚的账单去查询用户原始账单
void BillUpdate::getUserBill()
{
	  map<T_SERVER_ID,vector<UserBill>     >::iterator t_mtrUserBill;
	  map<T_SERVER_ID,vector<UserSumBill1> >::iterator t_mtrUserSumBill1;
	  map<T_SERVER_ID,vector<UserSumBill2> >::iterator t_mtrUserSumBill2;
	  vector<UserBill>     t_userBillVec;
	  vector<UserSumBill1> t_userSumBill1Vec;
	  vector<UserSumBill2> t_userSumBill2Vec;
	  int t_index;
	  for (t_index = 0; t_index < 12; ++t_index)
	  {
	  	for (t_mtrUserBill = m_tUserBill[t_index].begin(); t_mtrUserBill != m_tUserBill[t_index].end(); ++t_mtrUserBill)
	  	{
	  		m_billMgr->getUserBill(t_index+1,t_mtrUserBill->first,t_userBillVec);
	  		::sort(t_userBillVec.begin(),t_userBillVec.end());
	  		m_tUserBill_org[t_index].insert(map<T_SERVER_ID,vector<UserBill> >::value_type(t_mtrUserBill->first,t_userBillVec));
	  		t_userBillVec.clear();
	  	}
	  	for (t_mtrUserSumBill1 = m_tUserSumBill1[t_index].begin(); t_mtrUserSumBill1 != m_tUserSumBill1[t_index].end(); ++t_mtrUserSumBill1)
	  	{
	  		m_billMgr->getUserBill(t_index+1,t_mtrUserSumBill1->first,t_userSumBill1Vec);
	  		::sort(t_userSumBill1Vec.begin(),t_userSumBill1Vec.end());
	  		m_tUserSumBill1_org[t_index].insert(map<T_SERVER_ID,vector<UserSumBill1> >::value_type(t_mtrUserSumBill1->first,t_userSumBill1Vec));
	  		t_userSumBill1Vec.clear();
	  	}
	  }
	  for (t_mtrUserSumBill2 = m_tUserSumBill2.begin(); t_mtrUserSumBill2 != m_tUserSumBill2.end(); ++t_mtrUserSumBill2)
	  {
	  	m_billMgr->getUserBill(0,t_mtrUserSumBill2->first,t_userSumBill2Vec);
	  	::sort(t_userSumBill2Vec.begin(),t_userSumBill2Vec.end());
	  	m_tUserSumBill2_org.insert(map<T_SERVER_ID,vector<UserSumBill2> >::value_type(t_mtrUserSumBill2->first,t_userSumBill2Vec));
	  	t_userSumBill2Vec.clear();
	  }
}

template<class T>
void BillUpdate::genIncreBill(map<T_SERVER_ID,vector<T> > &r_orgBill, map<T_SERVER_ID,vector<T> > &r_newBill, const bool &flag, vector<T> r_increBill[2])
{
	map<T_SERVER_ID,vector<T> >::iterator t_mItr1,t_mItr2;
	vector<T>::iterator t_vItr1,t_vItr2;
	for (t_mItr1 = r_newBill.begin(); t_mItr1 != r_newBill.end(); ++t_mItr1)
	{
		t_mItr2 = r_orgBill.find(t_mItr1->first);
		//新增的用户
		if (t_mItr2 == r_orgBill.end())
		{
			//将该用户所有账单保存
			for (t_vItr1 = t_mItr1->second.begin(); t_vItr1 != t_mItr1->second.end(); ++t_vItr1)
			{
				//费用清零
				t_vItr1->clearValue();
				r_increBill[1].push_back(*t_vItr1);
			}
		}
		//已经存在的用户
		else
		{
			//遍历该用户账单
			for (t_vItr1 = t_mItr1->second.begin(); t_vItr1 != t_mItr1->second.end(); ++t_vItr1)
			{
				t_vItr2 = ::lower_bound(t_mItr2->second.begin(),t_mItr2->second.end(),*t_vItr1);
				//新增账单
				if (t_vItr2 == t_mItr2->second.end() || !(*t_vItr2 == *t_vItr1))
				{
					//费用清零
					t_vItr1->clearValue();
					r_increBill[1].push_back(*t_vItr1);
				}
				else
				{
					//账单回退时falg=-1,异常恢复时falg=1
					if (t_vItr2->m_value < t_vItr1->m_value)
					{
					    t_vItr2->m_value = 0;
					}
					else
					{
					    *t_vItr2 -= *t_vItr1;
					}
					r_increBill[0].push_back(*t_vItr2);
				}
			}	
		}
	}
}

//Added By Sunam 2011/3/23 9:48:25 提交账单
bool BillUpdate::commit()
{
	int t_index;
	bool t_bRet;
	char errMsg[500];
	memset (errMsg,0,sizeof(errMsg));
	//根据需要回滚的账单查询用户的原始账单
	getUserBill();
	//回滚用户账单，并得到需要更新的账单
	for (t_index = 0; t_index < 12; ++t_index)
	{
		genIncreBill(m_tUserBill_org[t_index],m_tUserBill[t_index],true,m_increUserBill[t_index]);
		genIncreBill(m_tUserSumBill1_org[t_index],m_tUserSumBill1[t_index],true,m_increUserSumBill1[t_index]);
	}
	genIncreBill(m_tUserSumBill2_org,m_tUserSumBill2,true,m_increUserSumBill2);
	//
	for (t_index = 0; t_index < 12; ++t_index)
 	{
 		//1.更新计费帐单
 		if(!(m_increUserBill[t_index][0].empty()))
 		{
			t_bRet = m_billMgr->updateUserBill(t_index+1,m_increUserBill[t_index][0]);
			if(t_bRet == false)
			{
				sprintf(errMsg,"[%s:%d:%d] userid=xx update UserBill failed! %s.",__FILE__,__LINE__,t_index,m_billMgr->getErrorMsg().c_str());
				m_billMgr->rollBack();
				return false;
			}
 		}
 		if(!(m_increUserBill[t_index][1].empty()))
 		{
			t_bRet = m_billMgr->insertUserBill(t_index+1,m_increUserBill[t_index][1]);
			if(t_bRet == false)
			{
				sprintf(errMsg,"[%s:%d:%d] userid=xx insert UserBill failed! %s.",__FILE__,__LINE__,t_index,m_billMgr->getErrorMsg().c_str());
				m_billMgr->rollBack();
				return false;
			}
 		}
 		//2.更新累积量1
 		if(!(m_increUserSumBill1[t_index][0].empty()))
 		{
			t_bRet = m_billMgr->updateUserBill(t_index+1,m_increUserSumBill1[t_index][0]);
			if(t_bRet == false)
			{
				sprintf(errMsg,"[%s:%d:%d] userid=xx update UserBill failed! %s.",__FILE__,__LINE__,t_index,m_billMgr->getErrorMsg().c_str());
				m_billMgr->rollBack();
				return false;
			}
 		}
 		if(!(m_increUserSumBill1[t_index][1].empty()))
 		{
			t_bRet = m_billMgr->insertUserBill(t_index+1,m_increUserSumBill1[t_index][1]);
			if(t_bRet == false)
			{
				sprintf(errMsg,"[%s:%d:%d] userid=xx insert UserBill failed! %s.",__FILE__,__LINE__,t_index,m_billMgr->getErrorMsg().c_str());
				m_billMgr->rollBack();
				return false;
			}
 		}
	}
	//3.更新累积量2
 	if(!(m_increUserSumBill2[0].empty()))
 	{
		t_bRet = m_billMgr->updateUserBill(t_index+1,m_increUserSumBill2[0]);
		if(t_bRet == false)
		{
			sprintf(errMsg,"[%s:%d] userid=xx update UserBill failed! %s.",__FILE__,__LINE__,m_billMgr->getErrorMsg().c_str());
			m_billMgr->rollBack();
			return false;
		}
 	}
 	if(!(m_increUserSumBill2[1].empty()))
 	{
		t_bRet = m_billMgr->insertUserBill(t_index+1,m_increUserSumBill2[1]);
		if(t_bRet == false)
		{
			sprintf(errMsg,"[%s:%d] userid=xx insert UserBill failed! %s.",__FILE__,__LINE__,m_billMgr->getErrorMsg().c_str());
			m_billMgr->rollBack();
			return false;
		}
 	}
 	m_billMgr->commit();
 	
	for (t_index = 0; t_index < 12; ++t_index)
	{
	    m_tUserBill[t_index].clear();
	    m_tUserBill_org[t_index].clear();
	    m_tUserSumBill1[t_index].clear();
	    m_tUserSumBill1_org[t_index].clear();
	    m_increUserBill[t_index][0].clear();
	    m_increUserBill[t_index][1].clear();
	    m_increUserSumBill1[t_index][0].clear();
	    m_increUserSumBill1[t_index][1].clear();
	}
	m_tUserSumBill2.clear();
	m_tUserSumBill2_org.clear();
	m_increUserSumBill2[0].clear();
	m_increUserSumBill2[1].clear();
	
 	return true;
}
