#include "LocalizeAttribProc_010.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeAttribProc034);

int LocalizeAttribProc034::execute()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeAttribSet);

int LocalizeAttribSet::execute()
{
	//江苏联通语音 IP特殊处理 将智能网类别06转换成01类型
	if ((m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GSM || m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_UNICOMIP) &&
		m_pcdr[ATTRI_USER_SERVICE_TYPE_B] == SERVICE_TYPE_SSP)
	{
		m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SERVICE_TYPE_GSM;
	}
	if(m_pcdr[ATTRI_SUM_SERVTYPE].empty())
	{
		m_pcdr[ATTRI_SUM_SERVTYPE] = m_pcdr[ATTRI_USER_SERVICE_TYPE_B];
	}
	
	//双模话单，G网下C网话单，置CTOG标记
	if (m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_GSM && m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == SERVICE_TYPE_CDMA)
	{
		m_pcdr[ATTRI_DOUBLEMODE] = DOUBLEMODE_CTOG;
	}

  //虚拟网 在所属区号内相互通话均判为本地通话
  if(m_globalManager->isVirtualArea(m_pcdr[ATTRI_HOME_CODE_A],m_pcdr[ATTRI_HOME_CODE_B],
          m_pcdr[ATTRI_USER_HOME_TYPE_A],m_pcdr[ATTRI_USER_ASP_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME]))
  {
  	m_pcdr[ATTRI_LONG_TYPE1] = LONG_TYPE_NONE;
  	m_pcdr[ATTRI_LONG_GROUP1] = DEFAULT_GROUP_ID_B;
  }

	return 0;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(CdrAttribInit);

int CdrAttribInit::execute()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(BeforeOtherPartyStand);
//对端号码提前规整
int BeforeOtherPartyStand::execute()
{
	int rd;
	T_ATTRI_VAL r_homecode;
	T_ATTRI_VAL r_longgroup;
	T_ATTRI_VAL r_usertype;
	T_ATTRI_VAL r_msisdn_b;
	int transNumbLen = 0;
	int msisdnALen = 0;
	T_ATTRI_VAL tmpTranType;
	T_ATTRI_VAL tmpCarrierType;
	T_ATTRI_VAL tmpUserAspB;
	T_ATTRI_VAL tmpDialType;
	T_ATTRI_VAL tmpUserType;
	T_ATTRI_VAL tmpIsnoneedTp;
	T_ATTRI_VAL called_type;
	T_ATTRI_VAL groupId;
	T_ATTRI_VAL sum_servtype;
	int length;
	int error_code;
	
	//去除区号
	if (m_pcdr[ATTRI_MSISDN_B].length() <5)
	{
		return 2;
	}
	
	//"00"+手机号码开始，则截去00 2008-12-26 15:20:01
	if ((strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "00", 2) == 0) && (isMobile(m_pcdr[ATTRI_MSISDN_B].c_str() + 2)))
	{
		m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_MSISDN_B].c_str() + 2;
		m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];
	}
	
	//手机号码截取前11位
	if(isMobile(m_pcdr[ATTRI_MSISDN_B]) && m_pcdr[ATTRI_MSISDN_B].length()>11)
	{
		m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_MSISDN_B].substr(0,11);
		m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];
		return 2;
	}
	
	//固话区号补0
	r_msisdn_b = m_pcdr[ATTRI_MSISDN_B];
	
	if(m_globalManager->getSpecialPhone(r_msisdn_b,m_pcdr[ATTRI_HOME_CODE_A],m_pcdr[ATTRI_HOME_CODE_B],m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
		m_pcdr,tmpUserType,tmpIsnoneedTp,called_type,groupId,sum_servtype,length,error_code))
	{
		//特殊号码标识
		m_pcdr[ATTRI_SPE_PHONE_FLAG] = ATTRI_SPECIALPHONE_Y;
	}
	if(r_msisdn_b.length()>=10 && !isMobile(r_msisdn_b) && 
		!m_globalManager->getTransNumber(r_msisdn_b,transNumbLen,tmpCarrierType,tmpTranType,tmpUserAspB,tmpDialType) &&
		m_pcdr[ATTRI_SPE_PHONE_FLAG] == ATTRI_SPECIALPHONE_N)
	{
		//漫游出访话单不在此补0处理
		if(m_pcdr[ATTRI_SOURCE_TYPE].in(SOURCE_TYPE_GSM_CRO,SOURCE_TYPE_GSM_CIO,SOURCE_TYPE_GSM_GBO))
			return 2;
		r_msisdn_b = r_msisdn_b.append(0,"0");
		rd = m_globalManager->getHomeAreaCodeByMsisdnN(r_msisdn_b,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			r_homecode,r_longgroup,r_usertype);
		//if(rd >0 && m_globalManager->isProvInnerCode(r_homecode))
		if(rd >0)
		{
			//主叫 对端号码1开头不规整 add by shenglj 2009-03-01 23:58:00
			if(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MOC && strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "1", 1) == 0)
			{
				return 2;
			}
			m_pcdr[ATTRI_MSISDN_B] = r_msisdn_b;
			m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];
			return 2;
		}
	}
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(AfterSetMocLongType);

