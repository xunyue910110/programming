#include "SmsAttribProc.h"

#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(SmsAttribProc);

int SmsAttribProc::execute()
{
	//错单,不进行处理
	if (!isValidCDR())
	{
		return 1;
	}
	
	getSmAHByMsisdn();
	
	getSmBHByMsisdn();


	//设置话单是否有原始费用信息标志
	ProcessBaseHelper::ExecuteProcedure("SetOriginFeeFlag");
	
  return 0;
}


bool SmsAttribProc::getSmAHByMsisdn()
{
  //集团用户不查号码段
  if(m_pcdr[ATTRI_SM_SERV_TYPE_A]==SM_SERV_TYPE_VPN ||
     m_pcdr[ATTRI_SM_SERV_TYPE_A]==SM_SERV_TYPE_0C ||
     m_pcdr[ATTRI_SM_SERV_TYPE_A]==SM_SERV_TYPE_0F ||
     m_pcdr[ATTRI_SM_SERV_TYPE_A]==SM_SERV_TYPE_0D ||
     m_pcdr[ATTRI_SM_SERV_TYPE_A]==SM_SERV_TYPE_0E ||
     m_pcdr[ATTRI_SM_SERV_TYPE_A]==SM_SERV_TYPE_HY)
  {
      m_pcdr[ATTRI_SM_USER_TYPE]      = USER_SM_TYPE_VPN;
      m_pcdr[ATTRI_USER_HOME_TYPE_A ] = AREA_TYPE_LOCAL;
      return true;
  }

	//Added by Sunam 2006-2-21 20:10 求A归属地
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


  return true;
}

bool SmsAttribProc::getSmBHByMsisdn()
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
	//短信中心
  if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_SMSC)
  {
		 if(isSelfNumber(m_pcdr[ATTRI_SERVICE_CODE]))
		 {
		 	 getBHByMsisdn(m_pcdr[ATTRI_SERVICE_CODE]);
		 }
  }
  //互通网关
  else if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_ISMG)
  {
		 if(isOtherNumber(m_pcdr[ATTRI_SERVICE_CODE]))
		 {
       rd = m_globalManager->getHomeAreaCodeByMsisdnO(m_pcdr[ATTRI_SERVICE_CODE],
              m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
              m_pcdr[ATTRI_HOME_CODE_B],
              groupATmp,
              m_pcdr[ATTRI_USER_TYPE_B],
              calledType);
       if(rd>=0)
       {
       	m_pcdr[ATTRI_USER_HOME_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B][3];
       }
	   }
     else if(m_pcdr[ATTRI_SM_SERV_TYPE_B]==SM_SERV_TYPE_FIX && m_pcdr[ATTRI_SM_SERV_TYPE_B][0]=='0')
     {
       rd = m_globalManager->getHomeAreaCodeByMsisdnN(m_pcdr[ATTRI_SERVICE_CODE],
        	    m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
        	    m_pcdr[ATTRI_HOME_CODE_B],
        	    groupATmp,
        	    m_pcdr[ATTRI_USER_TYPE_B]);
       if(rd>=0)
       {
       	m_pcdr[ATTRI_USER_HOME_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B][3];
       }
     }
  }
  else if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_IGSMG)
  {
        if (m_pcdr[ATTRI_HOME_CODE_B].length()>0 &&
        	  m_pcdr[ATTRI_HOME_CODE_B].compare(0,2,"00")!=0)
        {
            m_pcdr[ATTRI_HOME_CODE_B]="00" + m_pcdr[ATTRI_HOME_CODE_B];
        }
        else
        {
		       rd = m_globalManager->getHomeAreaCodeByMsisdnI(m_pcdr[ATTRI_SERVICE_CODE],
		        	    m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
		        	    m_pcdr[ATTRI_HOME_CODE_B],
		        	    long_group,long_group2,long_group3,long_group4,long_groupgprs,
		        	    m_pcdr[ATTRI_USER_TYPE_B],
		        	    m_pcdr[ATTRI_NATIONAL_CODE_B]);
		       if(rd>=0)
		       {
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
