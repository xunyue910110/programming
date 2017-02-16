#include "LocalizeCDRProc_010.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"


//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeGsmTrashGet);

//0正常话单 1无效话单 2continue
int LocalizeGsmTrashGet::execute()
{
	if (strncmp(m_pcdr[ATTRI_CALL_TYPE].c_str(), "F", 1) == 0)   //FF为TRASH单 预处理置
	{
		return 1;
	}
	
	/*
	if(strncmp(m_pcdr[ATTRI_SCP_USERTYPE].c_str(), "3", 1) == 0) //其他智能网话单TRASH处理
	{
    return 1;
	}
	*/
	
	//呼转中的对端及第三方号码需要去60或060处理
	if(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW || m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORWMTC)
	{
		if (strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "60", 2) == 0)
		{
			m_pcdr[ATTRI_MSISDN_B_ORG] = m_pcdr[ATTRI_MSISDN_B_ORG].c_str()+2;
		}
		if (strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "060", 3) == 0)
		{
			m_pcdr[ATTRI_MSISDN_B_ORG] = m_pcdr[ATTRI_MSISDN_B_ORG].c_str()+3;
		}
		if (strncmp(m_pcdr[ATTRI_ORG_PHONE].c_str(), "60", 2) == 0)
		{
			m_pcdr[ATTRI_ORG_PHONE] = m_pcdr[ATTRI_ORG_PHONE].c_str()+2;
		}
		if (strncmp(m_pcdr[ATTRI_ORG_PHONE].c_str(), "060", 3) == 0)
		{
			m_pcdr[ATTRI_ORG_PHONE] = m_pcdr[ATTRI_ORG_PHONE].c_str()+3;
		}
		//无条件呼转不拆分出呼转被叫 置错单(内蒙置为无效)
		if((m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNCONDITIONAL  ||
			m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNCONDITIONAL2 ||
			m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNREACHABLE) &&
			m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORWMTC &&
			m_pcdr[ATTRI_ERROR_CODE] == DEFAULT_ERROR)
		{
			m_pcdr[ATTRI_ERROR_CODE] = GSM_ERROR_FORWARDMTC;
			return 1;
		}
	}
	
	return 2;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeSmTrashGet);

//0正常话单 1无效话单 2continue
int LocalizeSmTrashGet::execute()
{
	if (strncmp(m_pcdr[ATTRI_CALL_TYPE].c_str(), "F", 1) == 0)   //FF为TRASH单 预处理置
	{
		return 1;
	}
	
	//PPS联通在信 上行话单发手机和发小灵通的TRASH处理，下行保留 2009年2月上旬后该数据源合并到A2中
	if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_PPS2 &&
		(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMO || m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMOF))
	{
		if(isMobile(m_pcdr[ATTRI_MSISDN_B]) ||
			strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "0", 1) == 0 ||
			strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "1060", 4) == 0)
		{
			return 1;
		}
	}
	
	//PPS点对点短信 上行话单 2009年2月上旬之前处理逻辑 发手机和发小灵通的保留，其余TRASH处理
	if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_PPS1 &&
		(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMO || m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMOF) &&
		m_FilterInfo->m_smsMethod == SM_PROCESS_METHOD)
	{
		if(!isMobile(m_pcdr[ATTRI_MSISDN_B]) &&
			strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "0", 1) != 0 &&
			strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "1060", 4) != 0)
		{
			return 1;
		}
	}
	
	
	return 2;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeGprsTrashGet);

//0正常话单 1无效话单 2continue
int LocalizeGprsTrashGet::execute()
{
	return 2;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeMiscTrashGet);

//0正常话单 1无效话单 2continue
int LocalizeMiscTrashGet::execute()
{
	if (strncmp(m_pcdr[ATTRI_CALL_TYPE].c_str(), "F", 1) == 0)   //FF为TRASH单 预处理置
	{
		return 1;
	}
	
	return 2;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeIPTrashGet);

//0正常话单 1无效话单 2continue
int LocalizeIPTrashGet::execute()
{
	if (strncmp(m_pcdr[ATTRI_CALL_TYPE].c_str(), "F", 1) == 0)   //FF为TRASH单 预处理置
	{
		return 1;
	}
	
	return 2;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeSmCdrGet);

//0 continue 1 finish
int LocalizeSmCdrGet::execute()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeFixSmCdrJudge);

