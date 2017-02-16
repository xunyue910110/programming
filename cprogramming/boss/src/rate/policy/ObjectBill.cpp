#include "ObjectBill.h"
#include "RateGlobalInfo.h"
#include "base/exceptions.h"
#include "AcctBillMgr.h"

extern RateGlobalInfo g_globalInfo;
RateGlobalInfo* ObjectBill::m_pRateInfo = &g_globalInfo;

ObjectBill::ObjectBill(){}
ObjectBill::~ObjectBill(){}

void ObjectBill::dump(char *prefix)
{
	char myPrefix[256] = "  ";
	strcat(myPrefix,prefix);
	cout <<myPrefix<<"{" <<endl;
	cout <<myPrefix<<"---- ObjectBill : " <<m_id<<" ----" <<endl;
	ObjectBase::dump(myPrefix);	
	cout <<myPrefix<<"m_attrSource = "<< m_attrSource <<endl;
    cout <<myPrefix<<"m_condId = "<< m_condId <<endl;
	cout <<myPrefix<<"m_itemType   = "<< m_itemType<<endl;
	cout <<myPrefix<<"m_roleCode   = "<< m_roleCode<<endl;
	cout <<myPrefix<<"}" <<endl;
}

bool ObjectBill::getObjectValue(T_OBJECT_ATTR &value)
{
	long intValue;
	if(m_roleCode== ROLECODE_ENUM)
	{
		//根据角色获取群内指定用户集合,并初始化
		getRoleUsers(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId);
		getValue(intValue);
	}
	else if (m_roleCode== ROLECODE_CONDID || m_roleCode== ROLECODE_ENUMCONDID)
	{
		//根据条件ID(或角色加条件)获取用户集合
		getUsers(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId);
		getValue(intValue);
	}
	else 
	{
		getValue(intValue);
	}
	value = intValue;

	return true;
}

bool ObjectBill::getObjectValue(long &value)
{
	value=0;
	//清空用户集合
	m_globalInfo->m_detailBillMgr.m_userIds.clear();
	if (m_roleCode==ROLECODE_SELF)         //参考自身
	{
		//取自身的值
		getValue(value);
	}
	else if(m_roleCode==ROLECODE_WIDECARD) //参考群（或账户）总量
	{
		if (m_globalInfo->m_detailBillMgr.getIdType()==USER_ID_TYPE)//当前作用的是用户
		{
			if ((m_policyInfo->m_rateTpInfo->m_pUserTp->m_relationVpnId==0&&
				  m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType==(char)USER_ID_TYPE)||
				 (m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType==(char)ACCT_ID_TYPE&&
				  m_policyInfo->m_rateTpInfo->m_execMode!=EXEC_METHOD_ACCTPARTINHERIT
				  ))//用户无关联群 或账户继承型不参考账户
			{
				//取自身的值
				getValue(value);	
			}	
			else if (m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType==(char)ACCT_ID_TYPE&&
				      m_policyInfo->m_rateTpInfo->m_execMode==EXEC_METHOD_ACCTPARTINHERIT)//用户参考账户 add by xuf 2010-1-30 20:07:50 
			{
			   //根据账户id和role取值
				getRoleBillValue(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId, value,ACCT_ID_TYPE);
			}	
			else
			{
				//根据群id和role取值
				getRoleBillValue(m_policyInfo->m_rateTpInfo->m_pUserTp->m_relationVpnId, value);
			}
		}
		else //if (m_globalInfo->m_detailBillMgr.getIdType()==GROUP_ID_TYPE)//当前作用的是群
		{
			//取自身的值
			getValue(value);
		}	
	}   
	else if (m_roleCode==ROLECODE_ACCT) //参考账户总量 add by xuf 2010-2-1 0:30:34
	{
		if (m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType==(char)ACCT_ID_TYPE&&
				m_policyInfo->m_rateTpInfo->m_execMode==EXEC_METHOD_ACCTPARTINHERIT)//用户参考账户 add by xuf 2010-1-30 20:07:50 
		{
			   //根据账户id和role取值
				getRoleBillValue(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId, value,ACCT_ID_TYPE);
		}
		else 
		{
			//取自身的值
			getValue(value);
		}				
	}  
	else if(m_roleCode== ROLECODE_ENUM)
	{
		//根据角色获取群内指定用户集合,并初始化
		getRoleUsers(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId);
		getValue(value);
	}
	else if (m_roleCode== ROLECODE_CONDID || m_roleCode== ROLECODE_ENUMCONDID)
	{
		//根据条件ID(或角色加条件)获取用户集合
		getUsers(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId);
		getValue(value);
	}  
	else //if (roleCode>"0")//具体角色编码"
	{
		if (m_globalInfo->m_detailBillMgr.getIdType()==USER_ID_TYPE)//当前作用的是用户
		{
			if(m_roleCode==ROLECODE_GROUPALL)//通过虚拟用户ID 取群下总费用 add by dingr 2010-08-30
			{
				getRoleBillValue(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId, value);			
			}
			else if (m_policyInfo->m_rateTpInfo->m_pUserTp->m_relationVpnId.value()==0)//modify by xuf 2009-12-24 23:56:41
			{
				//取自身的值
				getValue(value);	
			}	
			else
			{ 
				//根据群id和role取值
				getRoleBillValue(m_policyInfo->m_rateTpInfo->m_pUserTp->m_relationVpnId, value);
			}
		}
		else if (m_globalInfo->m_detailBillMgr.getIdType()==GROUP_ID_TYPE)//当前作用的是群
		{
			//根据群id和role取值
			getRoleBillValue(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId, value);			
		}
		else //帐户或客户
		{
			//取自身的值
			getValue(value);
		}
	}

	return true;
}

