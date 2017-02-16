#include "TpProc.h"

#include "RateError.h"
#include "PMManager.h"
#include "Mdb_VpnInfo_UserId.h"

INSTALL_PROCEDURE_INSTANCE(TpProc);

int TpProc::execute() 
{
   #ifdef FP_DEBUG
    FP_BEGIN(TpProc)
  #endif
  //0.错单,则直接跳过
	if ((*m_pcdr).m_errorCode != E_DEFAULT) 
	{
		#ifdef _DEBUG_
			cout << " errorCode : " << (*m_pcdr).m_errorCode << endl;
		#endif
		return 1;
	}
	//设置要求取基本政策对应的费用域
	m_pRateInfo->getFeeFields(m_pRateInfo->m_hasFeeFlag);
 	// 1.取基本政策
	if (m_pcdr[ATTRI_NEED_BASETP] != FLAG_NO )
	{
		//求基本费率政策,如果基本政策得到免费政策,也将免费标志置上
		if (!getBaseTpList())
		{
			return 1;
		}
	}

    //1.5 added by liugj 为了提前获取用户信息，可以，例如获取系统优惠时候，用于过滤非本地州的数据，原来在UserDiscntProc::init()中做这一步
    //取用户信息  
    if (strncmp(m_pcdr[ATTRI_FILE_NAME].c_str(),"TTD",3) != 0)
    {
        if(!(m_pRateConfig->m_channelType==CHANNEL_INTERPROV ||
           m_pRateConfig->m_channelType==CHANNEL_INTERNATIOANAL))
        {
            //Modified By Xuf 2009/9/28 23:00:00
            //if (!m_pRateInfo->m_comBill.m_ifExistUserInfo&&!m_pRateInfo->m_comBill.getUserInfo(m_pRateInfo->m_billUserId, m_pRateInfo->m_idInfo))
            if (!m_pRateInfo->m_comBill.m_ifExistUserInfo&&m_pmManager->getUserInfo(m_pRateInfo->m_billUserId, m_pRateInfo->m_idInfo,m_pRateInfo->m_cycParam)<=0)
            {    
                //无资料用户             
            	//theErrorMessages->insert(E_INITIALIZE,"Get user info error!! userId="+ m_pComBill->m_userId); 
            	//Added By Sunam 2009/8/15 10:48:15 取用户资料错，把错误编码置上
            //(*m_pcdr)[ATTRI_ERROR_CODE] = E_ACCT_GET_USERINFO;
            //(*m_pcdr).m_errorCode = E_ACCT_GET_USERINFO;
              	m_pRateInfo->m_comBill.setErrorCode(m_pRateInfo->m_billUserId,E_ACCT_GET_USERINFO,"0");
                DEBUG_PRINT("[%s:%d] Get user info error! userId=%s \n",__FILE__,__LINE__,m_pRateInfo->m_comBill.m_userId.c_str());   
                
               // return false;
            }
            else //获取到用户资料
            {
            	  m_pRateInfo->m_comBill.m_ifExistUserInfo= true;
                //如果是帐务事件类型，重置m_pcdr[ATTRI_HOME_CODE_A]字段，便于取系统优惠时候取到
                if((m_pRateInfo->m_bizType == BIZ_TYPE_SPMONTH)||     //如果是包月费事件
                	 (m_pRateInfo->m_bizType == BIZ_TYPE_MONFEE)||
                   (m_pRateInfo->m_bizType == BIZ_TYPE_BILL)||        //如果是优惠类型事件
                   (m_pRateInfo->m_bizType == BIZ_TYPE_GRPBILL)||        //如果是群优惠类型事件
                   (m_pRateInfo->m_bizType == BIZ_TYPE_ACCTBILL)||        //如果是账户优惠类型事件
                   (m_pRateInfo->m_bizType == BIZ_TYPE_ONETIMES)||    //如果是一次性费用事件
                   (m_pRateInfo->m_fileName.substr(0,3)=="RIP")||     //如果是增量固定费
                   (m_pRateInfo->m_fileName.substr(0,3)=="RAP")       //如果是全量量固定费
                   )
                {
                    if(m_pRateInfo->m_idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE].substr(0,2)=="00")
                    {
                        //遵从计费习惯，将0025、0020置为025、020
                        m_pcdr[ATTRI_HOME_CODE_A] = m_pRateInfo->m_idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE].substr(1,3);
                    }
                    else
                    {
                        m_pcdr[ATTRI_HOME_CODE_A] = m_pRateInfo->m_idInfo.m_idAttr[IdInfo::ID_EPARCHY_CODE];
                    }
                }
            }
        }
    }

  //2. 取基本优惠政策列表
	if (!getAllBaseTariff())
	{
		return 1;
	}
//#ifdef __PROV019__  //如果是山西	
//获取默认资费表数据.	
//add by cuiyw for shanxi At 2009-12-04 20:33 get Tp from td_b_default_feepolicy
    if (m_pRateInfo->m_bizType == BIZ_TYPE_BILL)
    {
    if(!getDefaultTariff())
    {
      return 1;
    }
    }
