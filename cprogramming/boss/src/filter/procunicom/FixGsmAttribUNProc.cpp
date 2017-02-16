#include "FixGsmAttribUNProc.h"

#include "FilterError.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(FixGsmAttribUNProc);

void FixGsmAttribUNProc::initialattr()
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
	m_patype = PA_OUT; //非区内区间
}

bool FixGsmAttribUNProc::getHomeAreaCodeA()
{
	if(ProcessBaseHelper::ExecuteProcedure("BeforeGetFixHomereaCodeA")==1)
	{
		return true;
	}
	
	int rd;
	int t_iRet;
	T_ATTRI_VAL long_group;
	T_ATTRI_VAL calledType;
	rd = m_globalManager->getHomeAreaCodeByFixPhone(m_pcdr[ATTRI_MSISDN_A],
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
		return true;
	}
	else if(rd<0)
	{
        m_pcdr[ATTRI_ERROR_CODE] = AH_GET_SECTION_ERROR; //获取A营业区失败
        return false;
	}
	else   //固网号段找不到
	{
		t_iRet=ProcessBaseHelper::ExecuteProcedure("GetFixHomereaCodeAByTrunkReason");
		if(t_iRet==1)
		{
			return true;
		}
		else if(t_iRet==2)
		{
			return false;
		}
	}
	
	return true;
}

bool FixGsmAttribUNProc::transNumberProcess()
{
    int transNumbLen = 0;
    int msisdnBLen = 0;
    T_ATTRI_VAL aspTemp;
	
	//默认无接入号
	m_pcdr[ATTRI_DIAL_TYPE] = "0";
	
	//求接入号时，不求对方运营商 mod by yangyi
    m_globalManager->getTransNumberCNC(m_pcdr[ATTRI_MSISDN_B],
		transNumbLen,
		m_pcdr[ATTRI_CARRIER_TYPE],
		m_pcdr[ATTRI_TRAN_TYPE_B],
		aspTemp,
		m_pcdr[ATTRI_DIAL_TYPE],
		m_pcdr[ATTRI_ISNONEED_TP]);
	
    msisdnBLen = m_pcdr[ATTRI_MSISDN_B].length();
    //对特殊号码截位
    if (transNumbLen > msisdnBLen)
    {
        m_pcdr[ATTRI_ERROR_CODE] = TRANS_NUMBER_LEN_OVER;
        return false;
    }
    //将接入号截掉。
    if (transNumbLen > 0)
    {
		  m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_MSISDN_B].substr(transNumbLen,m_pcdr[ATTRI_MSISDN_B].length()-transNumbLen);
		  m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];
    }
	
    //17951+013 的情况
    char sTmp1[24 + 1];
    if ((strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "0", 1) == 0) && (isMobile(m_pcdr[ATTRI_MSISDN_B].c_str() + 1)))
    {
        memset(sTmp1,0,sizeof(sTmp1));
        strcpy(sTmp1,m_pcdr[ATTRI_MSISDN_B].c_str()+1);
        m_pcdr[ATTRI_MSISDN_B] = sTmp1;
    }
	
    //二次拨号以参数表中的ASP为准
    if(m_pcdr[ATTRI_DIAL_TYPE]== "2" || m_pcdr[ATTRI_DIAL_TYPE]== "4" || m_pcdr[ATTRI_DIAL_TYPE]== "6" || m_pcdr[ATTRI_DIAL_TYPE]== "8")
    {
		m_pcdr[ATTRI_USER_ASP_B] = aspTemp;
    }
	
	return true;
}

