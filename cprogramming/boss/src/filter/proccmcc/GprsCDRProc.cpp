#include "GprsCDRProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(GprsCDRProc);

bool GprsCDRProc::InitCdrProperty()
{
	reset();

	return true;
}

bool GprsCDRProc::getTrash()
{
	 int t_iRet;
	 //本地化gprs trash判断
	 t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeGprsTrashGet");

    //取SCDR和GCDR话单
    if(m_pcdr[ATTRI_CALL_TYPE][0]=='0' || m_pcdr[ATTRI_CALL_TYPE][0]=='1')
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

bool GprsCDRProc::getDisChannelProperty()
{
	//1. 无效话单
	if(getTrash())
    return true;

	//2. 错误话单
	if(getError())
		 return true;

    if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GPRS_ROG  ||  //国内出访话单
       m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GPRS_ROIG ||  //国际出访话单
       m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GPRS_MMS)     //彩信核减清单
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


bool GprsCDRProc::getNational()
{
  int channelNo;

	//分检神州行话单
  if (m_globalManager->getHomeAreaCodeBySSP(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                                            areaCode,groupId,userType,officeCode,calledType))
  {
  	  //省内神州行以gcdr为准，来访的神州行以scdr为准
      if (userType[3]==AREA_TYPE_LOCAL[0]) //省内话单
      {
      	 if(m_pcdr[ATTRI_CALL_TYPE][0]=='0')
      	 {
      	 	 m_pcdr[ATTRI_TRASH_FLAG] = ATTRI_TRASH_Y;
      	 	 return true;
      	 }
         m_pcdr[ATTRI_USER_ASP_A]          = userType[0];
         m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = userType.substr(1,2);
         m_pcdr[ATTRI_USER_HOME_TYPE_A]    = userType[3];
         return true;
      }
      //省内话单 漫游上发获取分组通道
      else
      {
      	 if(m_pcdr[ATTRI_CALL_TYPE][0]=='1')
      	 {
      	 	 m_pcdr[ATTRI_TRASH_FLAG] = ATTRI_TRASH_Y;
      	 	 return true;
      	 }
      	m_globalManager->getNrChannelByProvCode(officeCode,channelNo);
      	m_pcdr[ATTRI_PROV_CHANNEL] = channelNo;
        return true;
      }
  }

	 if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_GPRS_CNG) //集团下发CNG话单
	 {
		  return true;
	 }

   //省内话单
   //GCDR话单，判断SGSN，本省SGSN有效，外省SGSN无效
   if(m_globalManager->getHomeAreaCodeByMsisdnP(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                                                areaCode,groupId,userType,officeCode,calledType))
   {
   	   //省内SCDR话单无效
    	 if(m_pcdr[ATTRI_CALL_TYPE][0]=='0')
    	 {
    	 	 m_pcdr[ATTRI_TRASH_FLAG] = ATTRI_TRASH_Y;
    	 	 return true;
    	 }

       m_pcdr[ATTRI_USER_ASP_A]          = userType[0];
       m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = userType.substr(1,2);
       m_pcdr[ATTRI_USER_HOME_TYPE_A]    = userType[3];
       //GCDR、CNG话单，判断SGSN
       if(m_pcdr[ATTRI_CALL_TYPE][0]=='1')
       {
          if(m_globalManager->getSgsnAddrInfo(m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
          	m_pcdr[ATTRI_GPRS_SGSN].c_str(),provinceCode,sgsnhomeType))
          {
          	  //归属是本地或者省内
              if(sgsnhomeType==AREA_TYPE_LOCAL || sgsnhomeType==AREA_TYPE_PROVINCE)
              {
                  return true;
              }
              else
              {
				    	 	 m_pcdr[ATTRI_TRASH_FLAG] = ATTRI_TRASH_Y;
				    	 	 return true;
              }
          }
          else
          {
              m_pcdr[ATTRI_ERROR_CODE]=GSM_ERROR_SGSN;
              return true;
          }
       }
      return true;
   }
   //国内来访话单
   else if(m_globalManager->getHomeAreaCodeByPhoneN(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                                                    areaCode,groupId,userType,provinceCode,calledType))
   {
 	    //国内GCDR话单无效
  	  if(m_pcdr[ATTRI_CALL_TYPE][0]=='1')
  	  {
  	  	 m_pcdr[ATTRI_TRASH_FLAG] = ATTRI_TRASH_Y;
  	  	 return true;
  	  }

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

int GprsCDRProc::execute()
{
  InitCdrProperty();

  //求分通道必要属性
  getDisChannelProperty();

  //分拣规则表求通道名
  ProcessBaseHelper::ExecuteProcedure("ChannelProc");

  return 0;
}