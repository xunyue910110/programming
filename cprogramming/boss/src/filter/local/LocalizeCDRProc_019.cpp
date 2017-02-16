#include "LocalizeCDRProc_019.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

//江苏联通军网VPNNID号
const string VPNID_GZHEJ        ("99434768985");

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
    //无条件呼转不拆分出呼转被叫 置错单
		/*wangzj 山西需要入库 20090418
		if((m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNCONDITIONAL  ||
			  m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNCONDITIONAL2 ||
			  m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNREACHABLE) &&
			  m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORWMTC &&
			  m_pcdr[ATTRI_ERROR_CODE] == DEFAULT_ERROR)
		{
			m_pcdr[ATTRI_ERROR_CODE] = GSM_ERROR_FORWARDMTC;
			return 2;
		}
	  */
 	}

//cout<<"m_pcdr[ATTRI_MSISDN_B_ORG]:"<<m_pcdr[ATTRI_MSISDN_B_ORG]<<__FILE__<<__LINE__<<endl;

  //无效话单
  if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "*72", 3) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "*73", 3) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "*74", 3) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "*75", 3) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "*77", 3) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "*78", 3) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "*79", 3) == 0 ||
     strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "*90", 3) == 0 ||
     strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "*92", 3) == 0)
  {
      return 1;
  }

  if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "0*72", 4) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "0*73", 4) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "0*74", 4) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "0*75", 4) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "0*77", 4) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "0*78", 4) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "0*79", 4) == 0 ||
     strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "0*90", 4) == 0 ||
     strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "0*92", 4) == 0 ||
     strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "13334", 5) == 0 ||
     strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "13344", 5) == 0 ||
     strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "EEE123", 6) == 0)
  {
      return 1;
  }

  //去前一位
  //010156、010157、010158、010159开头时，去掉前缀0 对端号码为010106114时，去掉前缀0
  if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "0100", 4) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "0165", 4) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "010156", 6) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "010157", 6) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "010158", 6) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "010159", 6) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "010106114", 9) == 0)
  {
  		m_pcdr[ATTRI_MSISDN_B_ORG] = m_pcdr[ATTRI_MSISDN_B_ORG].c_str()+1;
  }

  //去前二位
  //对端号码带3*,分拣去掉3*后,再根据号码前是60还是060来判断为IVPN话单,进行剔除
  if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "3*", 2) == 0)
  {
  		m_pcdr[ATTRI_MSISDN_B_ORG] = m_pcdr[ATTRI_MSISDN_B_ORG].c_str()+2;
  		if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "60", 2) == 0 ||
  	     strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "060", 3) == 0)
  	  {
         return 1;
  	  }
  }

  //去前三位
  if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "*76", 3) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "03*", 3) == 0)
  {
  		m_pcdr[ATTRI_MSISDN_B_ORG] = m_pcdr[ATTRI_MSISDN_B_ORG].c_str()+3;
  }

  //去前四位
  if(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "*7600", 5) == 0 ||
  	 strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(), "*76013", 6) == 0)
  {
  		m_pcdr[ATTRI_MSISDN_B_ORG] = m_pcdr[ATTRI_MSISDN_B_ORG].c_str()+4;
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
			//m_pcdr[ATTRI_CDR_CHANNEL] = "precard";
	    	m_pcdr[ATTRI_BACK_CHANNEL] = CHANNEL_BACKUP;
      		return 2;
		}
		else
		{
			//m_pcdr[ATTRI_CDR_CHANNEL] = "precard";
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
		 	//m_pcdr[ATTRI_CDR_CHANNEL] = "precard";
       		m_pcdr[ATTRI_BACK_CHANNEL] = CHANNEL_BACKUP;
       		return 2;
		 }
	}
	else
	{
		//m_pcdr[ATTRI_CDR_CHANNEL] = "precard";
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
	return 2;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeChannelReset);