int AfterSetMocLongType::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(AfterForwLongType);

int AfterForwLongType::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(BeforeGetVisitAreaCodeA);

int BeforeGetVisitAreaCodeA::execute()
{
	//PPS语音话单A到访，如果非区号，则重新求取
	if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_PPS)
	{
		T_ATTRI_VAL homeType;
		T_ATTRI_VAL long_group;
		if(m_globalManager->getGroupIdByAreaCodeN(m_pcdr[ATTRI_VISIT_CODE_A],
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			homeType,long_group))
		{
			m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
			return 1;
		}
	}

	// Begin add by xiezq 20101207 for 集团BSSV4.1  一卡双号需求
	//一卡双号特殊处理
	if(m_pcdr[ATTRI_SOURCE_TYPE].in("1E"))
	{
		if(!m_pcdr[ATTRI_VISIT_CODE_A].length())
		{
			m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
		}
		return 1;
	}
	// End add by xiezq 20101207 for 集团BSSV4.1  一卡双号需求
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(AfterGetVisitAreaCodeA);

int AfterGetVisitAreaCodeA::execute()
{
	//IVPN话单A到访求取失败，如果非区号，则重新求取
	if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_IVPN)
	{
		T_ATTRI_VAL homeType;
		T_ATTRI_VAL long_group;
		if(m_globalManager->getGroupIdByAreaCodeN(m_pcdr[ATTRI_VISIT_CODE_A],
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			homeType,long_group))
		{
			m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
			return 1;
		} 
	}
	
	// 互转且前转原因为无条件呼转或不可及互转，将到访地置为归属地，tanhao add, 2009-5-1
	if(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW)
	{
		if((m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNCONDITIONAL ||
			m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNREACHABLE) &&
			m_pcdr[ATTRI_ERROR_CODE] == DEFAULT_ERROR)
		{
			m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
			return 1;
		}
	}
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(BeforeGetVisitAreaCodeB);

int BeforeGetVisitAreaCodeB::execute()
{	
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(AfterGetVisitAreaCodeB);

int AfterGetVisitAreaCodeB::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(AfterSetRoamType);

int AfterSetRoamType::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(SetOriginFeeFlag);

int SetOriginFeeFlag::execute()
{

/*
    if (!msisdn_hk())
    {
        imsi_hk();
    }
*/
    return 0;
}

// Begin add by xiezq 20101207 for 集团BSSV4.1  一卡双号需求
// 一卡双号MSISDN
bool SetOriginFeeFlag::msisdn_hk()
{
	PO_MsisdnHk po;
	vector<PO_MsisdnHk*> rpo;
	vector<PO_MsisdnHk*>::iterator itr;

	po.recordStruct.begin_phone = m_pcdr[ATTRI_MSISDN_A];
	po.recordStruct.end_phone = m_pcdr[ATTRI_MSISDN_A];

	m_PO_MsisdnHkContainer = (ParamContainer<PO_MsisdnHk>*) PMHelperall::GetParamContainer("PO_MsisdnHk");
	if (m_PO_MsisdnHkContainer->searchMulti(po,rpo) <= 0)
	{
	    return false;
	}

    T_DATE_TIME beginTime = m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME];
	for (itr = rpo.begin(); itr != rpo.end(); ++itr)
	{
		if(beginTime >= (*itr)->recordStruct.beginDate &&
			beginTime <= (*itr)->recordStruct.endDate)
		{
			m_pcdr[ATTRI_SPEC_SERVICE]=(*itr)->recordStruct.usertype;
			return true;
		}
	}
	return false;
}

// 一卡双号IMSI
bool SetOriginFeeFlag::imsi_hk()
{
	PO_ImsiHk po;
	vector<PO_ImsiHk*> rpo;
	vector<PO_ImsiHk*>::iterator itr;

	po.recordStruct.begin_imsi = m_pcdr[ATTRI_IMSI_NUMBER];
	po.recordStruct.end_imsi = m_pcdr[ATTRI_IMSI_NUMBER];
	m_PO_ImsiHkContainer = (ParamContainer<PO_ImsiHk>*) PMHelperall::GetParamContainer("PO_ImsiHk");
	if (m_PO_ImsiHkContainer->searchMulti(po,rpo) <= 0)
	{
	    return false;
	}

    T_DATE_TIME beginTime = m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME];
	for (itr = rpo.begin(); itr != rpo.end(); ++itr)
	{
		if(beginTime >= (*itr)->recordStruct.beginDate &&
			beginTime <= (*itr)->recordStruct.endDate)
		{
			m_pcdr[ATTRI_SPEC_SERVICE]=(*itr)->recordStruct.usertype;
			return true;
		}
	}
	return false;
}
// End add by xiezq 20101207 for 集团BSSV4.1  一卡双号需求

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(CdrAttribEnd);

int CdrAttribEnd::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeErrorCdrProc);