void ObjectBill::getValue(long &intValue)
{
	if(m_attrSource == ATTR_SOURCE_FACT)//获取账目实际费用
	{
		m_globalInfo->m_detailBillMgr.getFactFee(m_attrId,intValue);
	}
	else if(m_attrSource == ATTR_SOURCE_CUR)//获取账目当前费用(上一个优惠作用后的费用)
	{
		m_globalInfo->m_detailBillMgr.getCurFee(m_attrId,m_itemType,intValue);
	}	
	else if(m_attrSource == ATTR_SOURCE_ORIGIN)//获取账目原始费用(优惠作用前的费用)
	{
		m_globalInfo->m_detailBillMgr.getOriginFee(m_attrId,m_itemType,intValue);
	}
	else if(m_attrSource == ATTR_SOURCE_ITEM_NUM)//明细帐目数量
	{
		m_globalInfo->m_detailBillMgr.getItemNum(m_attrId,intValue);
	}
	else if(m_attrSource == ATTR_SOURCE_BIT_NUM)//按bit位来统计数量

	{	

	  UserBill t_userBill;

	  const UserBill *t_pUserBill;

	  

	  t_userBill.m_userId = m_globalInfo->m_userId;

	  t_userBill.m_billId = m_attrId;

		if (m_globalInfo->getBillItem<UserBill>(

          m_globalInfo->m_userBills_new, t_userBill, t_pUserBill) )

    {

      unsigned int bitValue = (unsigned int)t_pUserBill->m_value;

      intValue =  (long) m_globalInfo->m_comBill.getCount(bitValue);

    }

    else

      intValue = 0L;

	}
	else if(m_attrSource == ATTR_SOURCE_FACT_BUTBFEE)//获取账目实际费用，但不包括调整补收的费用
	{
		m_globalInfo->m_detailBillMgr.getFactFeeButBfee(m_attrId,m_itemType,intValue);
	}
	else if(m_attrSource == ATTR_SOURCE_BILLUSER)//从bill_user_x获取费用，主要是用于参考不下账的费用
	{	
	  UserBill t_userBill;
	  const UserBill *t_pUserBill;
	  
	  if (m_globalInfo->m_bizType==BIZ_TYPE_GRPBILL)//虚拟用户参考群下成员
	  {
	  	intValue = 0L;
	  	for (set<T_SERVER_ID>::iterator t_itr= m_globalInfo->m_comBill.m_userIdSet.begin();
	  		   t_itr!= m_globalInfo->m_comBill.m_userIdSet.end();t_itr++)
	   {	  	
	    t_userBill.m_userId = *t_itr;
	  	t_userBill.m_billId = m_attrId;

			if (m_globalInfo->getBillItem<UserBill>(
          m_globalInfo->m_userBills_new, t_userBill, t_pUserBill) )

    	{
      	intValue += (unsigned int)t_pUserBill->m_value; //累加群下所有用户
    	}
     }
	  	
	  } 	
	  else	
	  {
	  t_userBill.m_userId = m_globalInfo->m_userId;
	  t_userBill.m_billId = m_attrId;

		if (m_globalInfo->getBillItem<UserBill>(
          m_globalInfo->m_userBills_new, t_userBill, t_pUserBill) )

    {
      intValue = (unsigned int)t_pUserBill->m_value;
    }
    else
      intValue = 0L;
    }

	}
	else if(m_attrSource == ATTR_SOURCE_ORG_FACT)//求优惠前与优惠后费用和add by xuzy 2011.2.7
	{	
		long intValueOrg ,intValueFact;

		//求取原始费用
		m_globalInfo->m_detailBillMgr.getOriginFee(m_attrId, m_itemType,intValueOrg);
		//求取实际费用
		m_globalInfo->m_detailBillMgr.getFactFee(m_attrId,intValueFact);
		
		intValue = intValueOrg+ intValueFact;
	}
	
  //added by wanglu 2011-2-24 begin
	else if(m_attrSource == ATTR_SOURCE_ACCTDEPOSIT)//来源于用户存折金额
	{
		vector<AcctDeposit> t_acctDepositVector;
		t_acctDepositVector.clear();
		
		//宽带账户归属于用户的默认账户下
		//根据用户ID获取其默认账户     
		T_ACCT_ID t_acctId = "";
		(m_globalInfo->m_comBill).getUserPayInfo(t_acctId,m_globalInfo->m_userId);
		
		if(t_acctId == "" || m_globalInfo->m_billMgr->getUserAcctDeposit(t_acctId,t_acctDepositVector)==false)
			return;
			
		for(vector<AcctDeposit>::iterator t_itr=t_acctDepositVector.begin();t_itr!= t_acctDepositVector.end();++t_itr)
		{
			intValue = 0L;
			#ifdef _DEBUG_
			cout<<"acct_balance_id="<<t_itr->m_acctBalanceId<<'\t'
			    <<"acct_id="<<t_itr->m_acctId<<'\t'
			    <<"user_id="<<t_itr->m_userId<<'\t'
			    <<"deposit_code="<<t_itr->m_depositCode<<'\t'
			    <<"odd_money="<<t_itr->m_oddMoney<<'\t'
			    <<"even_money="<<t_itr->m_evenMoney<<'\t'
			    <<"limit_money="<<t_itr->m_limitMoney<<'\t'
			    <<"limit_mode="<<t_itr->m_limitMode<<'\t'
			    <<"start_cycle_id="<<t_itr->m_startCycleId<<'\t'
			    <<"end_cycle_id="<<t_itr->m_endCycleId<<'\t'
			    <<"valid_tag="<<t_itr->m_validTag<<endl;                        
      #endif  
      
      if(m_globalInfo->m_paramMgr->m_sysparamDepositCodeVec.empty()) 
      	break;
      
      
      for(vector<string>::iterator t_it=(m_globalInfo->m_paramMgr->m_sysparamDepositCodeVec).begin();
      	  t_it!=(m_globalInfo->m_paramMgr->m_sysparamDepositCodeVec).end();++t_it)
      {
      	char r_depositCode[20];
      	memset(r_depositCode,0,sizeof(r_depositCode));
      	sprintf(r_depositCode,"%d",t_itr->m_depositCode);     	
      	if(*t_it == r_depositCode)
      	{
          intValue += (unsigned int)t_itr->m_oddMoney;
          intValue += (unsigned int)t_itr->m_evenMoney;      		
      	} 
      }
    }
  }
  //end 2011-2-25
  	
	else
	{
		THROW_C_P2(BusinessException, -1, "帐单对象错误,未定义的属性值来源："
                    "object_id=%d,m_attrSource=%c", m_id, m_attrSource);
	}	
}

