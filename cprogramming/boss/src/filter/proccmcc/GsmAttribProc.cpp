#include "GsmAttribProc.h"

#include "FilterError.h"

INSTALL_PROCESSBASE_INSTANCE(GsmAttribProc);

void GsmAttribProc::initialattr()
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
}

int GsmAttribProc::execute()
{
	//错单,不进行处理
	if (!isValidCDR())
	{
		return 1;
	}

	initialattr();

	//特殊处理,主要是对计费方号码标准化
	specialPreProcessCdr();
	
	//求A归属地
	if(!getHomeAreaCodeA())
	{
		return 1;
	}
	
	//Added By Sunam 2006-5-27 14:44 国际漫游来方呼转话单将其转换为主叫话单处理
	if (((m_pcdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_HONGKONG) ||
		 (m_pcdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_MACAO) ||
		 (m_pcdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_TAIWAN) ||
		 (m_pcdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_INTERNATIONAL)) && 
		(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW))
	{
		m_pcdr[ATTRI_CALL_TYPE] = CALL_TYPE_MOC;
	}
	
	//求A到访地
	if(!getVisitAreaCodeA())
	{
		return 1;
	}
	
	//判断边界漫游
	edgeRoamProcess();
	
	//对端号码标准化
	otherPartyStandardization();
	
	//求漫游类型
	setRoamType();
	
	//处理接入号码,如17951等
	if (!transNumberProcess())
	{
		return 1;
	}
	
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
				return 1;
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

	//配置特殊号码组表td_special_phone_group,判对方号码是否属于已定义的一组里
	if (!getSpecialPhoneGroup())
	{
		return 1;
	}
	
	//如果是呼转话单,配置呼转话单的userType
	if (!getUserTypeC())
	{
		return 1;
	}
	
	//求B到访地
	if (!getVisitAreaCodeB())
	{
		return 1;
	}
	
	//求中继信息
	if (!getTrunkInfo())
	{
		return 1;
	}
	
	//求长途类型
	setLongType();
	
	//置转接话单标志--m_isUnicomTrans
	setUnicomTrans();

	//本地化属性列表
	ProcessBaseHelper::ExecuteProcedure("LocalizeAttribSet");
	
	//设置话单是否有原始费用信息标志
	ProcessBaseHelper::ExecuteProcedure("SetOriginFeeFlag");
	
	//求完属性后，在这儿可以判断那些错单，不再需要批价处理，在方法里直接置上错误代码
	setErrorCode();
	
  return 0;
}