int LocalizeErrorCdrProc::execute()
{
	//modify by dph,此处改在int LocalizeProvRegionInfoGet::execute()里面实现
	/*//add by dph
	//本地主叫长途小于三秒，判为错单
	if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FIXVOICE && m_pcdr[ATTRI_DURATION] < DURATION_FREE &&m_pcdr[ATTRI_LONG_TYPE1] > LONG_TYPE_NONE)
	{
		m_pcdr[ATTRI_ERROR_CODE] = GSM_ERROR_FREE3S;
	}//end add by dph*/
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(IsNoNeedTPSpecialProc);

int IsNoNeedTPSpecialProc::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalEdgeRoamProc);
// tanhao add, 20090420
int LocalEdgeRoamProc::execute()
{
	//省内边漫 拨打AV=BH算漫游，AV!=BH 算本地 by shenglj 2009-01-13 21:51:01
	if(m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_PORVINNER)
	{
		m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
	}
	//省际边漫算本地
	else if(m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_INTERPROV)
	{
     	  m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
		  //m_pcdr[ATTRI_CELLID_A]     = "99999";
	}
	//集团下发CRO文件边漫算本地
	else if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GSM_CRO && m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_LOCAL_INTERPROV)
	{
		m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
		//m_pcdr[ATTRI_CELLID_A]     = "99999";
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeIPCdrSetProc);

int LocalizeIPCdrSetProc::execute()
{
	return 0;
}


///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeIPPAAreaSetProc);

int LocalizeIPPAAreaSetProc::execute()
{	
	return 0;
}



///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeIPSwitchSetProc);

int LocalizeIPSwitchSetProc::execute()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeGprsSpecialProc);

int LocalizeGprsSpecialProc::execute()
{
	setdecTohexAddr(m_pcdr[ATTRI_GPRS_SGSN],m_pcdr[ATTRI_GPRS_SGSN_HEX]);
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeSpSpecialProc);

int LocalizeSpSpecialProc::execute()
{
	m_PO_SpServCodeSpnameContainer  = (ParamContainer<PO_SpServCodeSpname>*) PMHelperall::GetParamContainer("PO_SpServCodeSpname");
	m_PO_ServiceCodeSpnameContainer = (ParamContainer<PO_ServiceCodeSpname>*) PMHelperall::GetParamContainer("PO_ServiceCodeSpname");
	
	m_spProperty.begin_time   = m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME];
	m_spProperty.end_time     = m_pcdr[ATTRI_END_DATE]+m_pcdr[ATTRI_END_TIME];
	
	//联通在信
	if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_PSMG || m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_NSMG)
	{
		if (getSpServCodeName(m_pcdr[ATTRI_CALL_TYPE],m_pcdr[ATTRI_SP_CODE],m_pcdr[ATTRI_OPER_CODE],m_pcdr[ATTRI_SERVICE_CODE],m_spProperty))
		{
			m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]    = m_spProperty.service_code;
			m_pcdr[ATTRI_USER_SERVICE_TYPE_B]  = m_spProperty.serv_type;
			m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B]  = m_spProperty.serv_code_type;
			m_pcdr[ATTRI_USER_ASP_B]           = m_spProperty.asp;
			m_pcdr[ATTRI_SM_TYPESTAT]          = m_spProperty.sm_typestat;
			m_pcdr[ATTRI_USER_HOME_TYPE_B]     = m_spProperty.home_type;
			m_pcdr[ATTRI_SP_SERV_AREA]         = m_spProperty.sp_serv_area;
			m_pcdr[ATTRI_SP_PROV_CODE]         = m_spProperty.prov_code;
			m_pcdr[ATTRI_SP_NAME]              = m_spProperty.sp_name;
			return 1;
		}
		else
		{
			m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_SPCODE;
			return 1;
		}
	}
	
	//PPS联通在信上行的话单中PDSCP不提供企业代码，由计费进行匹配，对于那种一个接入码对应多个企业代码的情况将随机进行匹配
	if((m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_PPS1 || m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_PPS2) && m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMO)
	{
		//1. 智能网点对点短信-A2，上行话单发移动、联通、小灵通之外的话单，SERVICE_CODE填OTHER_PARTY
		//2. 智能网联通在信-A3，上行话单发移动、联通、小灵通的话单作无效处理，其余的话单SERVICE_CODE填OTHER_PARTY
		m_pcdr[ATTRI_SERVICE_CODE] = m_pcdr[ATTRI_MSISDN_B];
		if (getSpServCodeName(m_pcdr[ATTRI_CALL_TYPE],m_pcdr[ATTRI_SERVICE_CODE],m_spProperty))
		{
			m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]    = m_spProperty.service_code;
			m_pcdr[ATTRI_USER_SERVICE_TYPE_B]  = m_spProperty.serv_type;
			m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B]  = m_spProperty.serv_code_type;
			m_pcdr[ATTRI_USER_ASP_B]           = m_spProperty.asp;
			m_pcdr[ATTRI_SM_TYPESTAT]          = m_spProperty.sm_typestat;
			m_pcdr[ATTRI_USER_HOME_TYPE_B]     = m_spProperty.home_type;
			m_pcdr[ATTRI_SP_SERV_AREA]         = m_spProperty.sp_serv_area;
			m_pcdr[ATTRI_SP_PROV_CODE]         = m_spProperty.prov_code;
			m_pcdr[ATTRI_SP_NAME]              = m_spProperty.sp_name;
			return 1;
		} 
		else
		{
			m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]    = m_pcdr[ATTRI_SERVICE_CODE];
			m_pcdr[ATTRI_USER_SERVICE_TYPE_B]  = SM_SERV_TYPE_SP;
			m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B]  = "0";
			m_pcdr[ATTRI_USER_ASP_B]           = m_Config->m_operatorType;
			m_pcdr[ATTRI_SM_TYPESTAT]          = "PPS";
			m_pcdr[ATTRI_USER_HOME_TYPE_B]     = AREA_TYPE_LOCAL;
			m_pcdr[ATTRI_SP_SERV_AREA]         = "0";
			m_pcdr[ATTRI_SP_PROV_CODE]         = m_Config->m_provinceCode;
			m_pcdr[ATTRI_SP_CODE]              = "99999";
			m_pcdr[ATTRI_SP_NAME]              = "PDSCP";
			m_pcdr[ATTRI_SERVICE_CODE]         = "PDSCP";
			return 1;
		}
	}
	
	//PPS联通在信下行话单 用SP_CODE OPER_CODE SERVICE_CODE去匹配
	if((m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_PPS1 || m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_PPS2) && m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMT)
	{
		if (getSpServCodeName(m_pcdr[ATTRI_CALL_TYPE],m_pcdr[ATTRI_SP_CODE],m_pcdr[ATTRI_OPER_CODE],m_pcdr[ATTRI_SERVICE_CODE],m_spProperty))
		{
			m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]    = m_spProperty.service_code;
			m_pcdr[ATTRI_USER_SERVICE_TYPE_B]  = m_spProperty.serv_type;
			m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B]  = m_spProperty.serv_code_type;
			m_pcdr[ATTRI_USER_ASP_B]           = m_spProperty.asp;
			m_pcdr[ATTRI_SM_TYPESTAT]          = m_spProperty.sm_typestat;
			m_pcdr[ATTRI_USER_HOME_TYPE_B]     = m_spProperty.home_type;
			m_pcdr[ATTRI_SP_SERV_AREA]         = m_spProperty.sp_serv_area;
			m_pcdr[ATTRI_SP_PROV_CODE]         = m_spProperty.prov_code;
			m_pcdr[ATTRI_SP_NAME]              = m_spProperty.sp_name;

			return 1;
		}
		else
		{
			m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]    = m_pcdr[ATTRI_SERVICE_CODE];
			m_pcdr[ATTRI_USER_SERVICE_TYPE_B]  = SM_SERV_TYPE_SP;
			m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B]  = "0";
			m_pcdr[ATTRI_USER_ASP_B]           = m_Config->m_operatorType;
			m_pcdr[ATTRI_SM_TYPESTAT]          = "PPS";
			m_pcdr[ATTRI_USER_HOME_TYPE_B]     = AREA_TYPE_LOCAL;
			m_pcdr[ATTRI_SP_SERV_AREA]         = "0";
			m_pcdr[ATTRI_SP_PROV_CODE]         = m_Config->m_provinceCode;
			m_pcdr[ATTRI_SP_NAME]              = "PDSCP";
			m_pcdr[ATTRI_SERVICE_CODE]         = "PDSCP";
			return 1;
		}
	}
	
	return 0;
}