//获取群下其他角色用户的费用值
void ObjectBill::getRoleBillValue(T_SERVER_ID &groupId, long &value,ID_TYPE r_idType)
{
	if (groupId != NULL_VPN_ID)
	{
		AcctBillMgr t_acctBillMgr;
		t_acctBillMgr.initialize(m_globalInfo);
		
		DetailBill *RoleBill = t_acctBillMgr.getRoleBill(groupId,m_roleCode,r_idType);
		if (RoleBill != NULL)
		{
			if (m_attrSource==ATTR_SOURCE_FACT)//获取账目实际费用
			{
				m_globalInfo->m_detailBillMgr.getFactFee(*RoleBill,m_attrId,value);
			}
			else if(m_attrSource==ATTR_SOURCE_ORIGIN)//获取账目原始费用
			{
				m_globalInfo->m_detailBillMgr.getOriginFee(*RoleBill,m_attrId,value);
			}
			else if(m_attrSource == ATTR_SOURCE_ITEM_NUM)//明细帐目数量
			{
				m_globalInfo->m_detailBillMgr.getItemNum(*RoleBill,m_attrId,value);
			}
			else if (m_attrSource==ATTR_SOURCE_CUR)//参考上条资费作用后的值 add by xuf 2010-2-1 1:52:28
			{
			 	m_globalInfo->m_detailBillMgr.getCurFee(*RoleBill,m_attrId,value);
			}
			else
			{
				THROW_C_P2(BusinessException, -1, "帐单对象错误,关联群账单不支持此费用源!"
            	"object_id=%d,attr_source=%c", m_id, m_attrSource);
			}
		}
		else
		{
			value = 0;
   		}
	}
	else
	{
		THROW_C_P1(BusinessException, -1, "帐单对象错误,关联群ID为空："
            "object_id=%d", m_id);
	}	
}