int LocalizeChannelReset::execute()
{
	//错单直接返回
	if(getError())
	  return 0;

  //端局、国内出访、国内边漫出访话单过滤江浙沪军网话单
  if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_MSC || m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_CRO ||
  	 m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_GBO)
  {
	   if(m_globalManager->getVpnInfoByMsisdn(VPNID_GZHEJ.c_str(),m_pcdr[ATTRI_MSISDN_A],
	  	        m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME]))
	   {
	  	 m_pcdr[ATTRI_ERROR_CODE]  = GSM_ERROR_GZHEJ;
	  	 m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_ERROR;
	  	 return 1;
	   }
  }
	//流量调整，如果为0,为空,或1024的整数倍,则不变。
	//否则不足1K按1K算,不足2K按2K算,......,即向上取整
	if (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GPRS_CG   //GPRS网关
	||  m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GPRS_ROG  //GPRS国内出访下发
	||  m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GPRS_ROIG //GPRS国际出访下发
	||  m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GPRS_PPS  //G网PPS数据业务
	)
	{
		long iDataUp1,iDataDown1,iDataUp2,iDataDown2;
		if (m_pcdr[ATTRI_GPRS_DATA_UP1].empty()) iDataUp1 = 0;
		else iDataUp1 = atol(m_pcdr[ATTRI_GPRS_DATA_UP1].c_str());
		
		if (m_pcdr[ATTRI_GPRS_DATA_DOWN1].empty()) iDataDown1 = 0;
		else iDataDown1 = atol(m_pcdr[ATTRI_GPRS_DATA_DOWN1].c_str());
		
		if (m_pcdr[ATTRI_GPRS_DATA_UP2].empty()) iDataUp2 = 0;
		else iDataUp2 = atol(m_pcdr[ATTRI_GPRS_DATA_UP2].c_str());
		
		if (m_pcdr[ATTRI_GPRS_DATA_DOWN2].empty()) iDataDown2 = 0;
		else iDataDown2 = atol(m_pcdr[ATTRI_GPRS_DATA_DOWN2].c_str());
		
		char cData[40]="\0";
		
		if (iDataUp1 % 1024 != 0){
			iDataUp1 = ((iDataUp1 / 1024) + 1)*1024;
			sprintf(cData,"%ld",iDataUp1);
			m_pcdr[ATTRI_GPRS_DATA_UP1] = cData;
		}
		if (iDataDown1 % 1024 != 0){			
			iDataDown1 = ((iDataDown1 / 1024) + 1)*1024;
			sprintf(cData,"%ld",iDataDown1);
			m_pcdr[ATTRI_GPRS_DATA_DOWN1] = cData;			
		}
		if (iDataUp2 % 1024 != 0){
			iDataUp2 = ((iDataUp2 / 1024) + 1)*1024;
			sprintf(cData,"%ld",iDataUp2);
			m_pcdr[ATTRI_GPRS_DATA_UP2] = cData;						
		}
		if (iDataDown2 % 1024 != 0){
			iDataDown2 = ((iDataDown2 / 1024) + 1)*1024;
			sprintf(cData,"%ld",iDataDown2);
			m_pcdr[ATTRI_GPRS_DATA_DOWN2] = cData;						
		}
	}
	if (m_pcdr[ATTRI_BACK_CHANNEL] == CHANNEL_BACKUP)
	{
		if (m_pcdr[ATTRI_HOME_CODE_A]=="0351")
		{
			m_pcdr[ATTRI_CDR_CHANNEL] = "precard1";
		}
		else if (m_pcdr[ATTRI_HOME_CODE_A].in("0355","0359","0356"))
		{
			m_pcdr[ATTRI_CDR_CHANNEL] = "precard2";
		}
		else if (m_pcdr[ATTRI_HOME_CODE_A].in("0357","0358","0352"))
		{
			m_pcdr[ATTRI_CDR_CHANNEL] = "precard3";
		}
		else
		{
			m_pcdr[ATTRI_CDR_CHANNEL] = "precard4";
		}
		
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeProvByNetType);
/*
各省ip业务如果是直接绑资料，走批价的方式，不需要再走一次分拣.
请大家在处理类里面改成 UniqueProc;ProvCDRUNProc 直接走二次分拣
至于用户A的归属求取，需要在LocalizeCDRProc_xxx.cpp内的LocalizeProvByNetType
内增加从资料获取归属方式
*/
int LocalizeProvByNetType::execute()
{
	//错单直接返回
	if(getError())
	  return 0;

	//端局智能网用户scpTrash处理 del by songyx 20090424 begin
	//if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GSM_MSC && m_pcdr[ATTRI_NET_TYPE_CODE]==NET_TYPE_PPS)
	//{
  //   m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_SCP;
  //  return 1;
	//}
	//端局智能网用户scpTrash处理 del by songyx 20090424 end
	
	//端局智能网用户scpTrash处理 add 短信类  add by songyx 20090424 begin
	if( m_pcdr[ATTRI_NET_TYPE_CODE]==NET_TYPE_PPS 
	&& (  m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GSM_MSC    ||
		  m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GSM_CRO    ||//总部下发的PPS国内漫游话单
		  m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_GSMSC  ||
		  m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_PSMG   ||
		  m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_NSMG   ||
		  m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_MINTER ||
		  m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_GINTER ||
		  m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_IINTER ||
		  m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GPRS_CG
		 ))
	{
     m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_SCP;
     return 1;
	}	
	////端局智能网用户scpTrash处理 add 短信类  add by songyx 20090424 end
	
	////过滤NET_TYPE_CODE=WV and BRAND_CODE=IVPN的话单 add 20090427 只包括语音不过滤短信
	//if((m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GSM_MSC ) && (m_pcdr[ATTRI_NET_TYPE_CODE]=="WV")
	//	   && (m_pcdr[ATTRI_USER_SERVICE_TYPE_A_YZ]=="IVPN"))
	//{
    // m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_SCP;
    // return 1;
	//}	
	////过滤NET_TYPE_CODE=WV and BRAND_CODE=IVPN的话单 end 20090427 
	
	//add by chenzm@20090417
	//山西IP业务直接走二分，因此需要在绑定资料后，设定A的归属。
	//if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_IP_193)
	//{
	//	m_pcdr[ATTRI_HOME_CODE_A] = m_pcdr[ATTRI_OFFICE_CODE];
	//	return 1;
	//}
	//add end
	
  //资料绑定已经加判网别类型，不符合的都进无主通道
/*
	if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_IP_193 && (m_pcdr[ATTRI_NET_TYPE_CODE]!=NET_TYPE_193REG &&
		 m_pcdr[ATTRI_NET_TYPE_CODE]!=NET_TYPE_193TRK && m_pcdr[ATTRI_NET_TYPE_CODE]!=NET_TYPE_IP &&
		 m_pcdr[ATTRI_NET_TYPE_CODE]!=NET_TYPE_LONGRENT && m_pcdr[ATTRI_NET_TYPE_CODE]!=NET_TYPE_DATATRK &&
		 m_pcdr[ATTRI_NET_TYPE_CODE]!=NET_TYPE_DATARENT && m_pcdr[ATTRI_NET_TYPE_CODE]!=NET_TYPE_NETRENT &&
		 m_pcdr[ATTRI_NET_TYPE_CODE]!=NET_TYPE_DATASP))
	{
		m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_TRASH;
		return 1;
	}
*/
	static bool m_mdbConneced = false;
	if (!m_mdbConneced)
	{
		if (!m_mdbMgr.initialization(m_Config->m_mdbInfo))
		{
			return 0;
		}
		m_mdbConneced = true;
	}

	//193加拨17911进无主
	if (m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_IP_193)
	{
		if (m_pcdr[ATTRI_NET_TYPE_CODE] == NET_TYPE_193TRK)//51
		{
			if (m_pcdr[ATTRI_SERVICE_TYPE] == "1")
			{
				if (!userHaveNetTypeCode("55"))
					m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_NOINFO;
			}
		}
		if (m_pcdr[ATTRI_NET_TYPE_CODE] == NET_TYPE_IP)//55
		{
			if (m_pcdr[ATTRI_SERVICE_TYPE] == "2")
			{
				if (!userHaveNetTypeCode("51"))
					m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_NOINFO;
			}
		}
		return 1;
	}
	
//wujg add 20090531
if((m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GSM_MSC) && (m_pcdr[ATTRI_CALLED_TYPE_B]!="10") && (m_pcdr[ATTRI_CALLED_TYPE_B]!="08") && (m_pcdr[ATTRI_CALLED_TYPE_B]!="15"))
	{
     if (((userHaveUserTypeCode("7")) || (userHaveUserTypeCode("D"))) && (m_pcdr[ATTRI_ROAM_TYPE] == "0") && (m_pcdr[ATTRI_CARRIER_TYPE]=="00") && (m_pcdr[ATTRI_CALL_TYPE]!="20") && (m_pcdr[ATTRI_LONG_TYPE1]!="3"))
     	{
     		  m_pcdr[ATTRI_CDR_CHANNEL]="share";
     		  //m_pcdr[ATTRI_SOURCE_TYPE]="99";
          return 1;
     	}	
	}	
//wujg end 
	
	//IVPN用户过滤到trash
	/*
	 天津的规则
	 
	1.       本地主叫/被叫/呼转话单：剔除各MSC采集的话单，以SCP下发的话单为准（SCP话单均为长号，无短号）。
	PS:原营帐系统将所有呼转话单均识别为无条件呼转，测试时可以看一下
	2.       国内漫游话单：天津为目标网，全触发模式。国内漫游话单已SCP下发为准，剔除总部下发的国内漫游话单；
	3.       国际漫游话单：国际漫游话单无法触发SCP，以总部下发的国际漫游话单为准，进行合帐；
	4.       边界漫游话单：综合VPN用户边界漫游不处理（视为无边界漫游），剔除总部下发边界漫游话单，以SCP回传的话单为准按标准漫游资费批价；
	*/	
	if (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_MSC //GSM端局、关口局话单
	||  m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_CRO //GSM总部下发国内漫游话单
	||  m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_GBO //GSM总部下发省级边界漫游话单
	// ||	m_pcdr[ATTRI_SOURCE_TYPE] == "18"  //如果用户即是IVPN用户又是商务总机的，则过滤IVPN话单 modify by wangzj 20090624
	 )               //商务总机中的IVPN
	{
		if (isIvpnUser())
		{
			m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_TRASH;
			return 1;
		}
	}
	return 0;
}
bool LocalizeProvByNetType::isIvpnUser()
{
  Mdb_UserMemberIvpn         t_po;
  vector<Mdb_UserMemberIvpn> t_rpos;
  int t_iRet=0;

  t_po.m_record.m_memberNumber = m_pcdr[ATTRI_MSISDN_A];
  
  t_iRet=m_mdbMgr.searchMulti<Mdb_UserMemberIvpn>(t_po,t_rpos);
  if(t_iRet<=0)
  {
  	return false;
  }
  for(vector<Mdb_UserMemberIvpn>::iterator t_itr=t_rpos.begin();
      t_itr!=t_rpos.end();++t_itr)
  {
		if (strncmp(t_itr->m_record.m_memberType.c_str(),"40",2) == 0
		&&  t_itr->m_record.m_beginTime <= m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME] 
		&&  t_itr->m_record.m_endTime   >= m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME])
		{
			return true;
		}
  }
  return false;	
}
bool LocalizeProvByNetType::userHaveNetTypeCode(const char * r_net_type_code)
{
  Mdb_UserInfoSN         t_po;
  vector<Mdb_UserInfoSN> t_rpos;
  int t_iRet=0;

  t_po.m_record.m_sn = m_pcdr[ATTRI_MSISDN_A].c_str();
  
  t_iRet=m_mdbMgr.searchMulti<Mdb_UserInfoSN>(t_po,t_rpos);
  if(t_iRet<=0)
  {
  	return false;
  }
  for(vector<Mdb_UserInfoSN>::iterator t_itr=t_rpos.begin();
      t_itr!=t_rpos.end();++t_itr)
  {
		if (strncmp(t_itr->m_record.m_net_type_code.c_str(),r_net_type_code,2) == 0
		//&&  t_itr->m_record.m_openTime <= (m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME]).value() 
		//&&  t_itr->m_record.m_endTime   >= m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME]
		)
		{
			return true;
		}
  }
  return false;		
}