bool LocalizeSpSpecialProc::getSpServCodeName(const T_ATTRI_VAL &r_call_type,
											  const T_ATTRI_VAL &r_sp_code,
											  const T_ATTRI_VAL &r_oper_code,
											  const T_ATTRI_VAL &r_service_code,
											  SpProperty &r_spProperty)
{
	PO_SpServCodeSpname po;
	vector<PO_SpServCodeSpname*> rpo;
	vector<PO_SpServCodeSpname*>::iterator itr;
	
	po.recordStruct.sp_code=r_sp_code;
	po.recordStruct.oper_code=r_oper_code;
	if (m_PO_SpServCodeSpnameContainer->searchMulti(po,rpo)<=0)
	{
		return false;
	}
	if(r_call_type.length()<2)
	{
		return false;
	}
	
	//按oper_code重新排序
	::sort(rpo.begin(),rpo.end(),SelfSort());
	for (itr=rpo.begin(); itr!=rpo.end(); itr++)
	{
		if(r_spProperty.begin_time>=(*itr)->recordStruct.begin_time &&
			r_spProperty.end_time<=(*itr)->recordStruct.end_time &&
			((*itr)->recordStruct.call_type[0]==r_call_type[1] || (*itr)->recordStruct.call_type=="*") &&
			(strcmp(r_oper_code.c_str(),(*itr)->recordStruct.oper_code.c_str())==0 || (*itr)->recordStruct.oper_code=="*") &&
			strncmp(r_service_code.c_str(),(*itr)->recordStruct.service_code.c_str(),(*itr)->recordStruct.service_code.length())==0)
		{
			r_spProperty=(*itr)->recordStruct;
			return true;
		}
	}
	return false;
}