//#endif	
//end by cuiyw for shanxi 

	/*
	#ifdef _DEBUG_
	  cout<<"------基本政策列表 BEGIN--------------"<<endl;
	  for(vector<T_TP_ID>::iterator t_bTpItr=m_baseTpList.begin();
	      t_bTpItr!=m_baseTpList.end();++t_bTpItr)
    {
      cout<<"\t基本政策："<<*t_bTpItr<<" "<<__FILE__<<__LINE__<<endl;
    }
	  cout<<"------基本政策列表 end--------------"<<endl;
	#endif
  */
  //3. 设置是否需要求取用户优惠政策标记
	if(isNoNeedTP()==true)
	{
		m_pcdr[ATTRI_NEED_USERTP] = FLAG_NO;
	}
   #ifdef FP_DEBUG
    FP_BEGIN(getUserTpList)
  #endif
	// 4.取用户优惠政策(包括了用户出帐等政策)
	if (!getUserTpList())
	{
		return 1;
	}	
   #ifdef FP_DEBUG
    FP_END(getUserTpList)
  #endif
  // 5. 取参数信息、参数定义，形成GlobalInfo中的m_userTpList
  if(setTpParamInfo()==false)
  {
    return 1;
  }
  
	#ifdef _DEBUG_
	cout<<"-----------用户套餐政策----------------"<<endl;
	for(vector<UserTp>::const_iterator t_userTpItr=m_pRateInfo->m_userTpList.begin();
	    t_userTpItr!=m_pRateInfo->m_userTpList.end();++t_userTpItr)
  {
    cout<<*t_userTpItr;
  }
  cout<<"-------------------------------------"<<endl;
	#endif
  	
  // 6. 调用GlobalInfo中的方法，将政策参数定义转为批价内部，
  //    并将政策分为计费政策和帐务优惠
  if(m_pRateInfo->getRateTpInfo()==false)
  {
    return 1;
  }
  #ifdef _DEBUG_
    m_pRateInfo->dumpRateTpInfo();
  #endif
   #ifdef FP_DEBUG
    FP_END(TpProc)
  #endif
	return 0;
}

int TpProc::reset()
{
	m_baseTpList.clear();
  for(int i = 0; i < FEEFIELDS_MAX; ++i )
	{
		m_hasTp[i] = false;
	}
	return 0;
}

//得到基本政策,置上免费标志
bool TpProc::getBaseTpList()
{
  //1.设置需要求取的基本政策对应的费用项
  //2.求取对应的基本政策
	T_TP_ID      t_tpId,t_tpId1;
	vector<int> &t_baseTpFacts=m_pRateInfo->m_processRule->m_baseTpFactorIds;
	
	//设置要求取基本政策对应的费用域
	m_pRateInfo->getFeeFields(m_pRateInfo->m_hasFeeFlag);

	int i,t_feeField;
	for(i=1;i<FEEFIELDS_MAX;++i)
	{
	  t_feeField = i;
	  if(m_pRateInfo->m_hasFeeFlag[t_feeField]==false)
	  {
	    m_pcdr[ATTRI_FREE_FEE1+t_feeField-1] = FLAG_YES;
	    //该费用项无需求取对应的政策
	    continue;
	  }
	  // 国际漫游来访基本政策先查TD_INTER_ROAMIN_BASETP表
	  if (m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_IN)
	  {
			char t_sameHomeAb;
			t_sameHomeAb = (m_pcdr[ATTRI_HOME_CODE_A] == m_pcdr[ATTRI_HOME_CODE_B])?'1':'0';
			if (m_pmManager->getInterRoamInBaseTp(t_feeField,
          							m_pRateInfo->m_beginDateTime,
          							m_pcdr[ATTRI_INTER_ASPGROUP],
          							m_pcdr[ATTRI_BIZ_TYPE],
          							t_sameHomeAb,
          							m_pcdr[ATTRI_HOME_CODE_B],
          							m_pcdr[ATTRI_CALL_TYPE],
          							m_pcdr[ATTRI_USER_ASP_B],
          							m_pcdr[ATTRI_USER_SERVICE_TYPE_B],
          							m_pcdr[ATTRI_CARRIER_TYPE],
          							m_pcdr[ATTRI_LONG_TYPE1],
          							m_pcdr[ATTRI_LONG_GROUP1],
          							m_pcdr[ATTRI_LONG_TYPE2],
          							m_pcdr[ATTRI_LONG_GROUP2],
          							t_tpId))
			{
				if (t_tpId != 0)
				{
					m_baseTpList.push_back(t_tpId);
					m_hasTp[t_feeField] = true;
					continue;
				}
			}	  
	  }

	  //add by zhaoly for 3g-partyprice 2009-5-6 17:23:41
    if (m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_UNICOMSP&&
        m_pcdr[ATTRI_SOURCE_TYPE]=="4D"&&
        m_pcdr[ATTRI_SP_SRVTYPE]!="70")//VAC话单 70为炫铃
    {
        for (vector<ParsedPartyPrice>::iterator priceItr=m_pRateInfo->m_parsedPartyPriceInfo.begin();
                 priceItr!=m_pRateInfo->m_parsedPartyPriceInfo.end();++priceItr)
        {
        	m_pcdr[ATTRI_BILLING_MODECODE]=priceItr->m_billingModeCode;
        	if(m_pcdr[ATTRI_BILLING_MODECODE].in("Free","OrderMonth","OrderDay","PerTime"))
          {
             //只支持资费,优惠通过套餐走
             //m_pcdr[ATTRI_3GFEE_TYPE]=m_pRateInfo->m_parsedPartyPriceInfo[0].m_prodPriceElement.find("feeType")->second;
             //m_pcdr[ATTRI_BILLING_MODECODE]=m_pRateInfo->m_parsedPartyPriceInfo[0].m_billingModeCode;
             if (priceItr->m_prodPriceElement.find("feetype") != priceItr->m_prodPriceElement.end())
             {
               m_pcdr[ATTRI_3GFEE_TYPE]=priceItr->m_prodPriceElement.find("feetype")->second;             
             }
             else
             {
	             m_pcdr[ATTRI_3GFEE_TYPE]="";
             }
             #ifdef _DEBUG_
             cout << "m_pcdr[ATTRI_3GFEE_TYPE] : " << m_pcdr[ATTRI_3GFEE_TYPE] << endl;
             cout << "m_pcdr[ATTRI_BILLING_MODECODE] : " << m_pcdr[ATTRI_BILLING_MODECODE] << endl;
             #endif
          }
        }
    }
    //add end
 #ifdef FP_DEBUG
    FP_BEGIN(getFeeBaseTp)
  #endif
        //Add by yangb For sx REQ12 At 2009/12/31 17:34:59
        if (t_feeField == 1)//市话
            m_pcdr[ATTRI_RECORD_TYPE]=m_pcdr[ATTRI_BRECORD_TYPE];
        if (t_feeField == 3)//长话
            m_pcdr[ATTRI_RECORD_TYPE]=m_pcdr[ATTRI_LRECORD_TYPE];
        if (t_feeField == 5)//信息费(和长话共用一个话单类型字段，并且不重复)
            m_pcdr[ATTRI_RECORD_TYPE]=m_pcdr[ATTRI_LRECORD_TYPE];


	  if(m_pRateInfo->m_paramMgr->getFeeBaseTp(t_feeField,m_pRateInfo->m_bizType,
	                                           t_baseTpFacts,m_pcdr,
	                                           m_pRateInfo->m_beginDateTime,
	                                           t_tpId)==true)
	  {
			m_baseTpList.push_back(t_tpId);
			#ifdef _DEBUG_
				cout << " 费率作用域 " << i<< " 基本政策: " << t_tpId << endl; 
			#endif
	    if(t_tpId == FREE_FEE_TPID[t_feeField])
			{
				m_pcdr[ATTRI_FREE_FEE1+t_feeField-1] = FLAG_YES;
			}
			m_hasTp[t_feeField] = true;
	      }
	      else
	  {//无对应的基本政策认为是免费
	    m_pcdr[ATTRI_FREE_FEE1+t_feeField-1] = FLAG_YES;
	  }
 #ifdef FP_DEBUG
    FP_END(getFeeBaseTp)
  #endif
	}
	setFreeFlag();
	if (m_pcdr[ATTRI_FREE_FEE] == FLAG_YES)
	{
		m_baseTpList.clear();
		for(i=1;i<FEEFIELDS_MAX;++i)
	  {
	    if(m_hasTp[i])
	    {
			  t_tpId = FREE_FEE_TPID[i];
			  m_baseTpList.push_back(t_tpId);
	    }
	  }
	}
	return true;
}

