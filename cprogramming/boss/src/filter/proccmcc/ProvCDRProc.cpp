#include "ProvCDRProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(ProvCDRProc);

bool ProvCDRProc::InitCdrProperty()
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

bool ProvCDRProc::getTrash()
{
  //剔除无效话单
  if(m_pcdr[ATTRI_CALL_TYPE][0]=='F')
      return true;
  else
      return false;
}

bool ProvCDRProc::getDisChannelProperty()
{
	//1. 无效话单
	if(getTrash())
	{
		m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_TRASH;
		return true;
	}

	//2. 错误话单
	if(getError())
	{
		m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_ERROR;
		return true;
	}

    //由于深圳中心处理I文件时以话单中的IMSI为标准，I文件的处理方式，以IMSI为标准判断MSISDN
    if (m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GSM_ROI)
    {
        m_FilterInfo->m_bindMethod=BIND_BY_IMSI;
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
     default:
           if (!getChannelByBindPhone())
              return false;
           break;
    }

  return true;
}


bool ProvCDRProc::getChannelByBindPhone()
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
 		 //无主话单通道
		 m_pcdr[ATTRI_ERROR_CODE]  = t_error;
		 m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_NOINFO;
 	}

  return true;
}

bool ProvCDRProc::getChannelByBindImsi()
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
 		 //无主话单通道
		 m_pcdr[ATTRI_ERROR_CODE]  = t_error;
		 m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_NOINFO;
 	}

  return true;
}

bool ProvCDRProc::getChannelByBindPhoneImsi()
{
  if(isSelfMobile(m_pcdr[ATTRI_MSISDN_A]) && m_pcdr[ATTRI_MSISDN_A].length()==11)
  {
  	 getChannelByBindPhone();
  }
  else
  {
  	 getChannelByBindImsi();
  }

  return true;
}

bool ProvCDRProc::getChannelByBindTrunk()
{
  int t_ret,t_error =0;
  char r_temp[5];
  if(m_pcdr[ATTRI_MSISDN_A].length()<=0)
  {
  	 //无主话单通道
     m_pcdr[ATTRI_CDR_CHANNEL]=CHANNEL_POCNOINFO;
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
	 		 //无主话单通道
			 m_pcdr[ATTRI_ERROR_CODE]  = t_error;
			 m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_POCNOINFO;
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
			 m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_POCNOINFO;
	 	}
  }
  else
  {
			m_pcdr[ATTRI_CDR_CHANNEL] = CHANNEL_POCNOINFO;
  }

  return true;
}

void ProvCDRProc::bindOtherInfo()
{
  int t_ret,t_error =0;
  T_ATTRI_VAL t_phonenumber;
  
  m_otherServInfo.reset();
  if(m_pcdr[ATTRI_MSISDN_B].compare(0,3,"861")==0 && m_pcdr[ATTRI_MSISDN_B].length()==13)
  {
  	t_phonenumber = m_pcdr[ATTRI_MSISDN_B].substr(2,m_pcdr[ATTRI_MSISDN_B].length()-2);
  }
  //对方号码以接入号开头,接入号后只判断一位是否是1,目前接入号只考虑179和12593 2007-4-4 16:03
  else if((m_pcdr[ATTRI_MSISDN_B].compare(0,3,"179")==0 || m_pcdr[ATTRI_MSISDN_B].compare(0,5,"12593")==0) && 
           m_pcdr[ATTRI_MSISDN_B].length()==16 && m_pcdr[ATTRI_MSISDN_B].compare(5,1,"1")==0)
  {
  	t_phonenumber = m_pcdr[ATTRI_MSISDN_B].substr(5,m_pcdr[ATTRI_MSISDN_B].length()-5);
  }
  else if(m_pcdr[ATTRI_MSISDN_B].compare(0,3,"179")==0 && 
          m_pcdr[ATTRI_MSISDN_B].length()==17 && m_pcdr[ATTRI_MSISDN_B].compare(5,2,"01")==0)
  {
  	t_phonenumber = m_pcdr[ATTRI_MSISDN_B].substr(6,m_pcdr[ATTRI_MSISDN_B].length()-6);
  }
  else
  {
  	t_phonenumber = m_pcdr[ATTRI_MSISDN_B];
  }

  if(isSelfMobile(t_phonenumber))
  {
    t_ret = m_globalManager->getUserServInfoByMsisdn(m_pcdr[ATTRI_SOURCE_TYPE].c_str(),t_phonenumber.c_str(),
               m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_END_DATE] + m_pcdr[ATTRI_END_TIME],
               m_otherServInfo,t_error);
  }
}

void ProvCDRProc::bindInfo(const UserServInfo &r_userServInfo, const UserServInfo &m_otherServInfo)
{
	m_pcdr[ATTRI_USER_ID]                = r_userServInfo.m_userId;
	m_pcdr[ATTRI_CUST_ID]                = r_userServInfo.m_custId;
	m_pcdr[ATTRI_PRODUCT_ID_A]           = r_userServInfo.m_productId;
	m_pcdr[ATTRI_OFFICE_CODE]            = r_userServInfo.m_office_code;
	m_pcdr[ATTRI_USER_SERVICE_TYPE_A_YZ] = r_userServInfo.m_brand_code;
	m_pcdr[ATTRI_DOUBLEMODE]             = "";
	m_pcdr[ATTRI_CHANNEL_NO]             = r_userServInfo.m_channelNo;

	m_pcdr[ATTRI_PRODUCT_ID_B]           = m_otherServInfo.m_productId;
	m_pcdr[ATTRI_USER_BRAND_TYPE_B]      = m_otherServInfo.m_brand_code;
	m_pcdr[ATTRI_OFFICE_CODE_B]          = m_otherServInfo.m_office_code;
	m_pcdr[ATTRI_OPEN_DATE]              = r_userServInfo.m_openTime;

}

void ProvCDRProc::replaceMsisdn(const UserServInfo &r_userServInfo)
{
	if(!isMobile(m_pcdr[ATTRI_MSISDN_A]) || m_pcdr[ATTRI_MSISDN_A].length()!=11) 
      m_pcdr[ATTRI_MSISDN_A] = r_userServInfo.m_msisdn;
  if(m_pcdr[ATTRI_IMSI_NUMBER].length()==0)
      m_pcdr[ATTRI_IMSI_NUMBER]=r_userServInfo.m_imsi;
}

int ProvCDRProc::execute()
{
	InitCdrProperty();

	getDisChannelProperty();
	
	//cout<<"m_pcdr[ATTRI_CDR_CHANNEL]  = "<<m_pcdr[ATTRI_CDR_CHANNEL]<<endl;

  return 0;
}
