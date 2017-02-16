#include "LocalizeScpAttribProc_010.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(ScpGsmAttribProc);

//sdbss
//山东智能网属性求取
int ScpGsmAttribProc::execute() 
{
	//错单,不进行处理
	if (!isValidCDR())
	{
		return 1;
	}
	//特殊处理,主要是对号码标准化
	specialPreProcessCdr();
	
	//求A到访地
	if(!getHomeAreaCodeA())
	{
		return 1;
	}

	
	//if(m_cdr[ATTRI_VISIT_CODE_A].length() == 0) 
	//if((m_cdr[ATTRI_VISIT_CODE_A].length() == 0) || (m_cdr[ATTRI_FORWARD_CAUSE].length() > 0 )) modify by xiangyang 20090302
	if((m_cdr[ATTRI_VISIT_CODE_A].length() == 0) || (m_cdr[ATTRI_FORWARD_CAUSE] > 0 ))  //add by 20081016
	{
		m_cdr[ATTRI_VISIT_CODE_A] = m_cdr[ATTRI_HOME_CODE_A];
	}

	//到访地前去86
	if(m_cdr[ATTRI_VISIT_CODE_A].compare(0,2,"86") == 0)
	{
		char sTmp[24 + 1];
        memset(sTmp,0,sizeof(sTmp));
        sprintf(sTmp,"%s", m_cdr[ATTRI_VISIT_CODE_A].c_str() + 2);
        m_cdr[ATTRI_VISIT_CODE_A] = sTmp;
	}
	
	if (m_cdr[ATTRI_VISIT_CODE_A].compare(0,1,"0") !=0)
	    m_cdr[ATTRI_VISIT_CODE_A] = m_cdr[ATTRI_VISIT_CODE_A].append(0,"0");
	    
	//边界漫游类型置为无边界漫游 add by yangrl 2010/12/30 15:45:51
	m_pcdr[ATTRI_EDGE_FLAG]='0';
	
	//WVPN话单也要求根据位置信息求到访地，为了判断边界漫游 --mod by yangyi 2006,12,24,02:03
	//if (m_cdr[ATTRI_ROAM_TYPE] >= ROAM_TYPE_NATIONAL_OUT)
	//小灵通携号转网话单,需要重新求取计费号码漫游地、漫游类型 mod by yangrl 2010/12/30 15:46:06
	if (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_GZHEJ_S)
	{
    	T_ATTRI_VAL backupVisitCode;
			backupVisitCode = m_cdr[ATTRI_VISIT_CODE_A];
			
    	if(!getVisitAreaCodeA())
    	{
    		// 呼转话单要把到访地置为归属地, tanhao modify, 2009-05-29
    		if ( m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW )
    		{
    			m_cdr[ATTRI_VISIT_CODE_A] = m_cdr[ATTRI_HOME_CODE_A];
    		}
				else
				{
					m_cdr[ATTRI_VISIT_CODE_A] = backupVisitCode;
				}
    		
    		m_cdr[ATTRI_ERROR_CODE] = DEFAULT_ERROR;
    	}
    	//边界漫游处理 
    	edgeRoamProcess();
    	//求漫游类型
			setScpRoamType();
	}
	

	//对端号码标准化
	otherPartyStandardization();
	

	//处理接入号码,如17951等
	if (!transNumberProcess())
		return false;

	//求对方号码属性
	if (!getHomeAreaCodeB())
	{
    	//求对方归属错,在 td_fix_telephone 里再查找
		if(!getFixTelephone())
		{
			//求对方号码归属错,再从特殊号码表找
			if (specialPhoneProcess()==1)
			{
				//找到匹配的特殊号码置缺省
				m_cdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
				m_cdr[ATTRI_HOME_CODE_B] = m_cdr[ATTRI_VISIT_CODE_A];
				m_cdr[ATTRI_USER_HOME_TYPE_B] = AREA_TYPE_LOCAL;
			}
			else
			{
				return 1;
			}
		}
		//在 td_fix_telephone 查找成功,也到 special_phone 里再找一遍
		if (specialPhoneProcess()==1)
		{
			//找到匹配的特殊号码置缺省
			m_cdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
			m_cdr[ATTRI_HOME_CODE_B] = m_cdr[ATTRI_VISIT_CODE_A];
			m_cdr[ATTRI_USER_HOME_TYPE_B] = AREA_TYPE_LOCAL;
		}
    }
  else
  {
		if( (m_cdr[ATTRI_USER_ASP_B] != COMPANY_TYPE_CMCC)&&
			(m_cdr[ATTRI_USER_ASP_B] != COMPANY_TYPE_UNICOM)&&
			((m_cdr[ATTRI_USER_HOME_TYPE_B] == AREA_TYPE_LOCAL)||
			 (m_cdr[ATTRI_USER_HOME_TYPE_B] == AREA_TYPE_PROVINCE)||
			 (m_cdr[ATTRI_USER_HOME_TYPE_B] == AREA_TYPE_NATIONAL)))
		{
			getFixTelephone();
		}
		
		//匹配特殊号码
		if (specialPhoneProcess()==-1)
		{
			return 1;
		}
	}
	if (!getSpecialPhoneGroup())
	{
		return 1;
	}
	if(m_cdr[ATTRI_VISIT_CODE_B].length() == 0)
	{
		m_cdr[ATTRI_VISIT_CODE_B]= m_cdr[ATTRI_HOME_CODE_B];
	}
	
	//求长途类型
	//小灵通携号转网话单,需要重新求取长途类型 mod by yangrl 2010/12/8 11:36:39
	if (m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_GZHEJ_S)
	{
		setScpLongType();
	}
	
	// 重置 long_group 主被叫都要重求 long_group  changed by liux 2004.11.03 
	// modif by yangyi,国际长途时候 不能重置 2008-2-26
    if ((m_cdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_NATIONAL_OUT) && (m_cdr[ATTRI_LONG_TYPE1] != LONG_TYPE_INTERNATIONAL))
    {
    	T_ATTRI_VAL homeType;
        m_globalManager->getGroupIdByAreaCodeN(m_cdr[ATTRI_VISIT_CODE_A],
         									m_cdr[ATTRI_BEGIN_DATE] + m_cdr[ATTRI_BEGIN_TIME],
         									homeType,
                                            m_cdr[ATTRI_LONG_GROUP1]);
    }
    
	//本地化属性列表
	ProcessBaseHelper::ExecuteProcedure("LocalizeAttribSet");
	
	//设置话单是否有原始费用信息标志
	ProcessBaseHelper::ExecuteProcedure("SetOriginFeeFlag");
	
	
	setUserServiceTypeA();
	
	getExtendAttr();
	
	//sdbss
	//判断智能网是否需要做基本政策和用户政策
	setNoNeedTp();
	
	setErrorCode();

	return 0;
}