//0 继续 1 正常处理
int LocalizeFixSmCdrJudge::execute()
{
	T_ATTRI_VAL   tmp_msisdn_b;
	tmp_msisdn_b = m_pcdr[ATTRI_MSISDN_B];
	tmp_msisdn_b = tmp_msisdn_b.append(0,"0");
	if(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "0", 1) != 0 &&
		strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "1", 1) != 0 &&
		m_globalManager->getHomeAreaCodeByMsisdnN(tmp_msisdn_b,m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],areaCode,groupId,userType))
	{
		//默认对端电信
		m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]   = m_pcdr[ATTRI_MSISDN_B];
		m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SM_SERV_TYPE_FIX;
		m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B] = "0";
		m_pcdr[ATTRI_USER_ASP_B]          = COMPANY_TYPE_CNC;
		m_pcdr[ATTRI_SM_TYPESTAT]         = "CNC";
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeProvCdrGet);

//0正常话单 1错单 2continue
int LocalizeProvCdrGet::execute()
{
	//错单直接返回
	if(getError())
		return 0;
	
	//非预开卡返回
	if(m_FilterInfo->m_acctTag != "2")
		return 0;
	
	//对于预开用户，下行话单且费用为0的，或者是信息费包月作错单处理
	if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_SMS)
	{
		if(m_pcdr[ATTRI_CHARGE_TYPE] == CHARGE_TYPE_03 ||
			((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMT || m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMTF) && 
			(m_pcdr[ATTRI_ORGFEE_FIELD2].value()==0 && m_pcdr[ATTRI_ORGFEE_FIELD3].value()==0)))
		{
			m_pcdr[ATTRI_ERROR_CODE] = SM_ERROR_PREPAY;
			return 1;
		}
	}
	
	//预开卡用户多输出清单给CRM
	//语音 短信 GPRS IP 彩信发送触发用户转正
	//PPS所有业务都触发用户转正
	//普通用户代收业务都不触发用户转正 错单
	if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_UNICOMSP)
	{
		//彩信发送成功 触发转正，否则错单
		if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SP_MMS && m_pcdr[ATTRI_SEND_STATUS] == "0")
		{
			m_pcdr[ATTRI_BACK_CHANNEL] = CHANNEL_BACKUP;
			return 2;
		}
		else
		{
			m_pcdr[ATTRI_ERROR_CODE] = ERROR_PREPAY_CDR;
			return 1;
		}
	}
	//SP包月话单 错单处理
	else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_SPMONTH)
	{
		m_pcdr[ATTRI_ERROR_CODE] = ERROR_PREPAY_CDR;
		return 1;
	}
	//智能网代收费 充值类话单不触发转正
	else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_STATIS)
	{
		if(m_pcdr[ATTRI_SOURCE_TYPE] == "A8" || m_pcdr[ATTRI_SOURCE_TYPE] == "A9" ||
			m_pcdr[ATTRI_SOURCE_TYPE] == "AA" || m_pcdr[ATTRI_SOURCE_TYPE] == "AB")
		{
			return 0;
		}
		else
		{
			m_pcdr[ATTRI_BACK_CHANNEL] = CHANNEL_BACKUP;
			return 2;
		}
	}
	else
	{
		m_pcdr[ATTRI_BACK_CHANNEL] = CHANNEL_BACKUP;
	}
	
	return 2;
}


//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeMiscNormalSet);

//0正常话单 1错单 2continue
int LocalizeMiscNormalSet::execute()
{
	//宝视通清单帐单 集团业务 不判计费号码
	if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SP_BST || m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SP_GRP)
	{
		return 0;
	}
	
	return 2;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeProvTrashGet);

//0正常话单 1无效话单 2continue
int LocalizeProvTrashGet::execute()
{
	//出访话单智能网话单trash处理
	if ((m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_CRO || m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_CIO || m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_GBO)&&
		m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == SERVICE_TYPE_SSP)
	{
		return 1;
	}
	if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_NET && strncmp(m_pcdr[ATTRI_CALL_TYPE].c_str(), "F", 1) == 0)
	{
	  return 1;
	}	

	return 2;
}

//add by shenglj 2009-04-13 11:11:01
//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeProvScpTrashGet);

//0正常话单 1无效话单 2continue
int LocalizeProvScpTrashGet::execute()
{
	return 2;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeChannelReset);