bool LocalizeSpSpecialProc::getSpServCodeName(const T_ATTRI_VAL &r_call_type,
											  const T_ATTRI_VAL &r_service_code,
											  SpProperty &r_spProperty)
{
	PO_ServiceCodeSpname po;
	vector<PO_ServiceCodeSpname*> rpo;
	vector<PO_ServiceCodeSpname*>::iterator itr;
	
	po.recordStruct.service_code=r_service_code;
	if (m_PO_ServiceCodeSpnameContainer->searchMulti(po,rpo)<=0)
	{
		return false;
	}
	if(r_call_type.length()<2)
	{
		return false;
	}
	
	for (itr=rpo.begin(); itr!=rpo.end(); itr++)
	{
		if(r_spProperty.begin_time>=(*itr)->recordStruct.begin_time &&
			r_spProperty.end_time<=(*itr)->recordStruct.end_time &&
			((*itr)->recordStruct.call_type[0]==r_call_type[1] || (*itr)->recordStruct.call_type=="*"))
		{
			r_spProperty=(*itr)->recordStruct;
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeSpBillRuleProc);

int LocalizeSpBillRuleProc::execute()
{
	m_PO_PO_SpBillRuleContainer = (ParamContainer<PO_SpBillRule>*) PMHelperall::GetParamContainer("PO_SpBillRule");
	
	//不符合代收费规则，置错单
	if(!SpCdrValid(m_pcdr[ATTRI_SOURCE_TYPE],
		m_pcdr[ATTRI_SP_CODE],m_pcdr[ATTRI_SP_SRVTYPE],
		m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME]))
	{
		m_pcdr[ATTRI_ERROR_CODE] = ERROR_INVALID_SERVSP;
	}
	
	return 0;
}



//增值业务话单是否符合规则
bool LocalizeSpBillRuleProc::SpCdrValid(const T_ATTRI_VAL &r_source,
										const T_ATTRI_VAL &r_spcode,
										const T_ATTRI_VAL &r_sp_srvtype,
										const T_DATE_TIME &r_begintime)
{
    PO_SpBillRule po;
    vector<PO_SpBillRule*> rpo;
    vector<PO_SpBillRule*>::iterator itr;
	
	po.recordStruct.m_source=r_source;
	po.recordStruct.m_spcode=r_spcode;
    if(m_PO_PO_SpBillRuleContainer->searchMulti(po,rpo)<=0)
		return true;
	
	for(itr = rpo.begin();itr != rpo.end();itr++)
    {
		if (((*itr)->recordStruct.beginTime <= r_begintime) && ((*itr)->recordStruct.endTime >= r_begintime) &&
			((*itr)->recordStruct.m_sp_srvtype == r_sp_srvtype || (*itr)->recordStruct.m_sp_srvtype == "*"))
		{
			if((*itr)->recordStruct.m_billflag == "0")
			{
				return false;
			}
			else
			{
				return true;
			}
		}
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeSpOperFeeProc);

int LocalizeSpOperFeeProc::execute()
{
	return 1;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(BeforeGetFixHomereaCodeA);

//0继续处理 1正常返回
int BeforeGetFixHomereaCodeA::execute()
{
	//800话单归属地不根据号码来重求
	if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_FIXGSM_800)
	{
		m_pcdr[ATTRI_HOME_CODE_A] = m_pcdr[ATTRI_VISIT_CODE_A];
		if(strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(), "800", 3) == 0)
		{
			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SERVICE_TYPE_800;
		}
		else if(strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(), "4006", 4) == 0)
		{
			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SERVICE_TYPE_4006;
		}
		else if(strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(), "0", 1) != 0)
		{
			m_pcdr[ATTRI_MSISDN_A] = m_pcdr[ATTRI_VISIT_CODE_A] + m_pcdr[ATTRI_MSISDN_A];
			return 0;
		}
		else
		{
			return 0;
		}
		m_pcdr[ATTRI_USER_ASP_A] = COMPANY_TYPE_CNC;
		return 1;
	}
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(GetFixHomereaCodeAByTrunkReason);

//0继续处理 1正常返回 2错单
int GetFixHomereaCodeAByTrunkReason::execute()
{
	 T_ATTRI_VAL long_group;
   T_ATTRI_VAL calledType;
	 int rd;
   if(m_pcdr[ATTRI_TRUNK_FLAG] == TRUNK_FLAG_YES)
   {
     //中继计费也尝试求营业区
	  rd = m_globalManager->getHomeAreaCodeByFixPhone(m_pcdr[ATTRI_MSISDN_A],
               m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
               m_pcdr[ATTRI_EXCHANGECODE_A],m_pcdr[ATTRI_SUB_HOMECODE_A],m_pcdr[ATTRI_HOME_CODE_A],long_group,
               m_pcdr[ATTRI_USER_TYPE_A],m_pcdr[ATTRI_OFFICE_CODE],
               calledType);
	  if (rd<=0)
	  {
		  m_pcdr[ATTRI_USER_TYPE_A] = USER_TYPE_B_DEFAULT_FIX;
	  }
	  m_pcdr[ATTRI_USER_ASP_A] = m_pcdr[ATTRI_USER_TYPE_A][0];
	  m_pcdr[ATTRI_USER_HOME_TYPE_A] = m_pcdr[ATTRI_USER_TYPE_A][3];
    //归属地如果为空 重置
    if(m_pcdr[ATTRI_HOME_CODE_A].empty())
	    m_pcdr[ATTRI_HOME_CODE_A]      = m_pcdr[ATTRI_VISIT_CODE_A];
	  return 1;
    }
    else
    {
      m_pcdr[ATTRI_ERROR_CODE] = AH_FIX_MSISDN_ERROR;
      return 1;
    }

	return 0;
}


