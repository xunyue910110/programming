#include "ProvCDRUNProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(ProvCDRUNProc);

bool ProvCDRUNProc::InitCdrProperty()
{
	//如有原先规整的B号码，以规整为准
	if(m_pcdr[ATTRI_MSISDN_B_FOR_ROAM].length()<=0)
	{
		m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B_ORG];
	}
	if(m_pcdr[ATTRI_MSISDN_B].length()<=0)
	{
		m_pcdr[ATTRI_MSISDN_B]          = m_pcdr[ATTRI_MSISDN_B_FOR_ROAM];
	}
	
	return true;
}

bool ProvCDRUNProc::getTrash()
{
	int t_iRet;
	//本地化 scpTrash判断
	t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeProvTrashGet");
	if(t_iRet==0)
	{
		return false;
	}
	else if(t_iRet==1)
	{
		m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_TRASH;
		return true;
	}
	else if(t_iRet==9)
	{
		return true;
	}
	
	return false;
}

bool ProvCDRUNProc::getscpTrash()
{
	int t_iRet;
	//本地化 scpTrash判断
	t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeProvScpTrashGet");
	if(t_iRet==0)
	{
		return false;
	}
	else if(t_iRet==1)
	{
		m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_SCP;
		return true;
	}
	
	return false;
}

bool ProvCDRUNProc::getDisChannelProperty()
{
	int t_iRet;
	//1. 无效话单
	if(getTrash())
	{
		return true;
	}
	
	//2. SCP无效话单
	if(getscpTrash())
	{
		return true;
	}
	
	//3. 错误话单
	if(getError())
	{
		m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_ERROR;
		return true;
	}
	
    switch(m_FilterInfo->m_bindMethod)
    {
		//根据号码绑定资料
	case BIND_BY_MSISDN:
		if (!getChannelByBindPhone())
			return false;
		break;
		//根据IMSI绑定资料
	case BIND_BY_IMSI:
		if (!getChannelByBindImsi())
			return false;
		break;
		//先根据号码，找不到再根据IMSI
	case BIND_BY_MSISDN_IMSI:
		if (!getChannelByBindPhoneImsi())
			return false;
		break;
		//根据号码和接入号等绑定资料
	case BIND_BY_TRUNK:
		if (!getChannelByBindTrunk())
			return false;
		break;
		//根据USERID绑定资料 SP包月类话单
	case BIND_BY_USERID:
		if (!getChannelByBindUserId())
			return false;
		break;
		//根据号码 网别绑定资料 IP话单
	case BIND_BY_MSISDN_NETTYPE:
		if (!getChannelByBindMsisdnNetType())
			return false;
		break;
	default:
		if (!getChannelByBindPhone())
			return false;
		break;
    }
	
	t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeProvRegionInfoGet");
	if(t_iRet==2)
	{
		m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_ERROR;
		return true;
	}
	
    m_FilterInfo->m_acctTag = m_userServInfo.m_acctTag;
	//本地化预开卡特殊处理
	t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeProvCdrGet");
	if(t_iRet==1)
	{
		m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_ERROR;
		return true;
	}
	//本地化根据网别编码挑单
	t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeProvByNetType");
	if(t_iRet==1)
	{
		return true;
	}
	
	//本地化特殊分通道
	t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeChannelReset");
	if(t_iRet==1)
	{
		return true;
	}
	
	//cout<<"m_pcdr[ATTRI_NET_TYPE_CODE]:"<<m_pcdr[ATTRI_NET_TYPE_CODE]<<__FILE__<<__LINE__<<endl;
	//cout<<"m_pcdr[ATTRI_CDR_CHANNEL]:"<<m_pcdr[ATTRI_CDR_CHANNEL]<<__FILE__<<__LINE__<<endl;
	
	return true;
}