void ObjectBill::getFactValue(long &value)
{
	m_globalInfo->m_detailBillMgr.getFactFee(m_attrId,value);
}

//按“用户平摊+账目平摊”方式处理优惠值
long ObjectBill::update_userAveItemAve(long &effectFee,
                                       const char &discountType, 
                                       const char &ifNotShowNegative, 
                                       const char &ifNegativeBill)

{
	if (ifNegativeBill == NEGATIVE_BILL_YES)
	{
		long factFee = 0;
		m_globalInfo->m_detailBillMgr.getFactFee(factFee);
		if (factFee>=0 && effectFee>factFee)
		{
			effectFee = factFee;
		}
	}
	return m_globalInfo->m_detailBillMgr.update_userAveItemAve(m_attrId, effectFee,  
                          ifNotShowNegative, discountType, ifNegativeBill);
	
} 

//add by cuiyw for shanxi begin at 2009-12-12 18:24:06
/*作用:优惠不参考实际费用是否小于优惠费用*/
long ObjectBill::update_userButNEGative(long &effectFee,
                                       const char &discountType, 
                                       const char &ifNotShowNegative, 
                                       const char &ifNegativeBill)

{

	return m_globalInfo->m_detailBillMgr.update_userAveItemAve(m_attrId, effectFee,  
                          ifNotShowNegative, discountType, ifNegativeBill);
	
} 
//add bu ucuiyw for shanxi end
                         
//按“用户平摊+账目优先”方式处理优惠值
long ObjectBill::update_userAveItemPrior(long &effectFee,
                                         const char &discountType,
                                         const char &ifNotShowNegative,
                                         const char &ifNegativeBill)
{ 
	if (ifNegativeBill == NEGATIVE_BILL_YES)
	{
		long factFee = 0;
		m_globalInfo->m_detailBillMgr.getFactFee(factFee);
		if (factFee>=0 && effectFee>factFee)
		{
			effectFee = factFee;
		}
	}
	
	return m_globalInfo->m_detailBillMgr.update_userAveItemPrior(m_attrId, effectFee,  
                            ifNotShowNegative, discountType, ifNegativeBill);
} 
                                                             