//置免费话单标志,可根据实际情况扩充
void TpProc::setFreeFlag()
{
  if ((m_pcdr[ATTRI_FREE_FEE1] == FLAG_YES)&&
    	(m_pcdr[ATTRI_FREE_FEE2] == FLAG_YES)&&
    	(m_pcdr[ATTRI_FREE_FEE3] == FLAG_YES)&&
    	(m_pcdr[ATTRI_FREE_FEE4] == FLAG_YES)&&
    	(m_pcdr[ATTRI_FREE_FEE5] == FLAG_YES))
    {
        m_pcdr[ATTRI_FREE_FEE] = FLAG_YES;
        return ;
    }
    
    //以下免费话单校验规则只针对语音话单
    if (m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GSM)
    {
      //移动采用部分硬编码方式
      if(m_pRateConfig->m_operatorType == OPERATOR_TYPE_CMCC)
      { 
    	  setGsmFreeFlag();
    	  return ;
    	}
    	setGsmFreeFlag();
    }
    //Added by Sunam 2009/10/27 14:35:27
    else if (m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FIXVOICE)
    {
        setFixVFreeFlag();
    }
     
    return;
}

//Added By Sunam 2009/10/27 14:33:51
void TpProc::setFixVFreeFlag()
{
    //如果LocalizeGsmFreeFlagProc返回1的话，不再往下处理，返回0继续往下走
    if (ProcedureHelper::ExecuteProcedure("LocalizeFixVFreeFlagProc") == 1)
    {
    	return ;
    }
    
    return;
}