//特殊号码处理
//   1.从特殊号码表找到匹配
//   0.没有找到匹配
//  -1.参数表出错
int FixGsmAttribUNProc::specialPhoneProcess()
{
    string Stmp,tmp1,tmp2;
    int length;
    int error_code;
    /* calling internal user, there is no special service code */
    if (!strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"00",2))
    {
		return 0;
    }
	
	if (m_globalManager->getSpecialPhoneCNC(m_pcdr[ATTRI_MSISDN_B],
		m_pcdr[ATTRI_HOME_CODE_A],
		m_pcdr[ATTRI_HOME_CODE_B],
		m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
		m_pcdr,
		m_pcdr[ATTRI_USER_TYPE_B],
		m_pcdr[ATTRI_ISNONEED_TP],
		m_pcdr[ATTRI_CALLED_TYPE_B],
		m_pcdr[ATTRI_SPECIAL_PHONE_GROUP],
		m_pcdr[ATTRI_SUM_SERVTYPE],
		length,
		error_code))
	{
		if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
		{
			m_pcdr[ATTRI_ERROR_CODE] = BH_UT_SPHONE_ERROR;
			return -1;
		}
		
		m_pcdr[ATTRI_USER_ASP_B] = m_pcdr[ATTRI_USER_TYPE_B][0];
		m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
		//特殊号码标识
		m_pcdr[ATTRI_SPE_PHONE_FLAG] = ATTRI_SPECIALPHONE_Y;
		m_pcdr[ATTRI_SPEC_SERVICE]   = m_pcdr[ATTRI_USER_TYPE_B][3];
		
		//Added By Sunam 2006-8-8 13:39 如果length>0,则other_party只保留前length位
		if (length > 0)
		{
			//考虑可能有接入号等号码在内，已经被截去
			length += m_pcdr[ATTRI_MSISDN_B_FOR_ROAM].length() - m_pcdr[ATTRI_MSISDN_B].length();
			m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B_FOR_ROAM].substr(0,length);
		}
		return 1;
	}
	else
	{
		if(error_code>0)
		{
			m_pcdr[ATTRI_ERROR_CODE] = error_code;
			return -1;
		}
		return 0;
	}
}

bool FixGsmAttribUNProc::setFixLongType()
{
	///求A的省内通道号
	if(m_globalManager->getProvChannelByCitYCode(m_pcdr[ATTRI_HOME_CODE_A],m_channelNo))
	{
		m_pcdr[ATTRI_CITY_CHANNEL] = m_channelNo;
	}
	
    if (isSameArea(m_pcdr[ATTRI_HOME_CODE_A],m_pcdr[ATTRI_HOME_CODE_B]))
    {
        m_pcdr[ATTRI_LONG_TYPE1] = LONG_TYPE_NONE;   //非长途
    }
    else
    {
        if (!strncmp(m_pcdr[ATTRI_HOME_CODE_B].c_str(),"00",2))
            m_pcdr[ATTRI_LONG_TYPE1] = LONG_TYPE_INTERNATIONAL; //国际长途
        else
        {
			if (m_pcdr[ATTRI_USER_HOME_TYPE_B] < AREA_TYPE_NATIONAL )
				m_pcdr[ATTRI_LONG_TYPE1] = LONG_TYPE_PROV; //省内长途
			else
				m_pcdr[ATTRI_LONG_TYPE1] = LONG_TYPE_NATIONAL; //国内长途
        }
    }
	return true;
}

bool FixGsmAttribUNProc::setSameSubAreaType()
{
	int rd;
	T_ATTRI_VAL phoneB;
	if (!isSameArea(m_pcdr[ATTRI_HOME_CODE_A],m_pcdr[ATTRI_HOME_CODE_B]))
		return false;
	
	if(isMobile(m_pcdr[ATTRI_MSISDN_B]))
	{
		if(!m_globalManager->getSectionByMsisdn(m_pcdr[ATTRI_HOME_CODE_B],m_pcdr[ATTRI_MSISDN_B],
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			m_pcdr[ATTRI_SUB_HOMECODE_B]))
		{
			m_pcdr[ATTRI_PA_TYPE] = PA_IN; //区内
			return true;
		}
	}
	if(m_pcdr[ATTRI_SUB_HOMECODE_A].empty())
	{
		m_pcdr[ATTRI_PA_TYPE] = PA_IN; //区内
		return true;
	}
	
	
	m_globalManager->getSpecialPhoneSection(m_pcdr[ATTRI_MSISDN_B],m_pcdr[ATTRI_HOME_CODE_A],m_pcdr[ATTRI_HOME_CODE_B],
		m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
		m_pcdr[ATTRI_SUB_HOMECODE_B]);
	
	if(strncmp(m_pcdr[ATTRI_MSISDN_B_FOR_ROAM].c_str(), "0", 1) == 0)
		phoneB = m_pcdr[ATTRI_MSISDN_B_FOR_ROAM];
	else
		phoneB = m_pcdr[ATTRI_MSISDN_B];
	
	if(m_globalManager->getRelationByPhoneandSection(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_SUB_HOMECODE_A],
		phoneB,m_pcdr[ATTRI_SUB_HOMECODE_B],m_pcdr[ATTRI_HOME_CODE_B],
		m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
		m_pcdr[ATTRI_PA_TYPE]))
	{
		return true;
	}
	else if(isSameArea(m_pcdr[ATTRI_SUB_HOMECODE_A],m_pcdr[ATTRI_SUB_HOMECODE_B]))
	{
		m_pcdr[ATTRI_PA_TYPE] = PA_IN; //区内
		return true;
	}
	else
	{
		if(m_pcdr[ATTRI_SUB_HOMECODE_B].empty())
		{
			m_pcdr[ATTRI_PA_TYPE] = PA_IN; //区内
			return true;
		}
		else
		{
			m_pcdr[ATTRI_PA_TYPE] = PA_BETWEEN; //区外
			return true;
		}
	}
	
	return false;
}

