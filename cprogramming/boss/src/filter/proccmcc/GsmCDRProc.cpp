#include "GsmCDRProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(GsmCDRProc);

bool GsmCDRProc::InitCdrProperty()
{
	reset();
  m_pcdr[ATTRI_USER_ASP_A]        = COMPANY_TYPE_UNKNOWN;
  m_pcdr[ATTRI_MSISDN_B]          = m_pcdr[ATTRI_MSISDN_B_ORG];
  m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B_ORG];

	return true;
}

bool GsmCDRProc::getTrash()
{
	 int t_iRet;
	 //本地化gsm trash判断
	 t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeGsmTrashGet");
	 if(t_iRet==0)
	 {
	 	  return false;
	 }
	 else if(t_iRet==1)
	 {
      m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
      return true;
	 }

   if(m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_MOC ||  // 01:MOC; 02:MTC; 03:FORW;
      m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_MTC ||   //有效话单
      m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_FORW)
   {
      return false;
   }
   else if(m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMMO ||  // SMMO SMMT
           m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMMT)
   {
       if(isNationalImsi(m_pcdr[ATTRI_IMSI_NUMBER]))
				 {
           m_pcdr[ATTRI_TRASH_FLAG] = ATTRI_TRASH_Y;     //国内短信无效话单
           return true;
				 }
        else
        {
           return false;//国际来访短信有效话单
        }
   }
	 else
	 {
	    m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;  //其他是无效
	    return true;
	 }

   return true;
}

bool GsmCDRProc::getDisChannelProperty()
{
	//1. 无效话单
	if(getTrash())
    return true;

	//2. 错误话单
	if(getError())
		 return true;

	//3. 是否国际来访
	if(m_globalManager->isImsiI(m_pcdr[ATTRI_IMSI_NUMBER],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],homeType) )
	{
		if(homeType!=AREA_TYPE_LOCAL)
		{
			m_pcdr[ATTRI_USER_HOME_TYPE_A] = homeType;
			return true;
		}
	}
	//4. 是否移动号段
	else if(isMobile(m_pcdr[ATTRI_MSISDN_A]))
	//else if(m_globalManager->getAspByMsisdnHeader(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],aspType))
	{
		 getNational();
		 return true;
	}
	else
	{
		 m_pcdr[ATTRI_ERROR_CODE] = GSM_ERROR_IMSI;
	}

	return true;
}


bool GsmCDRProc::getNational()
{
  int channelNo;

	//分检神州行话单
  if (m_globalManager->getHomeAreaCodeBySSP(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                                            areaCode,groupId,userType,officeCode,calledType))
  {
      if (userType[3]==AREA_TYPE_LOCAL[0]) //省内话单
      {
         m_pcdr[ATTRI_USER_ASP_A]          = userType[0];
         m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = userType.substr(1,2);
         m_pcdr[ATTRI_USER_HOME_TYPE_A]    = userType[3];
         return true;
      }
      //省内话单 漫游上发获取分组通道
      else
      {
      	m_globalManager->getNrChannelByProvCode(officeCode,channelNo);
      	m_pcdr[ATTRI_PROV_CHANNEL] = channelNo;
        return true;
      }
  }

   //省内话单
   if(m_globalManager->getHomeAreaCodeByMsisdnP(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                                                areaCode,groupId,userType,officeCode,calledType))
   {
       m_pcdr[ATTRI_USER_ASP_A]          = userType[0];
       m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = userType.substr(1,2);
       m_pcdr[ATTRI_USER_HOME_TYPE_A]    = userType[3];
      if(m_pcdr[ATTRI_RATE_INDICATE]=="v" && m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GSM_MSC)
      {
         m_pcdr[ATTRI_TRASH_FLAG] = ATTRI_TRASH_Y;
      }
      return true;
   }
   //国内来访话单
   else if(m_globalManager->getHomeAreaCodeByPhoneN(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                                                    areaCode,groupId,userType,provinceCode,calledType))
   {
      m_pcdr[ATTRI_USER_ASP_A]          = userType[0];
      m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = userType.substr(1,2);
      m_pcdr[ATTRI_USER_HOME_TYPE_A]    = userType[3];

      m_globalManager->getNrChannelByProvCode(provinceCode,channelNo);
      m_pcdr[ATTRI_PROV_CHANNEL] = channelNo;
      return true;
   }
   //默认省内话单
   else
   {
   	  m_pcdr[ATTRI_USER_ASP_A]          = COMPANY_TYPE_CMCC;
      m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SERVICE_TYPE_GSM;
      m_pcdr[ATTRI_USER_HOME_TYPE_A]    = AREA_TYPE_LOCAL;
      return true;
   }

   return true;
}

//先求通道后求属性
int GsmCDRProc::execute()
{
  InitCdrProperty();

  //求分通道必要属性
  getDisChannelProperty();

  //分拣规则表求通道名
  ProcessBaseHelper::ExecuteProcedure("ChannelProc");

  return 0;
}