void ScpGsmAttribProc::setScpRoamType(void)
{
	//在GetHomeCode里,HOME_TYPE_A 已经定义了是本省用户,国内用户,和国际用户
	//1.省内用户
	if (m_cdr[ATTRI_USER_HOME_TYPE_A] < AREA_TYPE_NATIONAL)
	{
		T_ATTRI_VAL homeType;
		T_ATTRI_VAL longGroup;
		m_cdr[ATTRI_USER_HOME_TYPE_A] = AREA_TYPE_LOCAL;
		if(isRoam())
		{
			if (m_cdr[ATTRI_VISIT_CODE_A].compare(0,2,"00") == 0)
			{
				//到访地错误情况判断为非漫游 add by xiangyang 20090328
				if (m_cdr[ATTRI_VISIT_CODE_A].compare(0,4,"0000") == 0)
				{
					m_cdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NONE;
				}
				else
				{
					m_cdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_INTERNATIONAL_OUT;
				}
			}
			else 
			{
				m_globalManager->getGroupIdByAreaCodeN(m_cdr[ATTRI_VISIT_CODE_A],
													m_cdr[ATTRI_BEGIN_DATE] + m_cdr[ATTRI_BEGIN_TIME],
													homeType,
													longGroup);
				//国内出访 4
				if(homeType == AREA_TYPE_NATIONAL)
				{
					m_cdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NATIONAL_OUT;
				}
				//省内漫游 1
				else
				{
					m_cdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_PRO_IN;
				}                
			}
		}
		else
		{
			//未漫游
			m_cdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NONE;
		}
	}
	//2.国内来访用户
	else if (m_cdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_NATIONAL)
	{
		//国内来访　- 3
		m_cdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NATIONAL_IN;
	}
	//3.国际来访用户
	else
	{
		//国际来访　-5
		m_cdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_INTERNATIONAL_IN;
	}
}

int ScpGsmAttribProc::setScpLongType() 
{
	AttribUNProc::setLongType();
	
	return 0;
}

int ScpGsmAttribProc::setNoNeedTp()
{
    //if (m_cdr[ATTRI_VPN_FLAG] == "Y")
    //{
    //    m_cdr[ATTRI_NEED_BASETP] = FLAG_NO;
    //    m_cdr[ATTRI_NEED_USERTP] = FLAG_NO;
    //}
    //else
    {
        m_cdr[ATTRI_NEED_BASETP] = FLAG_YES;
        m_cdr[ATTRI_NEED_USERTP] = FLAG_YES;
    }
    return 0;
}

