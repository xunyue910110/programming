#include "MiscCDRUNProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(MiscCDRUNProc);

bool MiscCDRUNProc::InitCdrProperty()
{
	reset();
  m_pcdr[ATTRI_USER_ASP_A]        = COMPANY_TYPE_UNKNOWN;
  m_pcdr[ATTRI_MSISDN_B]          = m_pcdr[ATTRI_MSISDN_B_ORG];
  m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B_ORG];

	return true;
}

bool MiscCDRUNProc::getTrash()
{
	 int t_iRet;
	 //本地化增数值业务 trash判断
	 t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeMiscTrashGet");
	 if(t_iRet==0)
	 {
	 	  return false;
	 }
	 else if(t_iRet==1)
	 {
      m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
      return true;
	 }

   return false;
}

bool MiscCDRUNProc::getDisChannelProperty()
{
	int t_iRet;
	//1. 无效话单
	if(getTrash())
    return true;

	//2. 错误话单
	if(getError())
		 return true;

	 //本地化增数值业务数据源判断
	 t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeMiscNormalSet");
	 if(t_iRet==0)
	 {
	 	  return true;
	 }
	 else if(t_iRet==1)
	 {
      m_pcdr[ATTRI_ERROR_FLAG]=ATTRI_ERROR_Y;
      return true;
	 }

	//3. 是否本运营商号段
	if(isSelfMobile(m_pcdr[ATTRI_MSISDN_A]))
	{
		 getNationalMisc();
		 return true;
	}
  //计费方即不是本地移动号码，也不是企业号码，无效话
	else
	{
		 m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
	}

	return true;
}

bool MiscCDRUNProc::getNationalMisc()
{
  int channelNo;

	//分检神州行话单
  if (m_globalManager->getHomeAreaCodeBySSP(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                                            areaCode,groupId,userType,officeCode,calledType))
  {
      m_pcdr[ATTRI_USER_ASP_A]          = userType[0];
      m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = userType.substr(1,2);
      m_pcdr[ATTRI_USER_HOME_TYPE_A]    = userType[3];

      if (userType[3]==AREA_TYPE_NATIONAL[0]) //外省话单
      {
      	m_globalManager->getNrChannelByProvCode(officeCode,channelNo);
      	m_pcdr[ATTRI_PROV_CHANNEL] = channelNo;
      }

      m_globalManager->getProvChannelByCitYCode(areaCode,channelNo);
      m_pcdr[ATTRI_CITY_CHANNEL] = channelNo;
      return true;
  }

   //省内话单
   if(m_globalManager->getHomeAreaCodeByMsisdnP(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                                                areaCode,groupId,userType,officeCode,calledType))
   {
       m_pcdr[ATTRI_USER_ASP_A]          = userType[0];
       m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = userType.substr(1,2);
       m_pcdr[ATTRI_USER_HOME_TYPE_A]    = userType[3];

       m_globalManager->getProvChannelByCitYCode(areaCode,channelNo);
       m_pcdr[ATTRI_CITY_CHANNEL] = channelNo;
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
   	  m_pcdr[ATTRI_USER_ASP_A]          = m_Config->m_operatorType;
      m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SERVICE_TYPE_GSM;
      m_pcdr[ATTRI_USER_HOME_TYPE_A]    = AREA_TYPE_LOCAL;
      return true;
   }

   return true;
}

int MiscCDRUNProc::execute()
{
 #ifdef FP_DEBUG
   FP_BEGIN(MiscCDRUNProc)
 #endif

 InitCdrProperty();

 //求分通道必要属性
 getDisChannelProperty();

 #ifdef FP_DEBUG
   FP_END(MiscCDRUNProc)
 #endif
  return 0;
}