int LocalizeChannelReset::execute()
{
	//错单直接返回
	if(getError())
		return 0;
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeProvByNetType);

int LocalizeProvByNetType::execute()
{
	//错单直接返回
	if(getError())
		return 0;

	//2G、3G、OCS互转期间的话单过滤 add by yangrl 2010/5/7 10:56:50
	//1.OCS用户要转成后付费用户，CRM业务成功后指令发到OCS平台那一刻,CRM侧已经是后付费用户，该用户这时的话单，就因该按后付费套餐批价,然而指令可能延时等原因,OCS侧还可能会下话单,但OCS这时就会批成0费用的话单,继续传给我们BILLING侧,需要BILLING侧批价,但现在OCS和后付费是两种通道。做无效处理
  if ((m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GSM_OCS ||
			m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_OCS ||
      m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GPRS_OCS ||
			m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SP_OCS ||
			m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_MONFEE_OCS ||
			m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_RECHARGE_OCS
			)
      && 
      (m_pcdr[ATTRI_NET_TYPE_CODE]!= NET_TYPE_OCS &&
       m_pcdr[ATTRI_NET_TYPE_CODE]!=NET_TYPE_OCS_3G)
     )
  {
     m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_TRASH;
     return 1;
  }

  //2.后付费用户转成OCS用户，在互转那一刻,普通交换机可能会下话单,但这时用户已经是OCS用户，需要按OCS套餐批价,所以是不是也要回传给OCS批价，针对这种话单，我们是把话单输出到跟OCS的一个接口目录里面，现在甘肃是只有这两种接口处理，其他话单做无效单处理 ,要是跟OCS没接口 就只能做无效处理。
	if ((m_pcdr[ATTRI_SOURCE_TYPE]!=SOURCE_TYPE_GSM_OCS &&
			 m_pcdr[ATTRI_SOURCE_TYPE] != SOURCE_TYPE_SMS_OCS &&
       m_pcdr[ATTRI_SOURCE_TYPE]!=SOURCE_TYPE_GPRS_OCS &&
			 m_pcdr[ATTRI_SOURCE_TYPE] != SOURCE_TYPE_SP_OCS &&
			 m_pcdr[ATTRI_SOURCE_TYPE] != SOURCE_TYPE_MONFEE_OCS &&
			 m_pcdr[ATTRI_SOURCE_TYPE] != SOURCE_TYPE_RECHARGE_OCS
			 )
       && (m_pcdr[ATTRI_NET_TYPE_CODE]==NET_TYPE_OCS ||
		   m_pcdr[ATTRI_NET_TYPE_CODE]==NET_TYPE_OCS_3G )
		 )
  {
     m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_TRASH;//修改的地方
     return 1;
  }
  // end add by yangrl 2010/5/7 10:56:50

	return 0;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeProvRegionInfoGet);

//0不需要求取行政区域信息 1需要求取行政区域信息
int LocalizeProvRegionInfoGet::execute()
{
	
	return 1;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeFixGsmTrashGet);

//0正常话单 1无效话单 2continue
int LocalizeFixGsmTrashGet::execute()
{
	if (strncmp(m_pcdr[ATTRI_CALL_TYPE].c_str(), "F", 1) == 0)   //FF为TRASH单 预处理置
	{
		return 1;
	}
	
	if (m_pcdr[ATTRI_BIZ_TYPE].empty()) //空话单无效处理
	{
		return 1;
	}
	
	return 2;
}


//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeFixGsmNormalSet);

//0正常话单 1无效话单 2continue 9特殊话单
int LocalizeFixGsmNormalSet::execute()
{
	return 2;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeFixGsmgetTrunkProperty);
//0 正常 1 无效话单 2continue
int LocalizeFixGsmgetTrunkProperty::execute()
{
	if (m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_FIXGSM_800)
	{
		return 0;
	}
	
	T_ATTRI_VAL  m_bill_number0;  //中继计费代表号码
	T_ATTRI_VAL  m_bill_type0;    //中继计费代表号码类别 0:保持计费号码不变 1:替换计费号码
	//默认入中继
	m_pcdr[ATTRI_TRUNK_DIRECT] = TRUNK_IN;
	m_pcdr[ATTRI_TRUNK_FLAG] = TRUNK_FLAG_NO;
	if(m_pcdr[ATTRI_TRUNK_DIRECT] == TRUNK_IN)
	{
		if(m_globalManager->getTypeByFixTrunk(m_pcdr[ATTRI_VISIT_CODE_A],m_pcdr[ATTRI_MSC],
			m_pcdr[ATTRI_TRUNK_DIRECT],m_pcdr[ATTRI_TRUNKGROUPIN],m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
			m_bill_type0,m_bill_number0,m_pcdr[ATTRI_IP_SERV_TYPE_A])<=0)
		{
			return 2;
		}
	}
	else if(m_pcdr[ATTRI_TRUNK_DIRECT] == TRUNK_ALL)
	{
		m_pcdr[ATTRI_TRUNK_DIRECT] = TRUNK_IN;
		if(m_globalManager->getTypeByFixTrunk(m_pcdr[ATTRI_VISIT_CODE_A],m_pcdr[ATTRI_MSC],
			m_pcdr[ATTRI_TRUNK_DIRECT],m_pcdr[ATTRI_TRUNKGROUPIN],m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
			m_bill_type0,m_bill_number0,m_pcdr[ATTRI_IP_SERV_TYPE_A])<=0)
		{
			m_pcdr[ATTRI_TRUNK_DIRECT] = TRUNK_OUT;
			if(m_globalManager->getTypeByFixTrunk(m_pcdr[ATTRI_VISIT_CODE_A],m_pcdr[ATTRI_MSC],
				m_pcdr[ATTRI_TRUNK_DIRECT],m_pcdr[ATTRI_TRUNKGROUPOUT],m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
				m_bill_type0,m_bill_number0,m_pcdr[ATTRI_IP_SERV_TYPE_A])<=0)
			{
				return 2;
			}
		}
	}
	else
	{
		if(m_globalManager->getTypeByFixTrunk(m_pcdr[ATTRI_VISIT_CODE_A],m_pcdr[ATTRI_MSC],
			m_pcdr[ATTRI_TRUNK_DIRECT],m_pcdr[ATTRI_TRUNKGROUPOUT],m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
			m_bill_type0,m_bill_number0,m_pcdr[ATTRI_IP_SERV_TYPE_A])<=0)
		{
			return 2;
		}
	}
	
	m_pcdr[ATTRI_TRUNK_FLAG] = TRUNK_FLAG_YES;
	if(m_bill_type0 == "1")
	{
		m_pcdr[ATTRI_MSISDN_A] = m_bill_number0; //填实际计费号码
	}
	if(m_pcdr[ATTRI_IP_SERV_TYPE_A] == "90")
	{
		m_pcdr[ATTRI_TRASH_FLAG] = ATTRI_TRASH_Y;
		return 1;
	}
	
	return 2;
}


//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeFixGsmSameMscCheck);

//0 正常 1 不需要校验 2 错单返回
int LocalizeFixGsmSameMscCheck::execute()
{
	////SCDMA数据源不进行同一地区校验
	/*
	if (m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_FIXGSM_SCDMA)
	{
	return 1;
	}
	*/

	if(m_pcdr[ATTRI_TRUNK_FLAG] == TRUNK_FLAG_NO && m_pcdr[ATTRI_VISIT_CODE_A] != m_pcdr[ATTRI_VISIT_CODE_A_ORG])
	{
		m_pcdr[ATTRI_ERROR_CODE] = AH_FIX_ROAM_ERROR;
		return 2;
	}
	
	return 0;
}