///////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(BeforeFixOtherPartyStand);

int BeforeFixOtherPartyStand::execute()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
INSTALL_PROCESSBASE_INSTANCE(LocalizeFixAttribSet);

int LocalizeFixAttribSet::execute()
{
	if (m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FIXVOICE)
	{
		//固网语音国际长途分组组号根据国际长途区号表(TD_INTER_FEE_CODE)中的GROUP_ID_EX1、GROUP_ID_EX2来决定，不与G网分组冲突
		if(m_pcdr[ATTRI_LONG_TYPE1] == LONG_TYPE_INTERNATIONAL)
		{
			//固网直播分组 GROUP_ID_EX1
			if(m_pcdr[ATTRI_CARRIER_TYPE] == CARRIER_TYPE_IP_DEFAULT)
			{
				m_pcdr[ATTRI_LONG_GROUP1] = m_pcdr[ATTRI_LONG_GROUP_EX1];
			}
			//固网IP分组 GROUP_ID_EX2
			else
			{
				m_pcdr[ATTRI_LONG_GROUP1] = m_pcdr[ATTRI_LONG_GROUP_EX2];
			}
		}

		//如果800业务为长途，费用挪到长途费上
		if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_FIXGSM_800 && m_pcdr[ATTRI_LONG_TYPE1] > LONG_TYPE_NONE)
		{
			m_pcdr[ATTRI_ORGFEE_FIELD3] = m_pcdr[ATTRI_ORGFEE_FIELD1];
			m_pcdr[ATTRI_ORGFEE_FIELD1] = 0;
		}

   //虚拟网 在所属区号内相互通话均判为本地通话
    if(m_globalManager->isVirtualArea(m_pcdr[ATTRI_HOME_CODE_A],m_pcdr[ATTRI_HOME_CODE_B],
          m_pcdr[ATTRI_USER_HOME_TYPE_A],m_pcdr[ATTRI_USER_ASP_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME]))
    {
  	  m_pcdr[ATTRI_LONG_TYPE1] = LONG_TYPE_NONE;
  	  m_pcdr[ATTRI_LONG_GROUP1] = DEFAULT_GROUP_ID_B;
    }

		if(m_pcdr[ATTRI_TRUNK_FLAG] == TRUNK_FLAG_YES)
		{
			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_IP_SERV_TYPE_A];
			return 0;
		}
		
		if(m_pcdr[ATTRI_STATIS_STR3] == BILL_FLAG_3)
		{
			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_MISC_STR3];
			if(m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == "13")
			{
				return 0;
			}
		}
		if(m_pcdr[ATTRI_STATIS_STR2] == BILL_FLAG_2)
		{
			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_MISC_STR2];
			if(m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == "12")
			{
				return 0;
			}
		}
		if(m_pcdr[ATTRI_STATIS_STR1] == BILL_FLAG_1)
		{
			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_MISC_STR1];
			if(m_pcdr[ATTRI_USER_SERVICE_TYPE_A] == "11")
			{
				return 0;
			}
		}
	}
	
	return 0;
}

