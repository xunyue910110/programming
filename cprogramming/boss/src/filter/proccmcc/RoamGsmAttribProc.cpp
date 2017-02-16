#include "RoamGsmAttribProc.h"
#include "PMGlobalManager.h"

#include "FilterError.h"

INSTALL_PROCESSBASE_INSTANCE(RoamGsmAttribProc);


int RoamGsmAttribProc::execute()
{
	int i;
	
	//错单,不进行处理
	if (!isValidCDR())
	{
		return 1;
	}

	//特殊处理,主要是对号码标准化
	specialPreProcessCdr();

	//求A归属地
	if (!getRoHomeAreaCodeA())
	{
		return 1;
	}
	if (isUnConditionalForw())
	{
		m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
	}
	else
	{
		if (!getRoVisitAreaCodeA())
		{
			return 1;
		}
	}

	//判断边界漫游
	edgeRoamProcess();

	//对端号码标准化
	if ((m_pcdr[ATTRI_SOURCE_TYPE] != SOURCE_TYPE_GSM_ROK) && 
			(m_pcdr[ATTRI_SOURCE_TYPE] != SOURCE_TYPE_GSM_ROI))
	{
		otherPartyStandardization();
	}
	
	//求漫游类型
	setRoRoamType();

	//处理接入号码,如17951等
	//Modified by Sunam 2007-3-5 9:06 国际出访话单不处理接入号
	//if (!transNumberProcess())
	//	return false;
	if ((m_pcdr[ATTRI_SOURCE_TYPE] != SOURCE_TYPE_GSM_ROK) &&
		(m_pcdr[ATTRI_SOURCE_TYPE] != SOURCE_TYPE_GSM_ROI))
	{
		if (!transNumberProcess())
		{
			return false;
		}
	}

	//求对方号码属性
	if (!getRoHomeAreaCodeB())
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
	//国际漫游客服全免
	if ((m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_ROK) || (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_ROI))
	{
		T_ATTRI_VAL msisdnB;
		//13800XXX186
		if (m_pcdr[ATTRI_MSISDN_B].length() >= 11 )
		{
			if ((strncmp(m_pcdr[ATTRI_MSISDN_B].c_str() + m_pcdr[ATTRI_MSISDN_B].length() - 3,"186",3) == 0) &&
				(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str() + m_pcdr[ATTRI_MSISDN_B].length() - 11,"13800",5) == 0))
			{
				m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SERVICE_TYPE_INTERROAMCALLCENTER;
			}
		}
		
		if (((strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"1860",4) ==0) || 
				(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"00861860",8) == 0)) &&
				(m_pcdr[ATTRI_BEGIN_DATE] <= "20061218"))
		{ 
			m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SERVICE_TYPE_INTERROAMCALLCENTER;
		}
		else if ((strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"10086",5) ==0) || 
						(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"008610086",9) == 0))
		{
			m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SERVICE_TYPE_INTERROAMCALLCENTER;
		}
		else if(strstr(m_pcdr[ATTRI_MSISDN_B].c_str(),"1860") || strstr(m_pcdr[ATTRI_MSISDN_B].c_str(),"10086"))
		{
			if(m_interHead.size() > 1)
			{
				::sort(m_interHead.begin(),m_interHead.end(),interHeadCmp);
			}
			
			for(i=0;i<m_interHead.size();++i)
			{
				// 第一步,匹配到国际长途接入字冠
				if( strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),m_interHead[i].c_str(),m_interHead[i].length())==0 )
				{
					//第二步,判断是否国际长途接入字冠后是"86"
					msisdnB = m_pcdr[ATTRI_MSISDN_B].substr(m_interHead[i].length(),m_pcdr[ATTRI_MSISDN_B].length()-m_interHead[i].length());
					if( strncmp(msisdnB.c_str(),"86",2)==0 )
					{
						T_ATTRI_VAL homeAreaCodeB,longGroup1,userTypeB;
						msisdnB = msisdnB.substr(2,msisdnB.length()-2);
						
						if ((strncmp(msisdnB.c_str(),"1860",4) == 0 ) && (m_pcdr[ATTRI_BEGIN_DATE] <= "20061218"))
						{
							m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SERVICE_TYPE_INTERROAMCALLCENTER;
						}
						else if (strncmp(msisdnB.c_str(),"10086",5) == 0 )
						{
							m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SERVICE_TYPE_INTERROAMCALLCENTER;
						}
						else
						{
							//第三步,判断1860前是否有国内有效区号
							msisdnB = "0" + msisdnB;
							if (m_globalManager->getHomeAreaCodeByMsisdnN(msisdnB, 
													m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME], 
													homeAreaCodeB,longGroup1,userTypeB))
							{
								msisdnB = msisdnB.substr(homeAreaCodeB.length(),msisdnB.length()-homeAreaCodeB.length());
								if ((strncmp(msisdnB.c_str(),"1860",4) == 0 ) && (m_pcdr[ATTRI_BEGIN_DATE] <= "20061218"))
          		  {
          		  	m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SERVICE_TYPE_INTERROAMCALLCENTER;
          		  }
          		  else if (strncmp(msisdnB.c_str(),"10086",5) == 0 )
          		  {
          		  	m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SERVICE_TYPE_INTERROAMCALLCENTER;
          		  }
          		}
						}
					}
					break;
				}
			}
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
	
	/* get the sub-B's visit area code */
	if(!getRoVisitAreaCodeB())
	{
		return 1;
	}
	
	//Added by Sunam 2007-1-22 9:33
	//国际漫游出访短信置上一个特殊的Serv_type，以便对国际漫游出访港澳台短信单独合账
	if ((m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_ROI) &&
			((m_pcdr[ATTRI_GSM_SERVICE_CODE] == "21") || 
			 (m_pcdr[ATTRI_GSM_SERVICE_CODE] == "22")))
	{
		m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SERVICE_TYPE_INTERSMS;
	}

	//求长途类型
	setRoLongType();

	//置转接话单标志--m_isUnicomTrans
	setUnicomTrans();
	
	//本地化属性列表
	ProcessBaseHelper::ExecuteProcedure("LocalizeAttribSet");

	//设置话单是否有原始费用信息标志
	ProcessBaseHelper::ExecuteProcedure("SetOriginFeeFlag");

	//setTimes();
	
	//A业务类型调整
	setUserServiceTypeA();
	//interNationalFeeAdjust();
	
	//Added by Sunam 2006-3-13 14:07
	getExtendAttr();
	
	//求完属性后，在这儿可以判断那些错单，不再需要批价处理，在方法里直接置上错误代码
	setErrorCode();

	return 0;
}

/*
bool RoamGsmAttribProc::interNationalFeeAdjust()
{
    	国际漫游出访被叫话单所有费用应该放在漫游费里,如果长途费有费用,加到漫游费上
    	然后漫游费上浮,长途费重新划价上浮
    if (m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_OUT)
    {
        m_pcdr[ATTRI_CFEE] = m_pcdr[ATTRI_CFEE] + m_pcdr[ATTRI_LFEE];
        m_pcdr[ATTRI_LFEE] = 0;
    }

    return true;
}
*/