void FixGsmAttribUNProc::setFixHomeAreaCodeB(const T_ATTRI_VAL &area_codeB)
{
    T_ATTRI_VAL homeType;
    T_ATTRI_VAL long_group;
	if(strncmp(area_codeB.c_str(), "0", 1) == 0 &&
		m_globalManager->getGroupIdByAreaCodeN(area_codeB,
		m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
		homeType,long_group))
    {
		m_pcdr[ATTRI_HOME_CODE_B] = area_codeB;
    }
    //求取失败 默认本地通话
    else if(strncmp(m_pcdr[ATTRI_HOME_CODE_B].c_str(), "0", 1))
    {
		m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_HOME_CODE_A];
    }
}

bool FixGsmAttribUNProc::getSpecialPhoneGroup()
{
/*
有些号码只是对某种情况的通话可能产生不同的费率政策，而这些号码又不太适合单独给它一种业务类型
和统计也没有什么关系，因此另建一张表特殊号码组表，为每一组特殊号码定义一个组号做为话单的一个属性
放在政策的约束条件中，此属性不用显示在用户的清单中
	*/
	int group;
	if(m_globalManager->getSpecialPhoneGroupCNC(m_pcdr[ATTRI_MSISDN_B],m_pcdr[ATTRI_HOME_CODE_B],
		m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],group))
	{
		m_pcdr[ATTRI_SPECIAL_PHONE_GROUP] = group;
	}
	return true;
}

int FixGsmAttribUNProc::execute()
{
#ifdef FP_DEBUG
	FP_BEGIN(FixGsmAttribUNProc)
#endif
		
		//无效话单直接返回
		if(m_pcdr[ATTRI_TRASH_FLAG]>=ATTRI_TRASH_Y)
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
		//specialPreProcessCdr();
		
		//求A归属地
		if(!getHomeAreaCodeA())
		{
			return 1;
		}
		
		//对端号码预先标准化
		ProcessBaseHelper::ExecuteProcedure("BeforeFixOtherPartyStand");
		T_ATTRI_VAL homecodeb = m_pcdr[ATTRI_HOME_CODE_B];
		//对端号码标准化
		otherPartyStandardization();
		
		//处理接入号码,如17951等
		if (!transNumberProcess())
		{
			return 1;
		}
		
		//求对方号码属性
		if (!getHomeAreaCodeB())
		{
			//求对方归属错，在td_fixphone_seg里再查找
			if(getFixTelephoneSeg()==0)
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
		else if(getFixTelephoneSeg()<0)
		{
			return 1;
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
				if(getFixTelephoneSeg()<0)
					return 1;
			}
			
			//规整对端号归属地
			setFixHomeAreaCodeB(homecodeb);
			
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
		
		//求长途类型
		setFixLongType();
		
    //求区间类型
		setSameSubAreaType();
		//本地化规整区内区间
		ProcessBaseHelper::ExecuteProcedure("LocalizeSubAreaTypeSet");
		
		//本地化属性列表
		ProcessBaseHelper::ExecuteProcedure("LocalizeFixAttribSet");
		
		//设置话单是否有原始费用信息标志
		ProcessBaseHelper::ExecuteProcedure("SetOriginFeeFlag");
		
		//A业务类型调整
		setUserServiceTypeA();
		
		getExtendAttr();
		
		//求完属性后，在这儿可以判断那些错单，不再需要批价处理，在方法里直接置上错误代码
		setErrorCode();
		
#ifdef FP_DEBUG
		FP_END(FixGsmAttribUNProc)
#endif
			return 0;
}
