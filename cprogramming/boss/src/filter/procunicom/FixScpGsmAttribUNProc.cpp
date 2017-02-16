#include "FixScpGsmAttribUNProc.h"

#include "FilterError.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(FixScpGsmAttribUNProc);

bool FixScpGsmAttribUNProc::getScpHomeAreaCodeA()
{
  int rd;
  T_ATTRI_VAL long_group;
  T_ATTRI_VAL calledType;
  T_ATTRI_VAL msisdn_number;
  msisdn_number = m_pcdr[ATTRI_MSISDN_A];
  rd = m_globalManager->getHomeAreaCodeByFixPhone(msisdn_number,
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
   }
   //固网号段匹配不到
   else if(m_globalManager->getHomeAreaCodeByMsisdnN(msisdn_number,
										m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
										m_pcdr[ATTRI_HOME_CODE_A],long_group,
										m_pcdr[ATTRI_USER_TYPE_A]))
   {
      //主叫号码求取出区号
      if (m_pcdr[ATTRI_USER_TYPE_A].length() != USER_TYPE_LENGTH)
      {
          m_pcdr[ATTRI_ERROR_CODE] = AH_MSISDNN_ERROR;
          return false;
      }
   }
   //加0匹配到区号
   else if(m_globalManager->getHomeAreaCodeByMsisdnN(msisdn_number.append(0,"0"),
										m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
										m_pcdr[ATTRI_HOME_CODE_A],long_group,
										m_pcdr[ATTRI_USER_TYPE_A]))
   {
      //主叫号码求取出区号
      if (m_pcdr[ATTRI_USER_TYPE_A].length() != USER_TYPE_LENGTH)
      {
          m_pcdr[ATTRI_ERROR_CODE] = AH_MSISDNN_ERROR;
          return false;
      }
   }
   else
   {
      m_pcdr[ATTRI_USER_TYPE_A] = USER_TYPE_B_DEFAULT_FIX;
      m_pcdr[ATTRI_HOME_CODE_A]      = m_pcdr[ATTRI_VISIT_CODE_A];
   }

   m_pcdr[ATTRI_USER_ASP_A] = m_pcdr[ATTRI_USER_TYPE_A][0];
   m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_USER_TYPE_A].substr(1,2);
   m_pcdr[ATTRI_USER_HOME_TYPE_A] = m_pcdr[ATTRI_USER_TYPE_A][3];
	 return true;
}

int FixScpGsmAttribUNProc::execute()
{
 #ifdef FP_DEBUG
   FP_BEGIN(FixScpGsmAttribUNProc)
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

	//求A归属地
	if(!getScpHomeAreaCodeA())
	{
		return 1;
	}

	//对端号码标准化
	otherPartyStandardization();

	//处理接入号码,如17951等
	if (!transNumberProcess())
	{
		return 1;
	}

  T_ATTRI_VAL homecodeb = m_pcdr[ATTRI_HOME_CODE_B];
	//求对方号码属性
	if (!getHomeAreaCodeB())
	{
		//求对方归属错，在td_fix_telephone里再查找
		if(!getFixTelephoneSeg())
		{
			//求对方号码归属错,再从特殊号码表找
			if (specialPhoneProcess()==1)
			{
				//找到匹配的特殊号码置缺省
				m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
				m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_HOME_CODE_A];
				m_pcdr[ATTRI_USER_HOME_TYPE_B] = AREA_TYPE_LOCAL;
			}
			else
			{
				//找不到，默认为本地固话 by shenglj 2009-01-10 00:25:01
				if(!setCNCDefaultHomeAreaB())
				{
				  return 1;
				}
				m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_HOME_CODE_A];
			}
		}
		//在td_fix_telephone查找成功，也到special_phone里再找一遍
		if (specialPhoneProcess()==1)
		{
			//找到匹配的特殊号码置缺省
			m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
			m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_HOME_CODE_A];
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
			getFixTelephoneSeg();
		}

    //规整对端号归属地
    setFixHomeAreaCodeB(homecodeb);

		//匹配特殊号码
		if (specialPhoneProcess()==-1)
		{
			return 1;
		}
	}

	//求长途类型
	setFixLongType();

	//求区间类型
	setSameSubAreaType();

	//本地化属性列表
	ProcessBaseHelper::ExecuteProcedure("LocalizeAttribSet");
	
	//设置话单是否有原始费用信息标志
	ProcessBaseHelper::ExecuteProcedure("SetOriginFeeFlag");

	//A业务类型调整
	setUserServiceTypeA();

	getExtendAttr();

	//求完属性后，在这儿可以判断那些错单，不再需要批价处理，在方法里直接置上错误代码
	setErrorCode();

 #ifdef FP_DEBUG
   FP_END(FixScpGsmAttribUNProc)
 #endif
  return 0;
}