bool ProvCDRUNProc::getChannelByBindPhone()
{
	int t_ret,t_error =0;
	char r_temp[5];
	if(m_pcdr[ATTRI_MSISDN_A].length()<=0)
	{
		//无主话单通道
		m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_NOINFO;
		return true;
	}
	t_ret = m_globalManager->getUserServInfoByMsisdn(m_pcdr[ATTRI_SOURCE_TYPE].c_str(),m_pcdr[ATTRI_MSISDN_A].c_str(),
		m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_END_DATE] + m_pcdr[ATTRI_END_TIME],
		m_userServInfo,t_error);
	
	if(t_ret>0)
	{
		//省内通道号 个位补0
		sprintf(r_temp,"%02d",m_userServInfo.m_channelNo);
		m_pcdr[ATTRI_CDR_CHANNEL]=r_temp;
		bindOtherInfo();
		bindInfo(m_userServInfo,m_otherServInfo);
		replaceMsisdn(m_userServInfo);
	}
	else
	{
		m_otherServInfo.reset();
		bindInfo(m_userServInfo,m_otherServInfo);
		//无主话单通道
		m_pcdr[ATTRI_ERROR_CODE]  = t_error;
		m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_NOINFO;
	}
	
	return true;
}

bool ProvCDRUNProc::getChannelByBindImsi()
{
	int t_ret,t_error =0;
	char r_temp[5];
	if(m_pcdr[ATTRI_IMSI_NUMBER].length()<=0)
	{
		//无主话单通道
		m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_NOINFO;
		return true;
	}
	t_ret = m_globalManager->getUserServInfoByImsi(m_pcdr[ATTRI_SOURCE_TYPE].c_str(),m_pcdr[ATTRI_IMSI_NUMBER].c_str(),
		m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_END_DATE] + m_pcdr[ATTRI_END_TIME],
		m_userServInfo,t_error);
	
	if(t_ret>0)
	{
		//省内通道号 个位补0
		sprintf(r_temp,"%02d",m_userServInfo.m_channelNo);
		m_pcdr[ATTRI_CDR_CHANNEL]=r_temp;
		bindOtherInfo();
		bindInfo(m_userServInfo,m_otherServInfo);
		replaceMsisdn(m_userServInfo);
	}
	else
	{
		m_otherServInfo.reset();
		bindInfo(m_userServInfo,m_otherServInfo);
		//无主话单通道
		m_pcdr[ATTRI_ERROR_CODE]  = t_error;
		m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_NOINFO;
	}
	
	return true;
}

bool ProvCDRUNProc::getChannelByBindPhoneImsi()
{
	int t_ret,t_error =0;
	char r_temp[5];
	if(m_pcdr[ATTRI_MSISDN_A].length()<=0)
	{
		//如果计费号码为空，通过IMSI查找
		getChannelByBindImsi();
		return true;
	}
	
	t_ret = m_globalManager->getUserServInfoByMsisdn(m_pcdr[ATTRI_SOURCE_TYPE].c_str(),m_pcdr[ATTRI_MSISDN_A].c_str(),
		m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_END_DATE] + m_pcdr[ATTRI_END_TIME],
		m_userServInfo,t_error);
	
	if(t_ret>0)
	{
		//省内通道号 个位补0
		sprintf(r_temp,"%02d",m_userServInfo.m_channelNo);
		m_pcdr[ATTRI_CDR_CHANNEL]=r_temp;
		bindOtherInfo();
		bindInfo(m_userServInfo,m_otherServInfo);
		replaceMsisdn(m_userServInfo);
		return true;
	}
	else
	{
		if(m_pcdr[ATTRI_IMSI_NUMBER].length()<=0)
		{
			//无主话单通道
			//add by dph
			m_pcdr[ATTRI_ERROR_CODE]  = t_error;
			//end of add by dph
			m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_NOINFO;
			return true;
		}
		t_ret = m_globalManager->getUserServInfoByImsi(m_pcdr[ATTRI_SOURCE_TYPE].c_str(),m_pcdr[ATTRI_IMSI_NUMBER].c_str(),
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_END_DATE] + m_pcdr[ATTRI_END_TIME],
			m_userServInfo,t_error);
		
		if(t_ret>0)
		{
			//省内通道号 个位补0
			sprintf(r_temp,"%02d",m_userServInfo.m_channelNo);
			m_pcdr[ATTRI_CDR_CHANNEL]=r_temp;
			bindOtherInfo();
			bindInfo(m_userServInfo,m_otherServInfo);
			replaceMsisdn(m_userServInfo);
			return true;
		}
		else
		{
			m_otherServInfo.reset();
			bindInfo(m_userServInfo,m_otherServInfo);
			//无主话单通道
			m_pcdr[ATTRI_ERROR_CODE]  = t_error;
			m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_NOINFO;
			return true;
		}
	}
	
	return true;
}