//add by dph 过滤号码处理
bool LocalizeFixAttribSet::FilterFixCalledPhone(const T_ATTRI_VAL &r_area_code,
												const T_ATTRI_VAL &r_msc,
												const T_ATTRI_VAL &r_phone_a,
												const T_ATTRI_VAL &r_area_code_b,
												const T_ATTRI_VAL &r_phone_b,
												const T_DATE_TIME &r_begintime,
												T_ATTRI_VAL &r_serv_type)
{
	vector<PO_ExchangeDiscount>::iterator itr;
	vector<PO_ExchangeDiscount> &t_Vector = m_globalManager->m_PO_ExchangeDiscountContainer.getParamVector();
	
	T_ATTRI_VAL t_phone_b;
	T_ATTRI_VAL t_phone_b1;
	T_ATTRI_VAL t_area_code;
	T_ATTRI_VAL t_area_code_b;
	T_ASP t_asp;
	T_ATTRI_VAL t_homecode;
    T_ATTRI_VAL t_longgroup;
    T_ATTRI_VAL t_usertype;
    bool isareaCode = true;
	
    t_area_code = r_area_code;
    t_phone_b   = r_phone_b;
    t_phone_b1  = r_phone_b;
	t_area_code_b = r_area_code_b;

	if(strncmp(r_phone_b.c_str(), "00", 2)==0)
    {
		isareaCode = false;
    }
	if(m_globalManager->getAspByMsisdnHeader(r_phone_b,r_begintime,t_asp))
    {
		//如果被叫是不带0的手机号码，需要加区号匹配
		t_phone_b = t_area_code_b + t_phone_b;
		isareaCode = false;
    }//end of modify by dph
	else if(strncmp(r_phone_b.c_str(), "0", 1)!=0 && r_phone_b.length()>=10)
    {
		t_phone_b1.append(0,"0");
		if(m_globalManager->getHomeAreaCodeByMsisdnN(t_phone_b1,r_begintime,t_homecode,t_longgroup,t_usertype)>0)
		{
			//modify by dph，不用判断是否是省内区号
			if(m_globalManager->isProvInnerCode(t_homecode))
			{
				t_phone_b = t_phone_b1;
				isareaCode = false;
			}
			//t_phone_b = t_phone_b1;
			//isareaCode = false;
		}
    }
    else if(strncmp(r_phone_b.c_str(), "0", 1)==0 && r_phone_b.length()>=10)
    {
		if(m_globalManager->getHomeAreaCodeByMsisdnN(r_phone_b,r_begintime,t_homecode,t_longgroup,t_usertype)>0)
		{
			//modify by dph，不用判断是否是省内区号
			/*if(isProvInnerCode(t_homecode))
			{
				isareaCode = false;
			}*/
			isareaCode = false;
		}
    }
    
    if(isareaCode)
    {
		t_phone_b = t_area_code + t_phone_b;
    }
	
	for (itr=t_Vector.begin(); itr!=t_Vector.end(); itr++)
	{
		//支持区号+*匹配方式
		if(strncmp(itr->recordStruct.m_beginPhone_a.c_str(), r_area_code.c_str(), r_area_code.length()) != 0)
			continue;
		if( (itr->recordStruct.m_msc == "*" || itr->recordStruct.m_msc == r_msc) &&
			((strncmp(itr->recordStruct.m_beginPhone_a.c_str(), r_area_code.c_str(), r_area_code.length()) == 0 &&
			strncmp(itr->recordStruct.m_beginPhone_a.c_str()+r_area_code.length(), "*", 1) == 0) ||
			((strncmp(r_phone_a.c_str(), itr->recordStruct.m_beginPhone_a.c_str(), itr->recordStruct.m_beginPhone_a.length()) >= 0) &&
			(strncmp(r_phone_a.c_str(), itr->recordStruct.m_endPhone_a.c_str(), itr->recordStruct.m_endPhone_a.length()) <= 0))) &&
			(itr->recordStruct.m_endPhone_b == "**" ||
			(strncmp(itr->recordStruct.m_beginPhone_b.c_str(), r_area_code_b.c_str(), r_area_code_b.length()) == 0 && 
			strncmp(itr->recordStruct.m_beginPhone_b.c_str()+r_area_code_b.length(), "*", 1) == 0) ||
			((strncmp(t_phone_b.c_str(), itr->recordStruct.m_beginPhone_b.c_str(), itr->recordStruct.m_beginPhone_b.length()) >= 0) &&
			(strncmp(t_phone_b.c_str(), itr->recordStruct.m_endPhone_b.c_str(), itr->recordStruct.m_endPhone_b.length()) <= 0))) &&
			(itr->recordStruct.m_beginTime <= r_begintime) &&
			(itr->recordStruct.m_endTime >= r_begintime)
			)
		{
			r_serv_type = itr->recordStruct.m_ServType;
			return true;
		}
	}
	
	return false;
}

bool LocalizeFixAttribSet::trashprefixCalled()
{
    T_ATTRI_VAL r_homecode;
    T_ATTRI_VAL r_longgroup;
    T_ATTRI_VAL r_usertype;
    T_ATTRI_VAL r_msisdn_b;
    T_ATTRI_VAL t_msisdn_b;
    T_ATTRI_VAL r_dealed_b;
	
    bool isareaCode = false; //默认无区号
    r_msisdn_b = m_pcdr[ATTRI_MSISDN_B_ORG];
    t_msisdn_b = m_pcdr[ATTRI_MSISDN_B_ORG];
	
    /*if(isMobile(r_msisdn_b) || strncmp(r_msisdn_b.c_str(), "00", 2)==0)
	return false;*/
	//modify by dph，手机号码也要过滤掉
	if(strncmp(r_msisdn_b.c_str(), "00", 2)==0)
		return false;
	if(isMobile(r_msisdn_b))
	{
		//手机号码不加区号
	}
	//else if(strncmp(r_msisdn_b.c_str(), "0", 1)!=0 && r_msisdn_b.length()>=9)
	else if(strncmp(r_msisdn_b.c_str(), "0", 1)!=0 && r_msisdn_b.length()>=9)
	{
		t_msisdn_b.append(0,"0");
		if(m_globalManager->getHomeAreaCodeByMsisdnN(t_msisdn_b,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			r_homecode,r_longgroup,r_usertype)>0)
		{
			//modify by dph,不用判断是否省内区号
			if(m_globalManager->isProvInnerCode(r_homecode))
			{
				r_msisdn_b = t_msisdn_b.c_str() + r_homecode.length();
				isareaCode = true;
			}
			/*r_msisdn_b = t_msisdn_b.c_str() + r_homecode.length();
			isareaCode = true;*/
		}
	}
	//else if(strncmp(r_msisdn_b.c_str(), "0", 1)==0 && r_msisdn_b.length()>=10)
	else if(strncmp(r_msisdn_b.c_str(), "0", 1)==0)
	{
		if(m_globalManager->getHomeAreaCodeByMsisdnN(r_msisdn_b,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			r_homecode,r_longgroup,r_usertype)>0)
		{
			//modify by dph,不用判断是否省内区号
			/*if(m_globalManager->isProvInnerCode(r_homecode))
			{
				r_msisdn_b = r_msisdn_b.c_str() + r_homecode.length();
				isareaCode = true;
			}*/
			r_msisdn_b = r_msisdn_b.c_str() + r_homecode.length();
			isareaCode = true;
		}
	}
	
	//拨打特定前缀话单无效处理
	if(RegularFixCalledPhone(m_pcdr[ATTRI_HOME_CODE_B],r_msisdn_b,m_pcdr[ATTRI_MSC],m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],r_dealed_b))
	{
		return true;
	}
	return false;
}