//按“用户优先+账目平摊”方式处理优惠值
long ObjectBill::update_userPriorItemAve(long &effectFee,
                                         const char &discountType,
                                         const char &ifNotShowNegative,
                                         const char &ifNegativeBill)
{
	if (ifNegativeBill == NEGATIVE_BILL_YES)
	{
		long factFee = 0;
		m_globalInfo->m_detailBillMgr.getFactFee(factFee);
		if (factFee>=0 && effectFee>factFee)
		{
			effectFee = factFee;
		}
	}
	
	return m_globalInfo->m_detailBillMgr.update_userPriorItemAve(m_attrId, effectFee, 
                            ifNotShowNegative, discountType, ifNegativeBill);                                   
}

//按“用户优先+账目优先”方式处理优惠值
long ObjectBill::update_userPriorItemPrior(long &effectFee, 
                                           const char &discountType,
                                           const char &ifNotShowNegative,
                                           const char &ifNegativeBill)
{
	if (ifNegativeBill == NEGATIVE_BILL_YES)
	{
		long factFee = 0;
		m_globalInfo->m_detailBillMgr.getFactFee(factFee);
		if (factFee>=0 && effectFee>factFee)
		{
			effectFee = factFee;
		}
	}
	
	return m_globalInfo->m_detailBillMgr.update_userPriorItemPrior(m_attrId,effectFee, 
                              ifNotShowNegative, discountType, ifNegativeBill);                                            
}   

//按用户优先级最高的作用                                   
long ObjectBill::update_fisrtUser(long &effectFee,
                                  const char &discountType,
                                  const char &ifNotShowNegative,
                                  const char &ifNegativeBill)
{
	if (ifNegativeBill == NEGATIVE_BILL_YES)
	{
		long factFee = 0;
		m_globalInfo->m_detailBillMgr.getFactFee(factFee);
		//modify by cuiyw for shanxi begin at 18:32:00 
      //#ifdef __PROV019__  //如果是山西
	  	//#else
		if (factFee>=0 && effectFee>factFee)
		{
			effectFee = factFee;
		}
      //#endif 
   //modify by cuiyw for shanxi end 
	}
	
	return m_globalInfo->m_detailBillMgr.update_fisrtUser(m_attrId, effectFee, 
                     ifNotShowNegative, discountType, ifNegativeBill);
}

//指定用户作用                                   
long ObjectBill::update_setUser(const T_USER_ID &userId,
																	long &effectFee,
                                  const char &discountType,
                                  const char &ifNotShowNegative,
                                  const char &ifNegativeBill)
{
	if (userId=="") 
		THROW_C_P1(BusinessException, -1, "更新的用户错误："
                    "userId=%d", userId.value());

//移到付费关系绑定，避免用户优惠后来不作用，或者0和6共存，6优先级高，0会分摊到虚拟用户上 2009-12-2 16:13:06                      
//	if(m_globalInfo->m_comBill.m_userIdSet.find(userId)==m_globalInfo->m_comBill.m_userIdSet.end())
//			m_globalInfo->m_comBill.m_userIdSet.insert(userId);
		
	if (ifNegativeBill == NEGATIVE_BILL_YES)
	{
		long factFee = 0;
		m_globalInfo->m_detailBillMgr.getFactFee(factFee);
		//modify by cuiyw for shanxi begin at 18:32:00 
    //#ifdef __PROV019__  //如果是山西           
	  //#else
		if (factFee>=0 && effectFee>factFee)
		{
			effectFee = factFee;
		}
    //#endif 
   //modify by cuiyw for shanxi end 
	}
	
	return m_globalInfo->m_detailBillMgr.update_setUser(userId, m_attrId, effectFee, 
                     ifNotShowNegative, discountType, ifNegativeBill);
}


void ObjectBill::updatePresentMoney(const long &insObjId, const long &value)
{	
	m_globalInfo->m_detailBillMgr.updatePresentMoney(m_attrId,insObjId,value);
	return; 
}