bool ProvCDRUNProc::getChannelByBindTrunk()
{
	int t_ret,t_error =0;
	char r_temp[5];
	if(m_pcdr[ATTRI_MSISDN_A].length()<=0)
	{
		//无主话单通道
		m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_NOINFO;
		return true;
	}
	
	if(m_pcdr[ATTRI_TRUNKGROUPIN].length()>=0)
	{
		t_ret = m_globalManager->getUserServInfoByMsisdnandImsi(m_pcdr[ATTRI_SOURCE_TYPE].c_str(),
			m_pcdr[ATTRI_MSISDN_A].c_str(),m_pcdr[ATTRI_TRUNKGROUPIN].c_str(),
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_END_DATE] + m_pcdr[ATTRI_END_TIME],
			m_userServInfo,t_error);
		if(t_ret>0)
		{
			//省内通道号 个位补0
			sprintf(r_temp,"%02d",m_userServInfo.m_channelNo);
			m_pcdr[ATTRI_CDR_CHANNEL]=r_temp;
			bindOtherInfo();
			bindInfo(m_userServInfo,m_otherServInfo);
			replaceMsisdn(m_userServInfo);
		}
		else
		{
			m_otherServInfo.reset();
			bindInfo(m_userServInfo,m_otherServInfo);
			//无主话单通道
			m_pcdr[ATTRI_ERROR_CODE]  = t_error;
			m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_NOINFO;
		}
	}
	else if(m_pcdr[ATTRI_TRUNKGROUPOUT].length()>=0)
	{
		t_ret = m_globalManager->getUserServInfoByMsisdnandImsi(m_pcdr[ATTRI_SOURCE_TYPE].c_str(),
			m_pcdr[ATTRI_MSISDN_A].c_str(),m_pcdr[ATTRI_TRUNKGROUPOUT].c_str(),
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_END_DATE] + m_pcdr[ATTRI_END_TIME],
			m_userServInfo,t_error);
		if(t_ret>0)
		{
			//省内通道号 个位补0
			sprintf(r_temp,"%02d",m_userServInfo.m_channelNo);
			m_pcdr[ATTRI_CDR_CHANNEL]=r_temp;
			bindOtherInfo();
			bindInfo(m_userServInfo,m_otherServInfo);
			replaceMsisdn(m_userServInfo);
		}
		else
		{
			//无主话单通道
			m_pcdr[ATTRI_ERROR_CODE]  = t_error;
			m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_NOINFO;
		}
	}
	else
	{
		m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_NOINFO;
	}
	
	return true;
}