void TpProc::setGsmFreeFlag()
{
	//本地的免费标志处理
	//如果LocalizeGsmFreeFlagProc返回1的话，不再往下处理，返回0继续往下走
  if (ProcedureHelper::ExecuteProcedure("LocalizeGsmFreeFlagProc") == 1)
  {
  	return ;
  }
  
	//三秒业务规则  中移市[2002]154号
  // 
  // 1. 拨打固定,移动或其它接入号码,发生长途通话,对3秒(包含3秒)长途话单免费
  // 2. 使用一次拨号IP电话(17951呼叫本地除外),或其他运行商一次拨号,,对3秒(包含3秒)长途话单免费
  // 3. 使用二次拨号IP(卡式IP),不进行3秒检查
  // 4. 使用铁通068,不进行3秒检查
  // 5. 呼转长途对3秒免费
  // 6. 所有被叫话单长途进行3秒检查
  // 7. 对国际漫游来访(除二次拨号IP和手机发短信)进行3秒检查
  // 8. 所有用户利用中国移动长途或IP电话网拨打电路长途电话或IP长途电话时,对3秒长途不计长途费
  // 9. 上述情况返回错单代码F178
  // 
  // 2008年 漫游资费调整了
  //对客户漫游时3秒以内的话单正常计费，本地长途3秒以内话单继续执行原优惠规则
  
  
  //一次拨号长途检查3秒政策
  //只有本运行商一次拨号 才区分long_type,其它IP不管有没有实际发生长途，都按三秒原则处理(免费) 
  if ((m_pcdr[ATTRI_DURATION] < DURATION_FREE) && 
      (m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_MOC))
  {
    //省内漫游、国内出访时不做3秒优惠
    //国际长途时3秒优惠
    if(!(m_pcdr[ATTRI_ROAM_TYPE]  == ROAM_TYPE_NATIONAL_OUT ||
         m_pcdr[ATTRI_ROAM_TYPE]  == ROAM_TYPE_PRO_IN         )||
         m_pcdr[ATTRI_LONG_TYPE1] == LONG_TYPE_INTERNATIONAL)
    {
    //本运行商非IP一次接入号,本运行商IP一次接入号
		if ((m_pcdr[ATTRI_DIAL_TYPE] == DIAL_TYPE_NIP1) || 
		    (m_pcdr[ATTRI_DIAL_TYPE] == DIAL_TYPE_IP1 ))
		{
			//检查是否有长途
    	if(m_pcdr[ATTRI_LONG_TYPE1] != LONG_TYPE_NONE)
    	{
    	  m_pcdr[ATTRI_FREE_FEE] = FLAG_YES;
        return ;
    	}
		}else if ((m_pcdr[ATTRI_DIAL_TYPE] == DIAL_TYPE_ONIP1) || 
		         (m_pcdr[ATTRI_DIAL_TYPE] == DIAL_TYPE_OIP1 ))
		{
			m_pcdr[ATTRI_FREE_FEE] = FLAG_YES;
      return ;
		}
  }    
  }

  // 长途3s免,如果二次拨号IP或者068,则无长途费,
  // m_isFreeLfee 为 true ,因此不会进行3秒检查
  if (((m_pcdr[ATTRI_LONG_TYPE1] != LONG_TYPE_NONE) || 
    	 (m_pcdr[ATTRI_LONG_TYPE2] > LONG_TYPE_NONE)) &&
    	 ((m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_NONE) || (m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_NATIONAL_IN))&&
    	(m_pcdr[ATTRI_DURATION] < DURATION_FREE) &&
    	((m_pcdr[ATTRI_FREE_FEE3] == FLAG_NO) || 
    	 (m_pcdr[ATTRI_FREE_FEE5] == FLAG_NO)))
  {
    m_pcdr[ATTRI_FREE_FEE] = FLAG_YES;
    return ;
  }

  //国际来访或者日韩来访小于3s免费
  if (((m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_IN) || 
       (m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == SERVICE_TYPE_KOREA))
      && (m_pcdr[ATTRI_DURATION] < DURATION_FREE))
  {
    if ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMMO)
      ||(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMMT))
    {//不包含短信
    }else if(((m_pcdr[ATTRI_DIAL_TYPE] == DIAL_TYPE_IP2)||
             (m_pcdr[ATTRI_DIAL_TYPE] == DIAL_TYPE_OIP2))
    		&&(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MOC))
    {//本运行商IP二次拔号,其它运营商IP二次拔号
     //不进行3秒判断
    }else //主被叫3秒全免
    {
      m_pcdr[ATTRI_FREE_FEE] = FLAG_YES;
      return ;
    }
  } 
  
  return ;
}
bool TpProc::getAllBaseTariff()
{
  //来访用户，不取基本优惠政策
  if(m_pRateConfig->m_channelType==CHANNEL_INTERPROV ||
     m_pRateConfig->m_channelType==CHANNEL_INTERNATIOANAL)
  {
    return true;
  } 
	// 1. 取面向所有业务都生效的政策 TD_BASE_TARIFF
	vector<T_TP_ID> t_tmpbaseTp;
	vector<T_TP_ID>::iterator t_tmpitr;
	bool     t_bizFlag=(m_pcdr[ATTRI_NEED_BASETP] != FLAG_NO &&
	                    m_pcdr[ATTRI_FREE_FEE]    != FLAG_YES );
	if ( m_pmManager->getBaseTp(t_bizFlag,
	                            m_pRateInfo->m_bizType,
	                            m_pRateInfo->m_beginDateTime,
              								m_pcdr[ATTRI_HOME_CODE_A] ,
              								m_pcdr[ATTRI_CALL_TYPE] ,
              								m_pcdr[ATTRI_ROAM_TYPE] ,
              								m_pcdr[ATTRI_USER_SERVICE_TYPE_B] ,
              								m_pcdr[ATTRI_CARRIER_TYPE] ,
              								m_pRateInfo->m_cycParam.m_endTime,
              								m_pcdr,
              								t_tmpbaseTp) )
	{
		for(t_tmpitr=t_tmpbaseTp.begin();t_tmpitr!=t_tmpbaseTp.end();t_tmpitr++)
		{
			this->m_baseTpList.push_back(*t_tmpitr);
		}
	}
	//2. 取基站通优惠政策
	getBaseCellTp();
	
	return true;
}

