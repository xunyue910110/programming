#include "FixGsmCDRUNProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(FixGsmCDRUNProc);

bool FixGsmCDRUNProc::InitCdrProperty()
{
	reset();
	m_pcdr[ATTRI_USER_ASP_A]        = COMPANY_TYPE_UNKNOWN;
	m_pcdr[ATTRI_MSISDN_B]          = m_pcdr[ATTRI_MSISDN_B_ORG];
	m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B_ORG];
	
	return true;
}

bool FixGsmCDRUNProc::getTrash()
{
	int t_iRet;
	//本地化gsm trash判断
	t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeFixGsmTrashGet");
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

bool FixGsmCDRUNProc::getDisChannelProperty()
{
	int t_iRet;
	MSC_TYPE    r_msc_type;
	
	//1. 无效话单
	if(getTrash())
		return true;
	
	//2. 错误话单
	if(getError())
		return true;
	
	InitCdrProperty(); //先规整号码
	
	//本地化特定数据源判断
	t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeFixGsmNormalSet");
	if(t_iRet==0)
	{
		return true;
	}
	else if(t_iRet==1)
	{
		m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
		return false;
	}
	else if(t_iRet==9)
	{
		m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_2;
		return false;
	}
	
	//3. 固网交换机号求取不到，判错
	r_msc_type = FIX_MSC;
	if(!m_globalManager->getAVByMsc(m_pcdr[ATTRI_MSC],r_msc_type,
		m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
		m_pcdr[ATTRI_VISIT_CODE_A]))
	{
		m_pcdr[ATTRI_ERROR_CODE] = AH_FIX_MSC_ERROR;
		return false;
	}
	
	//4. 中继计费判断
	t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeFixGsmgetTrunkProperty");
	if(t_iRet==0)
	{
		return true;
	}
	else if(t_iRet==1)
	{
		m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
		return false;
	}
	
	//5. 计费号码调整
	t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeFixGsmSetBillNumber");
	if(t_iRet==0)
	{
		return true;
	}
	else if(t_iRet==1)
	{
		m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
		return false;
	}
	
	//6. 根据真实计费号码剔单
	t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeFixGsmTrashNumberNbr");
	if(t_iRet==0)
	{
		return true;
	}
	else if(t_iRet==1)
	{
		m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
		return false;
	}
	else if(t_iRet==9)
	{
		m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_2;
		return false;
	}
	
	//7. 计费号码规整
	if(!StandFixMsisdnA())
		return false;
	
	return true;
}


//号码规整为区号+号码
bool FixGsmCDRUNProc::StandFixMsisdnA()
{
	int rd;
	int t_iRet;
	T_ATTRI_VAL r_homecode;
	T_ATTRI_VAL r_longgroup;
	T_ATTRI_VAL r_usertype;
	T_ATTRI_VAL r_msisdn_a;
	
	if (m_pcdr[ATTRI_MSISDN_A].length() <3)
	{
		return true;
	}
	if (m_pcdr[ATTRI_MSISDN_A][0] == '0' && m_pcdr[ATTRI_MSISDN_A][1] != '0')
	{
		rd = m_globalManager->getHomeAreaCodeByMsisdnN(m_pcdr[ATTRI_MSISDN_A],m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			m_pcdr[ATTRI_VISIT_CODE_A_ORG],r_longgroup,r_usertype);
		if(rd >0 && m_globalManager->isProvInnerCode(m_pcdr[ATTRI_VISIT_CODE_A_ORG]))
		{
			//不需要进行同一地区校验
			t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeFixGsmSameMscCheck");
			if(t_iRet==1)
			{
				return true;
			}
			else if(t_iRet==2)
			{
				return false;
			}
			return true;
		}
	}
	//modify by dph，A号码长度判断改为6
	//else if(m_pcdr[ATTRI_MSISDN_A].length()>=10 && m_pcdr[ATTRI_MSISDN_A][0] != '0')
	else if(m_pcdr[ATTRI_MSISDN_A].length()>=6 && m_pcdr[ATTRI_MSISDN_A].length()!=7 && m_pcdr[ATTRI_MSISDN_A][0] != '0')
	{
		r_msisdn_a = m_pcdr[ATTRI_MSISDN_A];
		r_msisdn_a.append(0,"0");
		rd = m_globalManager->getHomeAreaCodeByMsisdnN(r_msisdn_a,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			m_pcdr[ATTRI_VISIT_CODE_A_ORG],r_longgroup,r_usertype);
		if(rd >0 && m_globalManager->isProvInnerCode(m_pcdr[ATTRI_VISIT_CODE_A_ORG]))
		{
			m_pcdr[ATTRI_MSISDN_A] = r_msisdn_a;
			//不需要进行同一地区校验
			t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeFixGsmSameMscCheck");
			if(t_iRet==1)
			{
				return true;
			}
			else if(t_iRet==2)
			{
				return false;
			}
			return true;
		}
	}
	//modify by dph
	//else if(m_pcdr[ATTRI_MSISDN_A].length()>=7 && m_pcdr[ATTRI_MSISDN_A].length()<=8)
	else
	{
		m_pcdr[ATTRI_MSISDN_A] = m_pcdr[ATTRI_VISIT_CODE_A] + m_pcdr[ATTRI_MSISDN_A];
		return true;
	}
	
	return true;
}

//先求通道后求属性
int FixGsmCDRUNProc::execute()
{
#ifdef FP_DEBUG
	FP_BEGIN(FixGsmCDRUNProc)
#endif
		
		//求分通道必要属性
		getDisChannelProperty();
	
#ifdef FP_DEBUG
	FP_END(FixGsmCDRUNProc)
#endif
		return 0;
}