bool ProvCDRUNProc::getChannelByBindUserId()
{
	int t_ret,t_error =0;
	char r_temp[5];
	if(m_pcdr[ATTRI_USER_ID].length()<=0)
	{
		//无主话单通道
		m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_NOINFO;
		return true;
	}
	m_userServInfo.m_msisdn     = m_pcdr[ATTRI_MSISDN_A];
	m_userServInfo.m_brand_code = m_pcdr[ATTRI_USER_SERVICE_TYPE_A_YZ];
	m_userServInfo.m_inTime     = m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME];
	m_userServInfo.m_userId     = m_pcdr[ATTRI_USER_ID].value();
	t_ret = m_globalManager->getUserServInfo(m_userServInfo.m_userId,m_userServInfo,t_error);
	
	if(t_ret>0)
	{
		//省内通道号 个位补0
		sprintf(r_temp,"%02d",m_userServInfo.m_channelNo);
		m_pcdr[ATTRI_CDR_CHANNEL]=r_temp;
		bindOtherInfo();
		bindInfo(m_userServInfo,m_otherServInfo);
		replaceMsisdn(m_userServInfo);
	}
	else
	{
		//无主话单通道
		m_pcdr[ATTRI_ERROR_CODE]  = t_error;
		m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_NOINFO;
	}
	
	return true;
}

bool ProvCDRUNProc::getChannelByBindMsisdnNetType()
{
	int t_ret,t_error =0;
	char r_temp[5];
	if(m_pcdr[ATTRI_MSISDN_A].length()<=0)
	{
		//无主话单通道
		m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_NOINFO;
		return true;
	}
	t_ret = m_globalManager->getUserServInfoByMsisdnandNetType(m_pcdr[ATTRI_IP_SERV_TYPE_A],m_pcdr[ATTRI_MSISDN_A],
		m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_END_DATE] + m_pcdr[ATTRI_END_TIME],
		m_userServInfo,t_error);
	
	if(t_ret>0)
	{
		//省内通道号 个位补0
		sprintf(r_temp,"%02d",m_userServInfo.m_channelNo);
		m_pcdr[ATTRI_CDR_CHANNEL]=r_temp;
		bindOtherInfo();
		bindInfo(m_userServInfo,m_otherServInfo);
		replaceMsisdn(m_userServInfo);
	}
	else
	{
		//无主话单通道
		m_pcdr[ATTRI_ERROR_CODE]  = t_error;
		m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_NOINFO;
	}
	
	return true;
}


void ProvCDRUNProc::bindOtherInfo()
{
	int t_ret,t_error =0;
	T_ATTRI_VAL t_phonenumber;  
	m_otherServInfo.reset();
	
	if(m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_GSM || m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_SMS || m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_UNICOMIP ||
		m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_FIXVOICE || m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_FIXSMS)
	{
	}
	else
	{
		//其余业务类型直接返回
		return;
	}
	
	//本省用户才需要求对端资料
	if(isSelfMobile(t_phonenumber) && m_pcdr[ATTRI_USER_HOME_TYPE_B].in(AREA_TYPE_LOCAL,AREA_TYPE_PROVINCE))
	{
		t_ret = m_globalManager->getUserServInfoByMsisdn(m_pcdr[ATTRI_SOURCE_TYPE].c_str(),t_phonenumber.c_str(),
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_END_DATE] + m_pcdr[ATTRI_END_TIME],
			m_otherServInfo,t_error);
	}
	else if((m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_FIXVOICE || m_pcdr[ATTRI_BIZ_TYPE]==BIZ_TYPE_FIXSMS) && m_pcdr[ATTRI_USER_HOME_TYPE_B].in(AREA_TYPE_LOCAL,AREA_TYPE_PROVINCE))
	{
		t_ret = m_globalManager->getUserServInfoByMsisdn(m_pcdr[ATTRI_SOURCE_TYPE].c_str(),t_phonenumber.c_str(),
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_END_DATE] + m_pcdr[ATTRI_END_TIME],
			m_otherServInfo,t_error);
		if(t_ret<=0)
		{
			t_ret = m_globalManager->getUserServInfoByImsi(m_pcdr[ATTRI_SOURCE_TYPE].c_str(),t_phonenumber.c_str(),
				m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_END_DATE] + m_pcdr[ATTRI_END_TIME],
				m_otherServInfo,t_error);
		}
	}
}