//wujg add 20090531
bool LocalizeProvByNetType::userHaveUserTypeCode(const char * r_user_type_code)
{
  Mdb_UserInfoSN         t_po;
  vector<Mdb_UserInfoSN> t_rpos;
  int t_iRet=0;

  t_po.m_record.m_sn = m_pcdr[ATTRI_MSISDN_A].c_str();
  
  t_iRet=m_mdbMgr.searchMulti<Mdb_UserInfoSN>(t_po,t_rpos);
  if(t_iRet<=0)
  {
  	return false;
  }
  for(vector<Mdb_UserInfoSN>::iterator t_itr=t_rpos.begin();
      t_itr!=t_rpos.end();++t_itr)
  {
		if (strncmp(t_itr->m_record.m_user_type_code.c_str(),r_user_type_code,1) == 0)
		{
			return true;
		}
  }
  return false;		
}
//wujg end 

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeProvRegionInfoGet);

//0不需要求取行政区域信息 1需要求取行政区域信息
int LocalizeProvRegionInfoGet::execute()
{
  if (m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_FIXVOICE ||
  	  m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_FIXSMS ||
  	  m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_NET ||
  	  m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_MISC) //非固网系统直接退出
  {
  }
  else
  {
     return 0;
  }

	m_PO_LocalCdrTypeContainer = (ParamContainer<PO_LocalCdrType>*) PMHelperall::GetParamContainer("PO_LocalCdrType");

  //获取山西固网话单类型
 //cout<<"ATTRI_RECORD_TYPE:"<<m_pcdr[ATTRI_RECORD_TYPE]<<"   "<<__FILE__<<__LINE__<<endl;
  getShanxiFixCdrType(m_pcdr[ATTRI_BIZ_TYPE],m_pcdr[ATTRI_SOURCE_TYPE],m_pcdr[ATTRI_MSC],
                      m_pcdr[ATTRI_TRUNKGROUPIN],m_pcdr[ATTRI_CALL_TYPE], m_pcdr[ATTRI_USER_ASP_B],
                      m_pcdr[ATTRI_USER_SERVICE_TYPE_B],m_pcdr[ATTRI_PA_TYPE],m_pcdr[ATTRI_LONG_TYPE1],
                      m_pcdr[ATTRI_LONG_GROUP1],m_pcdr[ATTRI_CARRIER_TYPE],m_pcdr[ATTRI_HOME_CODE_A],
                      m_pcdr[ATTRI_HOME_CODE_B],m_pcdr[ATTRI_RECORD_TYPE]);
 //cout<<"ATTRI_RECORD_TYPE:"<<m_pcdr[ATTRI_RECORD_TYPE]<<"   "<<__FILE__<<__LINE__<<endl;

  if (m_pcdr[ATTRI_BIZ_TYPE]!=BIZ_TYPE_FIXVOICE) //固网语音求取行政区域信息
  {
      return 0;
  }

  if(m_pcdr[ATTRI_USER_ID].empty())
  	return 1;

  int t_paramid = 1;
  m_globalManager->getDistrictByUserId(m_pcdr[ATTRI_USER_ID].c_str(),t_paramid,
           m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
           m_pcdr[ATTRI_ADM_REGION_A]);

  //B用户标识不为空
  if(m_pcdr[ATTRI_USER_ID_B].empty())
  	return 1;
    m_globalManager->getDistrictByUserId(m_pcdr[ATTRI_USER_ID_B].c_str(),t_paramid,
           m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
           m_pcdr[ATTRI_ADM_REGION_B]);

	return 1;
}

