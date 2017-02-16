#include "SmsAttribUNProc.h"

#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(SmsAttribUNProc);

int SmsAttribUNProc::execute()
{
 #ifdef FP_DEBUG
   FP_BEGIN(SmsAttribUNProc)
 #endif

  //无效话单直接返回
  if(m_pcdr[ATTRI_TRASH_FLAG]==ATTRI_TRASH_Y)
	{
		  m_FilterInfo->m_unique_flag = FLAG_NO;
     return 0;
	}

	//错单,不进行处理
	if (!isValidCDR())
	{
		return 1;
	}

	//求A的归属
  if(m_FilterInfo->m_smsMethod == FIX_SM_PROCESS_METHOD ||
  	 m_FilterInfo->m_smsMethod == FIX_SP_PROCESS_METHOD)
  {
  	if(!getFixSmAHByMsisdn())
  	{
		  return 1;
  	}
  }
	else if(!getSmAHByMsisdn())
	{
		return 1;
	}

  //求A的省内通道号
  if(m_pcdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_LOCAL)
  {
     m_globalManager->getProvChannelByCitYCode(m_pcdr[ATTRI_HOME_CODE_A],m_channelNo);
     m_pcdr[ATTRI_CITY_CHANNEL] = m_channelNo;
  }

  //求B的归属
  if(m_FilterInfo->m_smsMethod == SM_PROCESS_METHOD ||
  	 m_FilterInfo->m_smsMethod == FIX_SM_PROCESS_METHOD ||
  	 m_FilterInfo->m_smsMethod == FIX_SP_PROCESS_METHOD) // for nmg fixsp  at 091217
  {
	  getSmBHByMsisdn();
  }

  setSmRoamType();

	//设置话单是否有原始费用信息标志
	ProcessBaseHelper::ExecuteProcedure("SetOriginFeeFlag");

 #ifdef FP_DEBUG
   FP_END(SmsAttribUNProc)
 #endif
  return 0;
}


bool SmsAttribUNProc::getSmAHByMsisdn()
{
	T_ATTRI_VAL groupATmp;
  //集团用户不查号码段
  if(m_pcdr[ATTRI_USER_SERVICE_TYPE_A]==SM_SERV_TYPE_VPN ||
     m_pcdr[ATTRI_USER_SERVICE_TYPE_A]==SM_SERV_TYPE_0C ||
     m_pcdr[ATTRI_USER_SERVICE_TYPE_A]==SM_SERV_TYPE_0F ||
     m_pcdr[ATTRI_USER_SERVICE_TYPE_A]==SM_SERV_TYPE_0D ||
     m_pcdr[ATTRI_USER_SERVICE_TYPE_A]==SM_SERV_TYPE_0E ||
     m_pcdr[ATTRI_USER_SERVICE_TYPE_A]==SM_SERV_TYPE_HY)
  {
      m_pcdr[ATTRI_SM_USER_TYPE]      = USER_SM_TYPE_VPN;
      m_pcdr[ATTRI_USER_HOME_TYPE_A ] = AREA_TYPE_LOCAL;
      return true;
  }

	//求A归属地
	//1. 本运营商手机号码
	if(isSelfNumber(m_pcdr[ATTRI_MSISDN_A]))
	{
		if (!getAHByMsisdn(m_pcdr[ATTRI_MSISDN_A]))
		{
			return false;
		}

		if(m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == SERVICE_TYPE_SSP)
		{
			 m_pcdr[ATTRI_SM_USER_TYPE]=USER_SM_TYPE_SZX;
		}
		else
		{
			 m_pcdr[ATTRI_SM_USER_TYPE]=USER_SM_TYPE_QQT;
		}
	}
	//2. 非本运营商手机号码
	else if(isOtherNumber(m_pcdr[ATTRI_MSISDN_A]))
	{
		if (!getAHByMsisdn(m_pcdr[ATTRI_MSISDN_A]))
		{
			return false;
		}
		if(m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == SERVICE_TYPE_SSP)
		{
			 m_pcdr[ATTRI_SM_USER_TYPE]=USER_SM_TYPE_SZX;
		}
		else
		{
			 m_pcdr[ATTRI_SM_USER_TYPE]=USER_SM_TYPE_QQT;
		}
	}
	//3. 小灵通号码
   else if(strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(), "0", 1) == 0
   	  && m_globalManager->getHomeAreaCodeByMsisdnN(m_pcdr[ATTRI_MSISDN_A],
   	          m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_HOME_CODE_A],groupATmp,m_pcdr[ATTRI_USER_TYPE_A]))
   {
   	  //计费方出现固网号码，直接判为无效话单
   	  m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SM_SERV_TYPE_FIX;
   	  m_pcdr[ATTRI_USER_HOME_TYPE_A]    = m_pcdr[ATTRI_USER_TYPE_A][3];
	    m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
	    return true;
   }

  return true;
}