void ProvCDRUNProc::bindInfo(const UserServInfo &r_userServInfo, const UserServInfo &m_otherServInfo)
{
	m_pcdr[ATTRI_USER_ID]                = r_userServInfo.m_userId;
	m_pcdr[ATTRI_CUST_ID]                = r_userServInfo.m_custId;
	m_pcdr[ATTRI_PRODUCT_ID_A]           = r_userServInfo.m_productId;
	m_pcdr[ATTRI_OFFICE_CODE]            = r_userServInfo.m_office_code;
	m_pcdr[ATTRI_USER_SERVICE_TYPE_A_YZ] = r_userServInfo.m_brand_code;
	m_pcdr[ATTRI_DOUBLEMODE]             = r_userServInfo.m_doubleMode;
	m_pcdr[ATTRI_CHANNEL_NO]             = r_userServInfo.m_channelNo;
	
	m_pcdr[ATTRI_USER_ID_B]              = m_otherServInfo.m_userId;
	m_pcdr[ATTRI_PRODUCT_ID_B]           = m_otherServInfo.m_productId;
	m_pcdr[ATTRI_USER_BRAND_TYPE_B]      = m_otherServInfo.m_brand_code;
	m_pcdr[ATTRI_OFFICE_CODE_B]          = m_otherServInfo.m_office_code;
	m_pcdr[ATTRI_OPEN_DATE]              = r_userServInfo.m_openTime;
	
	m_pcdr[ATTRI_NET_TYPE_CODE]          = m_userServInfo.m_net_type_code;
	m_pcdr[ATTRI_EPARCHY_CODE]           = m_userServInfo.m_eparchy_code;
	m_pcdr[ATTRI_PREPAY_FLAG]            = m_userServInfo.m_prepayTag;//add by shenglj 2009-10-06 20:19:01 获取预付费标志
	m_pcdr[ATTRI_CRMUSER_TYPE_CODE]      = m_userServInfo.m_userTypeCode; //add by shenglj 2009-11-28 16:08:01 获取用户类型
	
	m_pcdr[ATTRI_LOGIC_PHONE_A]          = r_userServInfo.m_msisdn;   //add by shenglj 2009-10-27 22:25:01 获取A逻辑号码
	m_pcdr[ATTRI_PHYCRM_PHONE_A]         = r_userServInfo.m_imsi;     //add by shenglj 2009-10-27 22:25:01 获取A物理号码
	m_pcdr[ATTRI_LOGIC_PHONE_B]          = m_otherServInfo.m_msisdn;  //add by shenglj 2009-10-27 22:25:01 获取B逻辑号码
	m_pcdr[ATTRI_PHYCRM_PHONE_B]         = m_otherServInfo.m_imsi;    //add by shenglj 2009-10-27 22:25:01 获取B物理号码
	
}

void ProvCDRUNProc::replaceMsisdn(const UserServInfo &r_userServInfo)
{
	if(m_pcdr[ATTRI_MSISDN_A].length()==0)
		m_pcdr[ATTRI_MSISDN_A] = r_userServInfo.m_msisdn;
	if(m_pcdr[ATTRI_IMSI_NUMBER].length()==0)
		m_pcdr[ATTRI_IMSI_NUMBER]=r_userServInfo.m_imsi;
}

int ProvCDRUNProc::execute()
{
#ifdef FP_DEBUG
	FP_BEGIN(ProvCDRUNProc)
#endif
		
		InitCdrProperty();
	
	getDisChannelProperty();
	
	//cout<<"m_pcdr[ATTRI_CDR_CHANNEL]  = "<<m_pcdr[ATTRI_CDR_CHANNEL]<<endl;
	
	//add by shenglj 2009-08-20 15:12:01 根据JS正则重求通道
	getChanByRule();
	
#ifdef FP_DEBUG
	FP_END(ProvCDRUNProc)
#endif
		return 0;
}




