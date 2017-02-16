#include "GprsCDRUNProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(GprsCDRUNProc);

bool GprsCDRUNProc::InitCdrProperty()
{
	reset();

	return true;
}

bool GprsCDRUNProc::getTrash()
{
	 int t_iRet;
	 //本地化gprs trash判断
	 t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeGprsTrashGet");

    //取SCDR和GCDR话单
    if(strncmp(m_pcdr[ATTRI_GPRS_RECORDTYPE].c_str(), "0", 1) == 0 ||
    	 strncmp(m_pcdr[ATTRI_GPRS_RECORDTYPE].c_str(), "1", 1) == 0)
    {
       return false;
    }
    else
    {
   	   m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
       return true;
    }     
   return true;
}

bool GprsCDRUNProc::getDisChannelProperty()
{
	//1. 无效话单
	if(getTrash())
    return true;

	//2. 错误话单
	if(getError())
		 return true;

/*
    if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GPRS_ROG  ||  //国内出访话单
       m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GPRS_ROIG ||  //国际出访话单
       m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GPRS_MMS)     //彩信核减清单
    {
        return true;
    }
*/
    if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GPRS_ROIG)     //国际出访话单
    {
       return true;
    }

	//3. 是否国际来访
	if(m_globalManager->isImsiI(m_pcdr[ATTRI_IMSI_NUMBER],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],homeType) )
	{
		if(homeType!=AREA_TYPE_LOCAL)
		{
			m_pcdr[ATTRI_USER_HOME_TYPE_A] = homeType;
			return true;
		}
	}
	//4. 是否移动号段 IMSI在范围内
	else if(isMobile(m_pcdr[ATTRI_MSISDN_A]) && isNationalImsi(m_pcdr[ATTRI_IMSI_NUMBER]))
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


bool GprsCDRUNProc::getNational()
{
  int channelNo;

	//分检神州行话单
  if (m_globalManager->getHomeAreaCodeBySSP(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                                            areaCode,groupId,userType,officeCode,calledType))
  {
      if (userType.length() != USER_TYPE_LENGTH)
      {
          m_pcdr[ATTRI_ERROR_CODE] = AH_MSISDNN_ERROR;
          return false;
      }
      m_pcdr[ATTRI_USER_ASP_A]          = userType[0];
      m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = userType.substr(1,2);
      m_pcdr[ATTRI_USER_HOME_TYPE_A]    = userType[3];
  	  //省内智能网以gcdr保留，scdr无效
      if (userType[3]==AREA_TYPE_LOCAL[0]) //省内话单
      {
         m_globalManager->getProvChannelByCitYCode(areaCode,channelNo);
         m_pcdr[ATTRI_CITY_CHANNEL] = channelNo;
         return true;
      }
      //智能网来访话单直接置无效标识 漫游上发获取分组通道
      else
      {
      	m_globalManager->getNrChannelByProvCode(officeCode,channelNo);
      	m_pcdr[ATTRI_PROV_CHANNEL] = channelNo;
        return true;
      }
  }

/*
   //集团下发国内出访 国际出访GPRS话单统计类 不作处理
	 if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GPRS_ROG || m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GPRS_ROIG) //集团下发话单
	 {
		  return true;
	 }
*/
   //省内话单
   //GCDR话单，判断SGSN，本省SGSN有效，外省SGSN无效
   if(m_globalManager->getHomeAreaCodeByMsisdnP(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                                                areaCode,groupId,userType,officeCode,calledType))
   {
      if (userType.length() != USER_TYPE_LENGTH)
      {
          m_pcdr[ATTRI_ERROR_CODE] = AH_MSISDNN_ERROR;
          return false;
      }
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
      if (userType.length() != USER_TYPE_LENGTH)
      {
          m_pcdr[ATTRI_ERROR_CODE] = AH_MSISDNN_ERROR;
          return false;
      }
      m_pcdr[ATTRI_USER_ASP_A]          = userType[0];
      m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = userType.substr(1,2);
      m_pcdr[ATTRI_USER_HOME_TYPE_A]    = userType[3];

 	    //国内话单无效
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

int GprsCDRUNProc::execute()
{
 #ifdef FP_DEBUG
   FP_BEGIN(GprsCDRUNProc)
 #endif

  InitCdrProperty();

  //求分通道必要属性
  getDisChannelProperty();

 #ifdef FP_DEBUG
   FP_END(GprsCDRUNProc)
 #endif
  return 0;
}