bool SmsAttribUNProc::getFixSmAHByMsisdn()
{
  int rd;
  T_ATTRI_VAL long_group;
  T_ATTRI_VAL calledType;
  rd = m_globalManager->getHomeAreaCodeByFixPhone(m_pcdr[ATTRI_MSISDN_A],
         m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
         m_pcdr[ATTRI_EXCHANGECODE_A],m_pcdr[ATTRI_SUB_HOMECODE_A],m_pcdr[ATTRI_HOME_CODE_A],long_group,
         m_pcdr[ATTRI_USER_TYPE_A],m_pcdr[ATTRI_OFFICE_CODE],
         calledType);
   if(rd>0)
   {
      if (m_pcdr[ATTRI_USER_TYPE_A].length() != USER_TYPE_LENGTH)
      {
          m_pcdr[ATTRI_ERROR_CODE] = AH_MSISDNN_ERROR;
          return false;
      }

      m_pcdr[ATTRI_USER_ASP_A]  = m_pcdr[ATTRI_USER_TYPE_A][0];
      m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_USER_TYPE_A].substr(1,2);
      m_pcdr[ATTRI_USER_HOME_TYPE_A]    = m_pcdr[ATTRI_USER_TYPE_A][3];
      return true;
   }
   //固网号段找不到
   m_pcdr[ATTRI_ERROR_CODE] = AH_FIX_MSISDN_ERROR;
   return false;
}

bool SmsAttribUNProc::getSmBHByMsisdn()
{
	T_ATTRI_VAL groupATmp;
  T_ATTRI_VAL officeATmp;
	T_ATTRI_VAL calledType;

  T_ATTRI_VAL long_group;
  T_ATTRI_VAL long_group2;
  T_ATTRI_VAL long_group3;
  T_ATTRI_VAL long_group4;
  T_ATTRI_VAL long_groupgprs;

	int rd=0;

  //国际互通短信
  if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_IINTER)
  {
        if (m_pcdr[ATTRI_HOME_CODE_B].length()>0 &&
        	  m_pcdr[ATTRI_HOME_CODE_B].compare(0,2,"00")!=0)
        {
            m_pcdr[ATTRI_HOME_CODE_B]="00" + m_pcdr[ATTRI_HOME_CODE_B];
        }
        else
        {
		       rd = m_globalManager->getHomeAreaCodeByMsisdnI(m_pcdr[ATTRI_MSISDN_B],
		        	    m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
		        	    m_pcdr[ATTRI_HOME_CODE_B],
		        	    long_group,long_group2,long_group3,long_group4,long_groupgprs,
		        	    m_pcdr[ATTRI_USER_TYPE_B],
		        	    m_pcdr[ATTRI_NATIONAL_CODE_B]);
		       if(rd>0)
		       {
	       	 if(m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
	         {
				      m_pcdr[ATTRI_ERROR_CODE] = BH_MSISDNI_ERROR;
				      return false;
			     }
		       	m_pcdr[ATTRI_USER_HOME_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B][3];
		       }
        }
  }
	//短信中心 移动互通网关 联通互通网关 G网PPS点对点短信
  else
  {
		 if(isSelfNumber(m_pcdr[ATTRI_MSISDN_B]))
		 {
		 	 getBHByMsisdn(m_pcdr[ATTRI_MSISDN_B]);
		 }
		 else if(isOtherNumber(m_pcdr[ATTRI_MSISDN_B]))
		 {
       rd = m_globalManager->getHomeAreaCodeByMsisdnO(m_pcdr[ATTRI_MSISDN_B],
              m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
              m_pcdr[ATTRI_HOME_CODE_B],
              groupATmp,
              m_pcdr[ATTRI_USER_TYPE_B],
              calledType);
       if(rd>0)
       {
       	if(m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
        {
			     m_pcdr[ATTRI_ERROR_CODE] = BH_MSISDNO_ERROR;
			     return false;
		    }
		    m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
       	m_pcdr[ATTRI_USER_HOME_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B][3];
       }
	   }
     else if(m_pcdr[ATTRI_USER_SERVICE_TYPE_B]==SM_SERV_TYPE_FIX &&
     	       strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "0", 1) == 0)
     {
       rd = m_globalManager->getHomeAreaCodeByMsisdnN(m_pcdr[ATTRI_MSISDN_B],
        	    m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
        	    m_pcdr[ATTRI_HOME_CODE_B],
        	    groupATmp,
        	    m_pcdr[ATTRI_USER_TYPE_B]);
       if(rd>0)
       {
       	if(m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
        {
			     m_pcdr[ATTRI_ERROR_CODE] = BH_MSISDNN_ERROR;
			     return false;
		    }
		    m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
       	m_pcdr[ATTRI_USER_HOME_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B][3];
       }
     }
  }


  //没有求到对方归属，置HOME_AREA_CODE
  if (m_pcdr[ATTRI_HOME_CODE_B].length()==0)
  {
      m_pcdr[ATTRI_HOME_CODE_B]=m_pcdr[ATTRI_HOME_CODE_A];
  }

  return true;
}

void SmsAttribUNProc::setSmRoamType()
{
  if (m_pcdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_LOCAL)
  {
      m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NONE; //非漫游 - 0
  }
  else if (m_pcdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_NATIONAL)
  {
      m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NATIONAL_IN;  //国内来访　- 3
  }
  else
  {
      m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_INTERNATIONAL_IN; //国际来访　-5
  }
}