bool TpProc::getUserTpList()
{
  char t_errMsg[128];
  bool t_flag = true;
	if (m_pRateInfo->m_userId == INVALID_SERVER_ID ||
	    m_pRateInfo->m_userId.value() == 0)
	{
	  return true;
	}
	#ifdef FP_DEBUG
    FP_BEGIN(m_pmManagergetUserTp)
  #endif
  
	//0. 获取计费和对方VPN关系信息 
	int t_ret=0;
	T_SERVER_ID t_lastVpnId;
	//只对GSM，对方号码是固话(0非00开头)的时候循环处理
	bool t_other_needLoop =((m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_GSM)&&
							(strncmp(m_pRateInfo->m_otherParty.c_str(),"0",1)==0) &&
							(strncmp(m_pRateInfo->m_otherParty.c_str(),"00",2)!=0));
	t_ret = m_pmManager->getVpnInfoByUserId(m_pRateInfo->m_userId,VPN_MEMIDTYPE_USER,
									m_pRateInfo->m_beginDateTime,VPN_MEMTYPE_NULL,
									t_lastVpnId,m_pRateInfo->m_vpnInfoList_a);
	if(t_ret < 0)  m_pRateInfo->setErrInfo(E_GET_USER_TP,t_errMsg);
	t_ret = m_pmManager->getVpnInfoByNumber(m_pRateInfo->m_otherParty,VPN_MEMIDTYPE_USER,
									m_pRateInfo->m_beginDateTime,VPN_MEMTYPE_NULL,
									m_pRateInfo->m_vpnInfoList_b,t_other_needLoop);
	if(t_ret < 0)  m_pRateInfo->setErrInfo(E_GET_USER_TP,t_errMsg);
	
	
	for (list<Mdb_VpnInfo_UserId>::iterator t_vpnInfoItr = m_pRateInfo->m_vpnInfoList_a.begin();
                         t_vpnInfoItr != m_pRateInfo->m_vpnInfoList_a.end();
                             ++t_vpnInfoItr)
    {
        for (list<Mdb_VpnInfo_UserId>::iterator t_vpnInfoItr1 = m_pRateInfo->m_vpnInfoList_b.begin();
                         t_vpnInfoItr1 != m_pRateInfo->m_vpnInfoList_b.end();
                             ++t_vpnInfoItr1)
        {
            /*VPN内通话角色判断  20100315  判断主被叫都一致 限制了 4001=4005的情况
            if (t_vpnInfoItr->m_record.m_userId==t_vpnInfoItr1->m_record.m_userId)
                if (t_vpnInfoItr->m_record.m_lastMemberType==t_vpnInfoItr1->m_record.m_lastMemberType&&
                    t_vpnInfoItr->m_record.m_lastMemberType.substr(0,2)=="40")//ivpn
                    m_pcdr[ATTRI_VPN_FLAG]=FLAG_YES;
            */
            
            if (t_vpnInfoItr->m_record.m_userId==t_vpnInfoItr1->m_record.m_userId)
                if ( (t_vpnInfoItr1->m_record.m_lastMemberType.substr(0,2)=="40")&&  
                     (t_vpnInfoItr->m_record.m_lastMemberType.substr(0,2)=="40" ) )//判断对端和付费端vpn角色限制                    
                    m_pcdr[ATTRI_VPN_FLAG]=FLAG_YES;

        }
    }
  
  //1. 取普通用户政策
  if(m_pmManager->getUserTp(m_pRateInfo->m_userId,
                            m_pRateInfo->m_otherParty,
                            m_pRateInfo->m_beginDateTime,
                            m_pRateInfo->m_userTpList,
                            t_flag)<0)
	{
	  #ifdef _DEBUG_
	    cout<<"getUserTp false!userId="<<m_pRateInfo->m_userId
	        <<" "<<__FILE__<<__LINE__<<endl;
	  #endif
	  return false;
	}
	if(t_flag == false)
	{
    #ifdef _DEBUG_
      cout<<"小区参数没有实例化!"<<__FILE__<<__LINE__<<endl;
    #endif
    sprintf(t_errMsg,"资费参数没有实例化!");
    m_pRateInfo->setErrInfo(E_USER_TPPARAM,t_errMsg);
	}
 #ifdef FP_DEBUG
    FP_END(m_pmManagergetUserTp)
  #endif
	//return true;
	//2. 取群用户政策
		#ifdef FP_DEBUG
    FP_BEGIN(m_pmManagergetUserVpnTp)
  #endif
	if(m_pmManager->getUserVpnTp(m_pRateInfo->m_userId,
                               m_pRateInfo->m_otherParty,
                               m_pRateInfo->m_beginDateTime,
                               m_pRateInfo->m_userTpList)<0)
  {
	  #ifdef _DEBUG_
	    cout<<"getUserVpnTp false!userId="<<m_pRateInfo->m_userId
	        <<" "<<__FILE__<<__LINE__<<endl;
	  #endif
    #ifdef _DEBUG_
      cout<<"取用户套餐资料失败,数据库或对象异常!"<<__FILE__<<__LINE__<<endl;
    #endif
    sprintf(t_errMsg,"取用户套餐资料失败,数据库或对象异常!");
    m_pRateInfo->setErrInfo(E_GET_USER_TP,t_errMsg);
    return false;
  }
   #ifdef FP_DEBUG
    FP_END(m_pmManagergetUserVpnTp)
  #endif
#ifdef FP_DEBUG
    FP_BEGIN(getusertpnew_add)
  #endif
//--begin modified by chenyong 2009/11/1  
//CondTp在getUserTp里面统一获取。
/*
    //3.取用户动态优惠  wangzf
    m_pmManager->getUserCondTp(m_pRateInfo->m_userId,
                            m_pRateInfo->m_otherParty,
                            m_pRateInfo->m_beginDateTime,
                            m_pRateInfo->m_userTpList
                            );  
*/
//--end modified by chenyong 2009/11/1  

  //Added By Sunam 2009/11/27 19:03:00
  if(m_pcdr[ATTRI_NEED_USERTP] != FLAG_NO )
  {
    //Added By Sunam 2009/8/24 10:30:42
    //list<Mdb_VpnInfo_UserId> t_bVpnInfoList;
    vector<UserTp>::iterator t_itr;
    vector<T_TP_ID> t_vpnRelTpList;
    vector<T_SERVER_ID> t_bVpnIdList;
    UserTp t_userTp;
    vector<UserTp> t_vpnRelTp;
    for (t_itr = m_pRateInfo->m_userTpList.begin(); t_itr != m_pRateInfo->m_userTpList.end(); ++t_itr)
    {
        if ((t_itr->m_relationVpnId != NULL_VPN_ID) || (t_itr->m_subTpType == TP_TYPE_VPN))
        {
            if (m_pmManager->getVpnRelFeePolicy(t_itr->m_relationVpnId,m_pRateInfo->m_beginDateTime,t_bVpnIdList,t_vpnRelTpList))
            {
                    for (int j=0; j < t_bVpnIdList.size(); ++j)
                    {
                        t_flag = false;
                    //for (list<Mdb_VpnInfo_UserId>::iterator t_vpnInfoItr = t_bVpnInfoList.begin();
                    //     t_vpnInfoItr != t_bVpnInfoList.end();
                    //     ++t_vpnInfoItr)
                   	for (list<Mdb_VpnInfo_UserId>::iterator t_vpnInfoItr = m_pRateInfo->m_vpnInfoList_b.begin();
                         t_vpnInfoItr != m_pRateInfo->m_vpnInfoList_b.end();
                             ++t_vpnInfoItr)
                        {
                            if (t_bVpnIdList[j] == t_vpnInfoItr->m_record.m_userId)
                            {
                                //t_userTp.m_tpObjId       = t_itr->m_record.m_tpObjId;
                                t_userTp.m_tpObjId       = 0;
                                t_userTp.m_tpId          = t_vpnRelTpList[j];
                                t_userTp.m_beginTime     = t_itr->m_beginTime;
                                t_userTp.m_endTime       = t_itr->m_endTime;
                                t_userTp.m_idType        = t_itr->m_idType;
                                t_userTp.m_userId        = m_pRateInfo->m_billUserId;
                                t_userTp.m_productId     = t_itr->m_productId;
                                t_userTp.m_servBund_Id   = t_itr->m_servBund_Id;
                                t_userTp.m_servId        = t_itr->m_servId;
                                t_userTp.m_feeBund_Id    = t_itr->m_feeBund_Id;
                                t_userTp.m_relationVpnId = NULL_VPN_ID;
                                t_userTp.m_subTpType     = TP_TYPE_NORMAL;
                                t_userTp.m_cellFlag      = FLAG_NO;
                                t_userTp.m_vpnFlag       = VPN_FLAG_OUT;
                                t_userTp.m_memberType_A  = VPN_MEMTYPE_NULL;
                                t_userTp.m_memberType_B  = VPN_MEMTYPE_NULL;
                                t_flag = true;
                                break;
                            }
                        }
                        if (t_flag)
                        {
                            t_vpnRelTp.push_back(t_userTp);
                            break;
                        }
                    }
                }
            }
        }
    
    if (!t_vpnRelTp.empty())
    {
        for (int i=0; i < t_vpnRelTp.size(); ++i)
        {
            m_pRateInfo->m_userTpList.push_back(t_vpnRelTp[i]);
        }
    }

   }
#ifdef FP_DEBUG
    FP_END(getusertpnew_add)
  #endif
    return true;
}

