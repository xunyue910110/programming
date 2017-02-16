#include "MiscAttribProc.h"

#include "PMGlobalManager.h"
#include "FilterError.h"

INSTALL_PROCESSBASE_INSTANCE(MiscAttribProc);

int MiscAttribProc::execute()
{
	//错单,不进行处理
	if (!isValidCDR())
	{
		return 1;
	}
	
	//求A归属地
	if(isSelfNumber(m_pcdr[ATTRI_MSISDN_A]))
	{
		if (!getAHByMsisdn(m_pcdr[ATTRI_MSISDN_A]))
		{
			return false;
		}
	}
	
	//Added by Sunam 2005-12-16 国际彩信求一下对端归属地,求不到也不判错
	if ((m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_MISC_MMS) ||
		(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_MISC_IMMS) ||
		(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_MISC_LMMS))
	{
		T_ATTRI_VAL longGroup1,longGroup2,longGroup3,longGroup4,long_groupgprs,userTypeB,provinceCode,calledTypeB;
		
		//receiveAddress如果是00开始，去查td_inter_fee_code得called_code,和b_home_type
		if (!(strncmp(m_pcdr[ATTRI_MISC_STR2].c_str(),"00",2)) && (strncmp(m_pcdr[ATTRI_MISC_STR2].c_str(),"000",3)))
    	{
			if (m_globalManager->getHomeAreaCodeByMsisdnI(m_pcdr[ATTRI_MISC_STR2], 
                                                   m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME], 
                                                   m_pcdr[ATTRI_HOME_CODE_B],
                                                   longGroup1,longGroup2, longGroup3,longGroup4,long_groupgprs,userTypeB,
                                                   m_pcdr[ATTRI_NATIONAL_CODE_B]))
        	{
        		m_pcdr[ATTRI_USER_HOME_TYPE_B] = userTypeB[3];
        	}
        	else
        	{
        		m_pcdr[ATTRI_ERROR_CODE] = DEFAULT_ERROR;
        	}
        }
        //国内用户手机号码
        else if (isSelfNumber(m_pcdr[ATTRI_MISC_STR2]))
        {
			if(m_globalManager->getHomeAreaCodeByPhoneN(m_pcdr[ATTRI_MISC_STR2], 
									m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
									m_pcdr[ATTRI_HOME_CODE_B],longGroup1,
									userTypeB,provinceCode,calledTypeB))
			{
				m_pcdr[ATTRI_USER_HOME_TYPE_B] = userTypeB[3];
			}
			else
        	{
        		m_pcdr[ATTRI_ERROR_CODE] = DEFAULT_ERROR;
        	}
        }
        #ifdef _PRICE_TRACE_DUMP_
        	cout << " m_pcdr[ATTRI_MISC_STR2  ] : " << m_pcdr[ATTRI_MISC_STR2] << endl;
        	cout << " m_pcdr[ATTRI_BEGIN_DATE ] : " << m_pcdr[ATTRI_BEGIN_DATE] << endl;
        	cout << " m_pcdr[ATTRI_BEGIN_TIME ] : " << m_pcdr[ATTRI_BEGIN_TIME] << endl;
        	cout << " m_pcdr[ATTRI_HOME_CODE_B] : " << m_pcdr[ATTRI_HOME_CODE_B] << endl;
        #endif 
	}
	
	//设置话单是否有原始费用信息标志
	ProcessBaseHelper::ExecuteProcedure("SetOriginFeeFlag");
	
	//Added by Sunam 2007-3-22 10:07
	if (m_Config->m_provinceCode == "731")
	{
		//湖南彩铃如果月租月份和账期不符判为错单
		if (m_pcdr[ATTRI_SOURCE_TYPE] == "4N")
		{
			if (atoi(m_pcdr[ATTRI_CYCLE_DATE].substr(4,2).c_str()) !=	m_pcdr[ATTRI_MISC_NUM2].value())
			{
				m_pcdr[ATTRI_ERROR_CODE] = SPECIAL_ERROR_CODE1;
				return false;
			}
		}
	}
	
	return 0;
	
}