bool LocalizeFixAttribSet::RegularFixCalledPhone(const T_ATTRI_VAL &r_calledareaCode,
												 const T_ATTRI_VAL &r_calledPhone,
												 const T_ATTRI_VAL &r_msc,
												 const T_DATE_TIME &r_beginTime,
												 T_ATTRI_VAL &r_dealedPhone)
{
	vector<PO_RegularFixCalled>::iterator itr;
	vector<PO_RegularFixCalled> &t_Vector = m_PO_RegularFixCalledContainer->getParamVector();
	
	for (itr=t_Vector.begin();itr!=t_Vector.end();itr++)
	{
		if((itr->recordStruct.m_calledareacode == "*" || itr->recordStruct.m_calledareacode == r_calledareaCode) &&
			strncmp(r_calledPhone.c_str(),itr->recordStruct.m_calledprefix.c_str(),itr->recordStruct.m_calledprefix.length())==0 &&
			(itr->recordStruct.m_msc == "*" || itr->recordStruct.m_msc == r_msc) &&
			itr->recordStruct.beginTime <= r_beginTime &&
			itr->recordStruct.endTime >= r_beginTime)
		{
			r_dealedPhone = r_calledPhone.c_str() + itr->recordStruct.m_calledprefix.length();
			return true;
		}
	}
	return false;
}

bool LocalizeFixAttribSet::trashfixPhonePtp(const T_ATTRI_VAL &r_msc,
											const T_ATTRI_VAL &r_callingPhone,
											const T_ATTRI_VAL &r_calledPhone,
											const T_DATE_TIME &r_beginTime)
{
    PO_NmFixTrashPtp po;
    vector<PO_NmFixTrashPtp*> rpo;
    vector<PO_NmFixTrashPtp*>::iterator itr;
	//add by dph
	vector<PO_NmFixTrashPtp*>::iterator itr1;
	//delete by dph
    //PO_NmFixTrashPtp *rpo1;
	
    int callingGroupId = 0;
    int calledGroupId = -1;
	
    po.recordStruct.m_msc = r_msc;
    po.recordStruct.m_phone = r_callingPhone;
	
    if(m_PO_NmFixTrashPtpContainer->searchMulti(po,rpo)<=0)
		return false;
	
    for(itr = rpo.begin(); itr != rpo.end(); itr++)
    {
		if(((*itr)->recordStruct.m_msc == "*" || (*itr)->recordStruct.m_msc == r_msc) &&
			(*itr)->recordStruct.beginTime <= r_beginTime &&
			(*itr)->recordStruct.endTime >= r_beginTime)
		{
			callingGroupId = (*itr)->recordStruct.m_groupId;
			po.recordStruct.m_phone = r_calledPhone;
			/*if(m_PO_NmFixTrashPtpContainer->search(po, rpo1)>0)
			{
				calledGroupId = (*rpo1).recordStruct.m_groupId;
				if(callingGroupId == calledGroupId)
					return true;
				else
					return false;
			}
			return false;*/
			
			//add by dph,被叫号码也要searchMulti
			if(m_PO_NmFixTrashPtpContainer->searchMulti(po, rpo)<=0)
				return false;

			for(itr1 = rpo.begin(); itr1 != rpo.end(); itr1++)
			{
				if(((*itr1)->recordStruct.m_msc == "*" || (*itr1)->recordStruct.m_msc == r_msc) &&
					(*itr1)->recordStruct.beginTime <= r_beginTime &&
					(*itr1)->recordStruct.endTime >= r_beginTime)
				{
					calledGrId = (*itr)->recordStruct.m_groupId;
			po.recordStruct.m_phone = r_calledPhone;
			/*if(m_PO_NmFixTrashPtpContainer->search(po, rpo1)>0)
			{
				calledGroupId = (*rpo1).recordStruct.m_groupId;
				if(callingGroupId == calledGroupId)
					return true;
				else
					return false;
			}
			return false;*/
			
			//add by dph,琚