bool TpProc::setTpParamInfo()
{
  //设置用户级首月入网和当月入网标记
  m_pcdr[ATTRI_HALF_OPEN_FLAG]=0;
  m_pcdr[ATTRI_CURRENT_OPEN_FLAG]=0;
  if(m_pRateInfo->m_firstCallTime >= m_pRateInfo->m_cycParam.m_beginTime )
  {
    m_pcdr[ATTRI_CURRENT_OPEN_FLAG]=1;
    if(m_pRateInfo->m_firstCallTime >= m_pRateInfo->m_cycParam.m_halfTime)
    {
      m_pcdr[ATTRI_HALF_OPEN_FLAG]=1;
    }
  }
  vector<PO_UserTpParam_def*> t_userTpParamDefs;
  //Added By Sunam 2009/10/5 15:55:28
  vector<T_TPPARAM_VALUE> t_tpParamValueVec;
  bool  t_flag = true;
  //1. 设置政策参数中的额外属性
  for(vector<UserTp>::iterator t_tpItr=m_pRateInfo->m_userTpList.begin();
      t_tpItr!=m_pRateInfo->m_userTpList.end();++t_tpItr)
  {
      //2.求取资费政策参数定义
    t_userTpParamDefs.clear();
    if(m_pmManager->getUserTpParamDefInfo(t_tpItr->m_tpId,t_userTpParamDefs)==true)
    { //有用户资费参数定义，需要将用户资费参数实例求取出来
      char t_errMsg[128];
      //3.求取用户资费参数实例
      if(m_pmManager->getUserTpParam(t_tpItr->m_tpObjId,t_tpItr->m_paramList)<0)
      {
        sprintf(t_errMsg,"资费参数没有实例化!");
        m_pRateInfo->setErrInfo(E_USER_TPPARAM,t_errMsg);
        return false;
      }
      
      for(vector<PO_UserTpParam_def*>::iterator r_paramDefItr=t_userTpParamDefs.begin();
          r_paramDefItr!=t_userTpParamDefs.end();++r_paramDefItr)
      {
        t_flag = false;
        //4.根据资费政策参数定义设置用户资费参数实例一些属性
        for(vector<UserTpParam>::iterator r_paramItr=t_tpItr->m_paramList.begin();
            r_paramItr!=t_tpItr->m_paramList.end();++r_paramItr)
        {
          if(r_paramItr->m_tpParamId == (*r_paramDefItr)->m_record.m_tpParamId )
          {
            t_flag = true;
            r_paramItr->m_relatinType = (*r_paramDefItr)->m_record.m_relatinType;
            r_paramItr->m_valueType   = (*r_paramDefItr)->m_record.m_valueType;
            break;
          }else continue;
        }
        //Modified By Sunam 2009/11/20 14:07:52 有关联关系的参数不充许缺失，
        //无关联关系的参数在RateGlobalInfo.cpp中判断
        //(t_flag == false)
        if ((t_flag == false) && !((*r_paramDefItr)->m_record.m_relatinType == T_RELATYPE_NONE))
        { //用户资费参数错误(缺资费参数资料)
        	//Added By Sunam 2009/11/22 9:53:52 只有资费生效时，参数才必须实例化
			if (t_tpItr->isValid(m_pRateInfo->m_beginDateTime,m_pRateInfo->m_endDateTime))
			{
				sprintf(t_errMsg,"资费[%d]参数没有实例化!",(int)t_tpItr->m_tpId);
				m_pRateInfo->setErrInfo(E_USER_TPPARAM,t_errMsg);
				//增加帐务错误异常记录 add by xuf 2010-2-5 14:38:42
				m_pRateInfo->m_comBill.setErrorCode(t_tpItr->m_userId,E_ACCT_TP_PARAM,"0",USER_ID_TYPE,(long)(t_tpItr->m_tpObjId));
				
				return false;
			}
        }
      }
    }
    //Added By Sunam 2009/10/5 15:57:26
    t_tpParamValueVec.clear();
    for(vector<UserTpParam>::iterator t_tpParamItr=t_tpItr->m_paramList.begin();
        t_tpParamItr!=t_tpItr->m_paramList.end();++t_tpParamItr)
    {
      t_tpParamItr->m_levelType=LEVELTYPE_NULL;
      t_tpParamItr->m_memberType= VPN_MEMTYPE_NULL;
      if(t_tpParamItr->m_relatinType==T_RELATYPE_NONE)
      {
      }else if(t_tpParamItr->m_relatinType==T_RELATYPE_SPECIALCELL)
      {
        if(t_tpItr->m_subTpType == TP_TYPE_NORMAL)
        {
        t_tpItr->m_subTpType=TP_TYPE_CELL;
        }
			  //modified by gaojf 2009-2-19 14:20 经确认,
			  //该区号定位为位置信息
        if(m_pmManager->getUserTpCellInfo(//m_pcdr[ATTRI_HOME_CODE_A],
                                          m_pcdr[ATTRI_VISIT_CODE_A],
                                          t_tpParamItr->m_tpValue,
                                          m_pcdr[ATTRI_MSC] ,
                								          m_pcdr[ATTRI_LAC_A] ,
                								          m_pcdr[ATTRI_CELLID_A] ,
                								          m_pRateInfo->m_beginDateTime,
                								          t_tpParamItr->m_levelType,
                								          t_tpParamItr->m_memberType)==false)
        {
          //t_tpItr->m_validFlag = false;
          //小区外
          t_tpItr->m_cellFlag = FLAG_NO;
        }else
        { //小区内
          t_tpItr->m_cellFlag = FLAG_YES;
        }
      }else if(t_tpParamItr->m_relatinType==T_RELATYPE_SPECIALPHONE)
      {
        if(m_pmManager->getUserTpSpecialAttr(t_tpParamItr->m_tpValue,
                                             m_pRateInfo->m_otherParty,
                                             m_pcdr[ATTRI_HOME_CODE_B],
                								             m_pRateInfo->m_beginDateTime,
                								             t_tpParamItr->m_levelType,
                								             t_tpParamItr->m_memberType)==false)
        {
          t_tpItr->m_validFlag = false;
        }
      }else if(t_tpParamItr->m_relatinType==T_RELATYPE_SPECIALVPN)
      {
        //Modified By Sunam 2009/10/5 15:56:35 如果是T_RELATYPE_SPECIALVPN类型，则把所有的值保存下来
        //if(m_pmManager->getUserTpVpnInfo(t_tpParamItr->m_tpValue,
        //                                  m_pRateInfo->m_otherParty,
        //                                  m_pcdr[ATTRI_HOME_CODE_B],
        //        								          m_pRateInfo->m_beginDateTime,
        //        								          t_tpParamItr->m_levelType,
        //        								          t_tpParamItr->m_memberType)==false)
        //{
        //  t_tpItr->m_validFlag = false;
        //}
        t_tpParamValueVec.push_back(t_tpParamItr->m_tpValue);
      }
    }
    //Added By Sunam 2009/10/5 15:58:15 如果该资费里有多个关联的VPN集团则只要有一个满足，该资费即生效
    if (t_tpParamValueVec.size() > 0)
    {
        t_tpItr->m_validFlag = false;
        T_LEVEL_TYPE levelType=LEVELTYPE_NULL;
        T_MEMBER_TYPE memberType= VPN_MEMTYPE_NULL;
        for (int k=0; k < t_tpParamValueVec.size(); ++k)
        {
            if (m_pmManager->getUserTpVpnInfo(t_tpParamValueVec[k],
                                              m_pRateInfo->m_otherParty,m_pcdr[ATTRI_HOME_CODE_B],
                                              m_pRateInfo->m_beginDateTime,
                                              levelType,
                                              memberType))
            {
                t_tpItr->m_validFlag = true;
                break;
            }
        }
    }
    t_tpItr->m_firstMonthFlag = m_pcdr[ATTRI_CURRENT_OPEN_FLAG][0];
    t_tpItr->m_halfInNetFlag  = m_pcdr[ATTRI_HALF_OPEN_FLAG][0];
    //设置半月入网标志、首月标志 标记等
    if(t_tpItr->m_beginTime >= m_pRateInfo->m_cycParam.m_beginTime)
    {
      t_tpItr->m_firstMonthFlag  ='1';
      if(t_tpItr->m_beginTime >= m_pRateInfo->m_cycParam.m_halfTime)
      {
        t_tpItr->m_halfInNetFlag ='1';
      }
    }
  }
  //0. 将基本政策加入m_pRateInfo->m_userTpList
  for(vector<T_TP_ID>::iterator t_baseTpItr = m_baseTpList.begin();
      t_baseTpItr!=m_baseTpList.end();++t_baseTpItr)
  {
  	//added by hany for 3G at 2009-5-16 01:13上午
  	UserTp t_userTp(*t_baseTpItr,m_pRateInfo->m_billUserId);
    if (m_pcdr[ATTRI_SOURCE_TYPE]=="4D"&&
    	  m_pcdr[ATTRI_SP_SRVTYPE]!="70")
        t_userTp.m_tpObjId=m_pcdr[ATTRI_SP_SRVID];//VAC的基本政策用产品ID作为对象标识
    m_pRateInfo->m_userTpList.push_back(UserTp(*t_baseTpItr,m_pRateInfo->m_billUserId));
  }
  return true;
}

