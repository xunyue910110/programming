#include "IPAttribUNProc.h"

#include "FilterError.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(IPAttribUNProc);

void IPAttribUNProc::initialattr()
{
  //如有原先规整的B号码，以规整为准
  if(m_pcdr[ATTRI_MSISDN_B_FOR_ROAM].length()<=0)
  {
  	m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B_ORG];
  }
  if(m_pcdr[ATTRI_MSISDN_B].length()<=0)
  {
  	m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_MSISDN_B_FOR_ROAM];
  }
}


int IPAttribUNProc::execute()
{
 #ifdef FP_DEBUG
   FP_BEGIN(IPAttribUNProc)
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

	initialattr();

	//特殊处理,主要是对计费方号码标准化
	specialPreProcessCdr();

	//计费号码规整
	//msisdnStandardization();

	//主叫号码去掉17911,193等前缀
	//callingCodePrefixProcess();

	//求A归属地
	if(!getTollHomeAreaA())
	{
		return 1;
	}

  //归属地重置
  if(m_Config->m_provinceCode == "090")
  {
     m_pcdr[ATTRI_HOME_CODE_A] = m_pcdr[ATTRI_VISIT_CODE_A];
  }

	//对端号码标准化
	otherPartyStandardization();

	//处理接入号码,如17951等
	if (!transNumberProcess())
	{
		return 1;
	}

  m_pcdr[ATTRI_VISIT_CODE_B] = m_pcdr[ATTRI_HOME_CODE_B];
	//求对方号码属性
	if (!getHomeAreaCodeB())
	{
		//求对方归属错，在td_fix_telephone里再查找
		if(!getFixTelephone())
		{
			//求对方号码归属错,再从特殊号码表找
			if (specialPhoneProcess()==1)
			{
				//找到匹配的特殊号码置缺省
				m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
				m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_VISIT_CODE_A];
				m_pcdr[ATTRI_USER_HOME_TYPE_B] = AREA_TYPE_LOCAL;
			}
			else
			{
				//找不到，默认为本地固话 by shenglj 2009-01-10 00:25:01
				if(!setDefaultHomeAreaB())
				{
				  return 1;
				}
			}
		}
		//在td_fix_telephone查找成功，也到special_phone里再找一遍
		if (specialPhoneProcess()==1)
		{
			//找到匹配的特殊号码置缺省
			m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
			m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_VISIT_CODE_A];
			m_pcdr[ATTRI_USER_HOME_TYPE_B] = AREA_TYPE_LOCAL;
		}
	}
	else
	{
		string test = m_pcdr[ATTRI_USER_ASP_B];
		string hometypeb = m_pcdr[ATTRI_USER_HOME_TYPE_B];
		if( (m_pcdr[ATTRI_USER_ASP_B] != COMPANY_TYPE_CMCC)&&
			(m_pcdr[ATTRI_USER_ASP_B] != COMPANY_TYPE_UNICOM)&&
			((m_pcdr[ATTRI_USER_HOME_TYPE_B] == AREA_TYPE_LOCAL)||
			 (m_pcdr[ATTRI_USER_HOME_TYPE_B] == AREA_TYPE_PROVINCE)||
			 (m_pcdr[ATTRI_USER_HOME_TYPE_B] == AREA_TYPE_NATIONAL)))
		{
			getFixTelephone();
		}

		//匹配特殊号码
		if (specialPhoneProcess()==-1)
		{
			return 1;
		}
	}

  setRoamType();
  setLongType();

  //判区内区间 求对端类型 确定计费方
  ProcessBaseHelper::ExecuteProcedure("LocalizeIPPAAreaSetProc");

  //求城市代码
	if (!getCPNCode())
	{
	   return 1;
	}

  //求A的省内通道号
  m_globalManager->getProvChannelByCitYCode(m_pcdr[ATTRI_HOME_CODE_A],m_channelNo);
  m_pcdr[ATTRI_CITY_CHANNEL] = m_channelNo;

	//本地化属性列表
	ProcessBaseHelper::ExecuteProcedure("LocalizeAttribSet");

	getExtendAttr();

	//求完属性后，在这儿可以判断那些错单，不再需要批价处理，在方法里直接置上错误代码
	setErrorCode();

 #ifdef FP_DEBUG
   FP_END(IPAttribUNProc)
 #endif
  return 0;
}