//更新当前费用
void ObjectBill::updateCurFee()
{
	m_globalInfo->m_detailBillMgr.updateCurFee();
}

//分割枚举
void ObjectBill::splitEnumValue(const char* pEnumValue, 
							  T_MEMBER_TYPE &value, 
							  vector<T_MEMBER_TYPE> &vValue,
							  map<T_MEMBER_TYPE, T_MEMBER_TYPE> &mapValue)
{
	char paramValue[MAX_STR_LENGTH+1];
	char * pValue;
	int iPos;
	T_MEMBER_TYPE firstValue(""),secondValue("");
	string strValue;
	strncpy(paramValue, pEnumValue, MAX_STR_LENGTH);
	paramValue[MAX_STR_LENGTH]='\0';	
	vValue.clear();
	mapValue.clear();
	pValue = strtok(paramValue, ",");	

	while (pValue != NULL)
	{	
		strValue = pValue;
		iPos = strValue.find_first_of("-");
		if(iPos==strValue.npos)
		{
			firstValue = pValue;
			vValue.push_back(firstValue);
		}
		else 
		{
			if (iPos>0 && iPos<strValue.length()-1)
			{
				firstValue = strValue.substr(0,iPos).c_str();
				secondValue = strValue.substr(iPos+1).c_str();
				mapValue.insert(make_pair(firstValue,secondValue));
			}
		}
		pValue = strtok(NULL, ",");
	}

	//vValue排序
	if(!vValue.empty())
	{ std::sort(vValue.begin(),vValue.end());}
}	

//根据角色获取群内指定用户集合
void ObjectBill::getRoleUsers(T_SERVER_ID  &groupId)
{
	//清空用户集合
	m_globalInfo->m_detailBillMgr.m_userIds.clear();
	map<T_SERVER_ID,map<T_USER_ID,T_MEMBERINFO_TYPE> >::iterator t_pos;
	if ((t_pos= m_globalInfo-> m_comBill.m_groupUserIdMap.find(groupId))
		== m_globalInfo-> m_comBill.m_groupUserIdMap.end())
		if(!m_globalInfo-> m_comBill.GetUserInfoByGrpId(groupId))
			return ;

	map<T_USER_ID,T_MEMBERINFO_TYPE> l_userTypeMap;	
	if (t_pos!= m_globalInfo-> m_comBill.m_groupUserIdMap.end())
	{
		l_userTypeMap = t_pos->second;		
	}
	else
		return ;
	for (map<T_USER_ID,T_MEMBERINFO_TYPE>::iterator
		itgu = l_userTypeMap.begin();
		itgu != l_userTypeMap.end();++itgu)
	{
		if(m_roleCode==ROLECODE_ENUM && m_globalInfo->m_comBill.judgeRoleCodeIn(m_roleCodeVector,
			m_roleCodeMap,itgu->second.m_memberRole))
		{ 
			m_globalInfo->m_detailBillMgr.m_userIds.insert(itgu->first);
			//m_userIdsUnion
			m_globalInfo->m_detailBillMgr.m_userIdsUnion.insert(itgu->first);
		}
	}  
}