///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeFixGsmSetBillNumber);
//0正常话单 1无效话单 2continue
int LocalizeFixGsmSetBillNumber::execute()
{
	if( m_pcdr[ATTRI_TRUNK_FLAG] == TRUNK_FLAG_YES)
		return 2;
	
	T_ATTRI_VAL  m_bill_number1;  //千群百号代表号码
	T_ATTRI_VAL  m_bill_type1;    //千群百号代表号码类别 0:保持计费号码不变 1:替换计费号码
	T_ATTRI_VAL  m_bill_number2;  //号码转换代表号码
	T_ATTRI_VAL  m_bill_type2;    //号码转换代表号码类别 0:保持计费号码不变 1:替换计费号码
	
	T_ATTRI_VAL r_homecode;
	T_ATTRI_VAL r_longgroup;
	T_ATTRI_VAL r_usertype;
	T_ATTRI_VAL r_msisdn_a;
	T_ATTRI_VAL t_msisdn_a;
	
	r_msisdn_a = m_pcdr[ATTRI_MSISDN_A];
	t_msisdn_a = m_pcdr[ATTRI_MSISDN_A];
	bool addareacode = true;
	
	if(strncmp(r_msisdn_a.c_str(), "0", 1)!=0 && r_msisdn_a.length()>=9)
	{
		t_msisdn_a.append(0,"0");
		if(m_globalManager->getHomeAreaCodeByMsisdnN(t_msisdn_a,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			r_homecode,r_longgroup,r_usertype)>0)
		{
			if(m_globalManager->isProvInnerCode(r_homecode))
			{
				addareacode = false;
				r_msisdn_a = t_msisdn_a;
			}
		}
	}
	else if(strncmp(r_msisdn_a.c_str(), "0", 1)==0 && r_msisdn_a.length()>=10)
	{
		if(m_globalManager->getHomeAreaCodeByMsisdnN(r_msisdn_a,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			r_homecode,r_longgroup,r_usertype)>0)
		{
			if(m_globalManager->isProvInnerCode(r_homecode))
			{
				addareacode = false;
			}
		}
	}
	
	if(addareacode)
	{
		if(strncmp(r_msisdn_a.c_str(), "0", 1) !=0)
			r_msisdn_a = m_pcdr[ATTRI_VISIT_CODE_A] + m_pcdr[ATTRI_MSISDN_A];
	}
	
	m_pcdr[ATTRI_BILL_FLAG] = BILL_FLAG_NOMAL;
	
	//是否有号码转换计费
	if(m_globalManager->getBillNumberByPhone(r_msisdn_a,m_pcdr[ATTRI_MSC],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
		m_bill_type2,m_bill_number2,m_pcdr[ATTRI_MISC_STR2]))
	{
		m_pcdr[ATTRI_STATIS_STR2] = BILL_FLAG_2;
		if(m_bill_type2 == "1")
		{
			m_pcdr[ATTRI_MSISDN_A]   = m_bill_number2; //填实际计费号码
			m_pcdr[ATTRI_NBR_NUMBER] = m_bill_number2; //号码转换计费号码填到保留字段1上
			r_msisdn_a             = m_bill_number2;
		}
		
		m_pcdr[ATTRI_BILL_FLAG] = m_pcdr[ATTRI_BILL_FLAG].value() ^ m_pcdr[ATTRI_STATIS_STR2].value();
		if(m_pcdr[ATTRI_MISC_STR2] == "90")
		{
			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_MISC_STR2];
			return 1;
		}
	}
	
	//是否有号段拨打号段优惠
	if(m_globalManager->getServTypeByDoublePhone(m_pcdr[ATTRI_VISIT_CODE_A],m_pcdr[ATTRI_MSC],r_msisdn_a,m_pcdr[ATTRI_MSISDN_B],
		m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_MISC_STR3]))
	{
		m_pcdr[ATTRI_STATIS_STR3] = BILL_FLAG_3;
		
		m_pcdr[ATTRI_BILL_FLAG] = m_pcdr[ATTRI_BILL_FLAG].value() ^ m_pcdr[ATTRI_STATIS_STR3].value();
		if(m_pcdr[ATTRI_MISC_STR3] == "90")
		{
			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_MISC_STR3];
			return 1;
		}
	}
	
	//是否有千群百号计费
	if(m_globalManager->getNbrGroupByPhone(r_msisdn_a,m_pcdr[ATTRI_MSC],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
		m_bill_type1,m_bill_number1,m_pcdr[ATTRI_MISC_STR1]))
	{
		m_pcdr[ATTRI_STATIS_STR1] = BILL_FLAG_1;
		if(m_bill_type1 == "1")
		{
			m_pcdr[ATTRI_IMSI_NUMBER] = m_bill_number1; //填实际计费号码
			m_pcdr[ATTRI_NBR_NUMBER] = m_bill_number1; //千群百号计费号码填到保留字段1上
			//modify by dph，千群百号不使用代表号码替换计费号码
			//r_msisdn_a             = m_bill_number1;
		}
		
		m_pcdr[ATTRI_BILL_FLAG] = m_pcdr[ATTRI_BILL_FLAG].value() ^ m_pcdr[ATTRI_STATIS_STR1].value();
		if(m_pcdr[ATTRI_MISC_STR1] == "90")
		{
			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_MISC_STR1];
			return 1;
		}
	}
	
	return 2;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeFixGsmTrashNumberNbr);
//0正常话单 1无效话单 2continue
int LocalizeFixGsmTrashNumberNbr::execute()
{
	/*if(m_pcdr[ATTRI_SOURCE_TYPE] > SOURCE_TYPE_FIXGSM_METER)
		return 2;
	
	m_PO_TjSelectNbrContainer = (ParamContainer<PO_TjSelectNbr>*) PMHelperall::GetParamContainer("PO_TjSelectNbr");
	m_PO_TjTrashNbrContainer  = (ParamContainer<PO_TjTrashNbr>*) PMHelperall::GetParamContainer("PO_TjTrashNbr");
	
	T_ATTRI_VAL r_homecode;
	T_ATTRI_VAL r_longgroup;
	T_ATTRI_VAL r_usertype;
	T_ATTRI_VAL r_msisdn_a;
	T_ATTRI_VAL t_msisdn_a;
	
	r_msisdn_a = m_pcdr[ATTRI_MSISDN_A];
	t_msisdn_a = m_pcdr[ATTRI_MSISDN_A];
	
	if(r_msisdn_a.length()<=0)
		return 1;
	
	if(isMobile(r_msisdn_a) || strncmp(r_msisdn_a.c_str(), "00", 2)==0)
		return 1;
	
	if(strncmp(r_msisdn_a.c_str(), "0", 1)!=0 && r_msisdn_a.length()>=9)
	{
		t_msisdn_a.append(0,"0");
		if(m_globalManager->getHomeAreaCodeByMsisdnN(t_msisdn_a,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			r_homecode,r_longgroup,r_usertype)>0)
		{
			if(m_globalManager->isProvInnerCode(r_homecode))
			{
				r_msisdn_a = t_msisdn_a.c_str() + r_homecode.length();
			}
		}
	}
	else if(strncmp(r_msisdn_a.c_str(), "0", 1)==0 && r_msisdn_a.length()>=10)
	{
		if(m_globalManager->getHomeAreaCodeByMsisdnN(r_msisdn_a,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			r_homecode,r_longgroup,r_usertype)>0)
		{
			if(m_globalManager->isProvInnerCode(r_homecode))
			{
				r_msisdn_a = r_msisdn_a.c_str() + r_homecode.length();
			}
		}
	}
	
	//汇接局话单用TD_TJ_TRASH_NBR过滤，在TD_TJ_TRASH_NBR表内算无效话单
	if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_FIXGSM_HUIJIE)
	{
		if(trashSourceByPhone(r_msisdn_a,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME]))
		{
			return 9;
		}
		else
		{
			return 2;
		}
	}
	else  //汇接局话单用TD_TJ_SELECT_NBR计费，不在TD_TJ_SELECT_NBR表内算无效话单
	{
		if(chooseSourceByPhone(r_msisdn_a,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME]))
		{
			return 2;
		}
		else
		{
			return 9;
		}
	}*/
	
	return 2;
}

bool LocalizeProvRegionInfoGet::ScdmatrashfixPhone(const T_ATTRI_VAL &r_msc,
												   const T_ATTRI_VAL &r_callingPhone,
												   const T_ATTRI_VAL &r_calledPhone,
												   const T_DATE_TIME &r_beginTime)
{
    PO_NmFixTrashPtp po;
    vector<PO_NmFixTrashPtp*> rpo;
    vector<PO_NmFixTrashPtp*>::iterator itr;
    PO_NmFixTrashPtp *rpo1;
	
	int i;
	
    po.recordStruct.m_msc = r_msc;
	for(i=r_callingPhone.length(); i>4; i--)
	{
		po.recordStruct.m_phone = r_callingPhone.substr(0,i);
		if(m_PO_ScdmaFixTrashContainer->searchMulti(po,rpo)>0)
			return false;
	}

	po.recordStruct.m_msc = r_msc;
	for(i=r_calledPhone.length(); i>4; i--)
	{
		po.recordStruct.m_phone = r_calledPhone.substr(0,i);
		if(m_PO_ScdmaFixTrashContainer->searchMulti(po,rpo)>0)
			return true;
	}
	return false;
}
