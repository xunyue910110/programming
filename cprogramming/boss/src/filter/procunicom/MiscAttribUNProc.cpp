#include "MiscAttribUNProc.h"

#include "PMGlobalManager.h"
#include "FilterError.h"

INSTALL_PROCESSBASE_INSTANCE(MiscAttribUNProc);

void MiscAttribUNProc::initialattr()
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

int MiscAttribUNProc::execute()
{
 #ifdef FP_DEBUG
   FP_BEGIN(MiscAttribUNProc)
 #endif

  //无效话单直接返回
  if(m_pcdr[ATTRI_TRASH_FLAG]==ATTRI_TRASH_Y)
	{
		  m_FilterInfo->m_unique_flag = FLAG_NO;
     return 0;
	}

	//代收费业务计费规则判断
	if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_UNICOMSP)
	{
		 ProcessBaseHelper::ExecuteProcedure("LocalizeSpBillRuleProc");
	}

	//错单,不进行处理
	if (!isValidCDR())
	{
		return 1;
	}

	initialattr();

  //计费方号码特殊化
  specialPreProcessCdr();

  //pps语音A1
  //A和B的归属都要求取
  if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_PPS)
  {
     //msisdnStandardization();
     //callingCodePrefixProcess();
     //求A归属地
     if (!getHomeAreaCodeA())
     {
        //return 1;
	   }

     //求A到访地
		 if(!getVisitAreaCodeA())
		 {
			 //return 1;
		 }

     if((strncmp(m_pcdr[ATTRI_VISIT_CODE_A].c_str(),"0",1) != 0))
     {
        m_pcdr[ATTRI_VISIT_CODE_A].append(0,"0");
     }

     /*
     otherPartyStandardization();
     //求承载类型、传输类型
     if(!transNumberProcess())
     {
        //return 1;
     }
     */
     m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];

     T_ATTRI_VAL r_homecodeb_org;
     r_homecodeb_org = m_pcdr[ATTRI_HOME_CODE_B];
     //求B归属地
     if (!getHomeAreaCodeB())
     {
        m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_VISIT_CODE_A];
        m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;    
     }
     if(strncmp(r_homecodeb_org.c_str(),"0",1) == 0)
     {
       m_pcdr[ATTRI_HOME_CODE_B] = r_homecodeb_org;
     }

		 //求B到访地
		 if (!getVisitAreaCodeB())
		 {
			 getBVByHome();
		 }

     //求下漫游 长途类型
     if(m_pcdr[ATTRI_ROAM_TYPE].length()==0)
     {
	     setRoamType();
     }
     //本地处理
     if(m_pcdr[ATTRI_ROAM_TYPE].in(ROAM_TYPE_NONE,ROAM_TYPE_PRO_IN,ROAM_TYPE_NATIONAL_IN,ROAM_TYPE_INTERNATIONAL_IN))
     {
	     setLongType();
     }
     //来访处理
     else
     {
     	 setRoLongType();
     }
  }
  //宝视通清单帐单46 集团业务47
  else if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SP_BST ||
  	      m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SP_GRP)
  {
  	//置默认值
  }
  //省内彩信 48 A和B的归属都要求取
  else if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SP_MMS)
  {
     if (!getAHByMsisdn(m_pcdr[ATTRI_MSISDN_A]))
     {
        m_pcdr[ATTRI_ERROR_CODE]= DEFAULT_ERROR;
     }
     //求B归属地
     if (!getHomeAreaCodeB())
     {
         m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_VISIT_CODE_A];
         m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;    
     }
	   if((strncmp(m_pcdr[ATTRI_HOME_CODE_B].c_str(),"0",1) != 0))
	   {
	      m_pcdr[ATTRI_HOME_CODE_B].append(0,"0");
	   }
  }
  //SP包月事件 BIZ_TYPE为6
  //增值业务   BIZ_TYPE为8
  //统计业务   BIZ_TYPE为9 G网PPS数据业务A4 智能网SP增值业务A5 ......
  //只要求A的归属
  else if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_SPMONTH  ||
  	      m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_UNICOMSP ||
  	      m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_STATIS)
  {
     //msisdnStandardization();
     //callingCodePrefixProcess();
     if (!getAHByMsisdn(m_pcdr[ATTRI_MSISDN_A]))
     {
        m_pcdr[ATTRI_ERROR_CODE]= DEFAULT_ERROR;
     }
  }

  if(m_pcdr[ATTRI_USER_ASP_A] == COMPANY_TYPE_UNKNOWN)
  {
    m_pcdr[ATTRI_USER_ASP_A]          = m_Config->m_operatorType;
	  m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SERVICE_TYPE_GSM;
	  m_pcdr[ATTRI_USER_HOME_TYPE_A]    = AREA_TYPE_LOCAL;
  }

  //如果是错单，重置0
	if (!isValidCDR())
	{
    m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
	}

	//本地化属性列表
	ProcessBaseHelper::ExecuteProcedure("LocalizeAttribSet");

	//设置话单是否有原始费用信息标志
	ProcessBaseHelper::ExecuteProcedure("SetOriginFeeFlag");

 #ifdef FP_DEBUG
   FP_END(MiscAttribUNProc)
 #endif
	return 0;
	
}