//根据条件ID获取账户下的用户ID
void ObjectBill::getUsers(T_SERVER_ID &acctId)
{
	//清空用户集合
	m_globalInfo->m_detailBillMgr.m_userIds.clear();

	//保存出账用户信息
	IdInfo t_idInfo;
	t_idInfo  = m_globalInfo->m_idInfo;

	CondBase    *t_condRule=m_pRateInfo->m_paramMgr->getCondition(m_condId);
	if (t_condRule==NULL)
		return;

	//根据账户获取用户
	if (m_policyInfo->m_rateTpInfo->m_pUserTp->m_idType==(char)ACCT_ID_TYPE)
	{
		if(m_globalInfo->m_comBill.m_commonDealFlag==DEALTAG_LOADDETAILBILL_NOBILL)
		{
			map<T_SERVER_ID,int> t_userOrderMap;
			if (m_globalInfo->m_comBill.getUserDiscntOrderInAcct(acctId,
				t_userOrderMap)<=0) 
				return ;

			for (map<T_SERVER_ID,int>::iterator t_it = t_userOrderMap.begin();
				t_it!=t_userOrderMap.end();++t_it)
			{
				if (m_pRateInfo->m_paramMgr->getUserInfo(
					t_it->first,
					m_globalInfo->m_idInfo,
					m_globalInfo->m_comBill.m_cycParam)<=0)
				{       	 			
					continue;
				}

				if(t_condRule!=NULL)
				{
					if(t_condRule->execute())
					{
						m_globalInfo->m_detailBillMgr.m_userIds.insert(t_it->first);
					}
				}
			}
		}
		else 
		{
			vector<UserDetailBill>::iterator t_billIter;         //临时的用户明细账单指针
			for (t_billIter=m_globalInfo->m_comBill.m_detailBill.m_detailBillVector.begin();
				t_billIter!=m_globalInfo->m_comBill.m_detailBill.m_detailBillVector.end(); 
				++t_billIter)
			{            
				if (m_pRateInfo->m_paramMgr->getUserInfo(
					t_billIter->m_subId,
					m_globalInfo->m_idInfo,
					m_globalInfo->m_comBill.m_cycParam)<=0)
				{       	 			
					continue;
				}
				
				if(t_condRule!=NULL)
				{
					if(t_condRule->execute())
					{
						m_globalInfo->m_detailBillMgr.m_userIds.insert(t_billIter->m_subId);
					}
				}
			}

		}

	}

	else
	{
		map<T_SERVER_ID,map<T_USER_ID,T_MEMBERINFO_TYPE> >::iterator t_pos;
		if ((t_pos=m_globalInfo->m_comBill.m_groupUserIdMap.find(acctId))
			==m_globalInfo->m_comBill.m_groupUserIdMap.end())
			if(!m_globalInfo->m_comBill.GetUserInfoByGrpId(acctId))
				return ;

		map<T_USER_ID,T_MEMBERINFO_TYPE> l_userTypeMap;	
		if (t_pos!=m_globalInfo->m_comBill.m_groupUserIdMap.end())
		{
			l_userTypeMap = t_pos->second;		
		}
		else
			return ;

		for (map<T_USER_ID,T_MEMBERINFO_TYPE>::iterator itgu = l_userTypeMap.begin();
			itgu != l_userTypeMap.end();++itgu)
		{
			if (m_pRateInfo->m_paramMgr->getUserInfo(
				itgu->first,
				m_globalInfo->m_idInfo,
				m_globalInfo->m_comBill.m_cycParam)<=0)
			{       	 			
				continue;
			}

			if(t_condRule!=NULL)
			{
				if(t_condRule->execute())
				{
					if(m_roleCode==ROLECODE_ENUMCONDID ) 
					{
							if ( m_globalInfo->m_comBill.judgeRoleCodeIn(m_roleCodeVector,m_roleCodeMap,itgu->second.m_memberRole))
						{ 
							m_globalInfo->m_detailBillMgr.m_userIds.insert(itgu->first);
							//m_userIdsUnion  用户集合，群付费关系处理过滤用
							m_globalInfo->m_detailBillMgr.m_userIdsUnion.insert(itgu->first);
						}
					}
					else
					{
						m_globalInfo->m_detailBillMgr.m_userIds.insert(itgu->first);
						//m_userIdsUnion  用户集合，群付费关系处理过滤用
						m_globalInfo->m_detailBillMgr.m_userIdsUnion.insert(itgu->first);
					}

				}
			}
		}
	}

	m_globalInfo->m_idInfo = t_idInfo;
}	

void ObjectBill::geteffectUsers()
{
	if(m_roleCode== ROLECODE_ENUM)
	{
		//根据角色获取群内指定用户集合
		getRoleUsers(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId);
	}
	else if (m_roleCode== ROLECODE_CONDID || m_roleCode== ROLECODE_ENUMCONDID)
	{
		//根据条件ID(或角色加条件)获取用户集合
		getUsers(m_policyInfo->m_rateTpInfo->m_pUserTp->m_userId);
	}
	else
	{
		return ;
	}
}