/*
	判断是否需要做用户套餐,可根据实际情况扩充
	返回false表示需要做用户政策
	返回true 表示不做用户政策
*/
bool TpProc::isNoNeedTP()
{
	if (m_pcdr[ATTRI_FREE_FEE] == FLAG_YES)
	{
		return true;
	}
	
	//专网用户一定要做用户套餐 changed by liux 2005.03.07
	if (m_pcdr[ATTRI_USER_SERVICE_TYPE_A_YZ] == SERVICE_TYPE_PSTN)
	{
		return false;
	}
	

	/*
		m_cdr[ATTRI_ISNONEED_TP]根据特殊号码表里的isNoNeedTp得到，含义为
		0:做用户政策(同普通)
		1:不做用户政策
		2:做用户政策(和所有二批均采用择优方式)
	*/
	int t_isNoNeedTp = m_pcdr[ATTRI_ISNONEED_TP].value();
	if(t_isNoNeedTp == 1) return true;
	
	//增加本地化处理
	if (ProcedureHelper::ExecuteProcedure("IsNoNeedTPSpecialProc") == 1)
	{
		return true;
	}
  return false;
}
//取基站基本优惠政策
bool TpProc::getBaseCellTp()
{
  if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GSM)
  {
    //根据各种规则判断，求取基站通套餐
  	T_TP_ID  t_baseCellTp;
  	if(m_pmManager->getBaseCellTp(m_pRateInfo->m_beginDateTime,
                								  m_pcdr[ATTRI_CALL_TYPE] ,
                								  m_pcdr[ATTRI_USER_SERVICE++t_baseTpItr)
  {
  	//added by hany for 3G at 2009-5-16 01:13涓婂崍
  	UserTp t_userTp(*t_baseTpItr,m_pRateInfo->m_billUserId);
    if (m_pcdr[ATTRI_SOURCE_TYPE]=="4D"&&
    	  m_pcdr[ATTRI_SP_SRVTYPE]!="70")
        t_userTp.m_tpObjId=m_pcdr[ATTRI_SP_SRVID];//VAC鐨勫熀鏈