bool LocalizeProvRegionInfoGet::getShanxiFixCdrType(const T_ATTRI_VAL &r_m_bizType,
                       const T_ATTRI_VAL &r_sourceType,
                       const T_ATTRI_VAL &r_m_msc,
                       const T_ATTRI_VAL &m_trunkGroupIn,
                       const T_ATTRI_VAL &r_callType,
                       const T_ATTRI_VAL &r_userAspB,
                       const T_ATTRI_VAL &r_UserServiceTypeB,
                       const T_ATTRI_VAL &r_paType,
                       const T_ATTRI_VAL &r_longType,
                       const T_ATTRI_VAL &r_longGroup,
                       const T_ATTRI_VAL &r_carrierType,
                       const T_ATTRI_VAL &r_homeCodeA,
                       const T_ATTRI_VAL &r_homeCodeB,
                       T_ATTRI_VAL &r_recordType)
{
	PO_LocalCdrType po;
	vector<PO_LocalCdrType*> rpo;
	vector<PO_LocalCdrType*>::iterator itr;
  int nErrorCode=0;

  r_recordType = "9999"; //找不到，默认为9999
	po.recordStruct.m_sourceType = r_sourceType;
  if(m_PO_LocalCdrTypeContainer->searchMulti(po,rpo)<=0)
  {
  	return false;
  }
  ::sort(rpo.begin(),rpo.end(),LocalCdrTypeFixSort());

	for(itr=rpo.begin();itr!=rpo.end();itr++)
	{
    //cout<<"*itr"<<*(*itr)<<"   "<<__FILE__<<__LINE__<<endl;
    if(((*itr)->recordStruct.m_bizType  == r_m_bizType  || (*itr)->recordStruct.m_bizType == "*") &&
       ((*itr)->recordStruct.m_bizType  == r_m_msc      || (*itr)->recordStruct.m_msc == "*") &&
       ((*itr)->recordStruct.m_bizType  == m_trunkGroupIn  || (*itr)->recordStruct.m_trunkGroupIn == "*") &&
       ((*itr)->recordStruct.m_callType == r_callType   || (*itr)->recordStruct.m_callType == "*") &&
       ((*itr)->recordStruct.m_userAspB == r_userAspB   || (*itr)->recordStruct.m_userAspB == "*") &&
       ((*itr)->recordStruct.m_UserServiceTypeB == r_UserServiceTypeB || (*itr)->recordStruct.m_UserServiceTypeB == "*") &&
       ((*itr)->recordStruct.m_paType == r_paType       || (*itr)->recordStruct.m_paType == "*") &&
       ((*itr)->recordStruct.m_longType == r_longType   || (*itr)->recordStruct.m_longType == "*") &&
       ((*itr)->recordStruct.m_longGroup == r_longGroup     || (*itr)->recordStruct.m_longGroup == "*") &&
       ((*itr)->recordStruct.m_carrierType == r_carrierType || (*itr)->recordStruct.m_carrierType == "*") &&
       ((*itr)->recordStruct.m_homeCodeA == r_homeCodeA || (*itr)->recordStruct.m_homeCodeA == "*") &&
       ((*itr)->recordStruct.m_homeCodeB == r_homeCodeB || (*itr)->recordStruct.m_homeCodeB == "*"))
		{
			if((*itr)->recordStruct.m_ruleId.value()==0)
      {
        r_recordType = (*itr)->recordStruct.nRecord_type;
      }
			if(m_globalManager->isConditionValid(m_pcdr, (*itr)->recordStruct.m_ruleId.value(),nErrorCode))
			{
				//cout<<"nErrorCode:"<<nErrorCode<<"   "<<__FILE__<<__LINE__<<endl;
			  r_recordType = (*itr)->recordStruct.nRecord_type;
        return true;
			}
				//cout<<"nErrorCode:"<<nErrorCode<<"   "<<__FILE__<<__LINE__<<endl;
    }    
	}

	return false;
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
/*
  else
  {
     return 0;
  }
*/

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
//cout<<"m_pcdr[ATTRI_VISIT_CODE_A]:"<<m_pcdr[ATTRI_VISIT_CODE_A]<<"  "<<__FILE__<<__LINE__<<endl;
//cout<<"m_pcdr[ATTRI_MSISDN_A]:"<<m_pcdr[ATTRI_MSISDN_A]<<"  "<<__FILE__<<__LINE__<<endl;
//cout<<"r_msisdn_a:"<<r_msisdn_a<<"  "<<__FILE__<<__LINE__<<endl;

  m_pcdr[ATTRI_BILL_FLAG] = BILL_FLAG_NOMAL;

  //是否有号码转换计费
  if(m_globalManager->getBillNumberByPhone(r_msisdn_a,m_pcdr[ATTRI_MSC],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
     m_bill_type2,m_bill_number2,m_pcdr[ATTRI_MISC_STR2]))
  {
     m_pcdr[ATTRI_STATIS_STR2] = BILL_FLAG_2;
     if(m_bill_type2 == "1")
     {
        m_pcdr[ATTRI_MSISDN_A] = m_bill_number2; //填实际计费号码
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

     m_pcdr[ATTRI_BILL_FLAG] = m_pcdr[ATTRI_BILL_FLAG].value() ^ m_pcdr[ATTRI_STATIS_STR3].value()__<<__LINE__<<endl;
//cout<<"m_pcdr[ATTRI_MSISDN_A]:"<<m_pcdr[ATTRI_MSISDN_A]<<"  "<<__FILE__<<__LINE__<<endl;
//cout<<"r_msisdn_a:"<<r_msisdn_a<<"  "<<__FILE__<<__LINE__<<endl;

  m_pcdr[ATTRI_BILL_FLAG] = BILL_FLAG_NOMAL;

  //鏄