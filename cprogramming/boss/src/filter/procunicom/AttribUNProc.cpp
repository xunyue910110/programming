#include "AttribUNProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"
#include "FilterError.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"


INSTALL_PROCESSBASE_INSTANCE(AttribUNProc);

int AttribUNProc::execute()
{
    return 0;
}

//Added by Sunam 2006-6-22 9:00
bool AttribUNProc::initialize()
{
	//unique_flag = false;
	return true;
}

//------------------------------------------------------------------------------
//  得到计费号码的 HOME_AREA_CODE_A, OFFICE_CODE, PROVINCE_CODE_A
//    SET:USER_TYPE=M_USERASPA + M_USERSERVICETYPEA + M_HOMETYPEA
//        OFFICE_CODE,
//        HOME_AREA_CODE_A
//
//        如果是省内用户,OFFICE_CODE 填的是地市代码
//        如果是国内用户,PROVINCE_CODE_A 填的是省代码
//
//  根据msisdn和imsi两种方法求归属
bool AttribUNProc::getHomeAreaCodeA()
{
    T_ATTRI_VAL sTmp;
    T_ATTRI_VAL imsi;
    //Added by Sunam 2005-12-13
    T_ATTRI_VAL calledType;
    bool flag;
    string strTmp;
	
	// TODO 专网计费可能涉及到
	
    //--------------------------------------------------------------------------
    // 1. 根据 IMSI 求归属
    if (m_Config->m_getAHomeMethod == A_HOME_BY_IMSI)
    {
        return (getAHByImsi(m_pcdr[ATTRI_IMSI_NUMBER]));
    }
    //--------------------------------------------------------------------------
    // 2.缺省以号码 MSISDN 求归属
    else
    {
		// 专网与IP直通车
		if ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_POC) || (m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_PTC))
		{
            T_ATTRI_VAL long_group;
            int rd;
			
			/*
			专网用户一定为省内用户，如果计费方为手机号码，则去省内号码段查找。
			如果计费方为固定号码则去国内长途区号中查找。
			*/
			if (isSelfNumber(m_pcdr[ATTRI_MSISDN_A]))
			{
				//Modified by Sunam 2005-12-13
				rd = m_globalManager->getHomeAreaCodeByMsisdnP(m_pcdr[ATTRI_MSISDN_A],
					m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
					m_pcdr[ATTRI_HOME_CODE_A],long_group,
					m_pcdr[ATTRI_USER_TYPE_A],m_pcdr[ATTRI_OFFICE_CODE],
					calledType);
				
				if (rd<=0)
				{
					m_pcdr[ATTRI_ERROR_CODE] = BH_MSISDNP_ERROR;
					return false;
				}
				else
				{
					m_pcdr[ATTRI_USER_TYPE_A        ] = USER_TYPE_A_DEFAULT ;
					m_pcdr[ATTRI_USER_ASP_A         ] = m_pcdr[ATTRI_USER_TYPE_A][0];
					m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_USER_TYPE_A].substr(1,2);
					m_pcdr[ATTRI_USER_HOME_TYPE_A]    = m_pcdr[ATTRI_USER_TYPE_A][3];
					return true;
				}
			}
			else
			{
				rd = m_globalManager->getHomeAreaCodeByMsisdnN(m_pcdr[ATTRI_MSISDN_A],
					m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
					m_pcdr[ATTRI_HOME_CODE_A],long_group,
					m_pcdr[ATTRI_USER_TYPE_A]);
				if (rd<=0)
				{
					m_pcdr[ATTRI_ERROR_CODE] = BH_UT_MSISDNN_ERROR;
					return false;
				}
				else
				{
					m_pcdr[ATTRI_USER_TYPE_A        ] = USER_TYPE_A_DEFAULT ;
					m_pcdr[ATTRI_USER_ASP_A         ] = m_pcdr[ATTRI_USER_TYPE_A][0];
					m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_USER_TYPE_A].substr(1,2);
					m_pcdr[ATTRI_USER_HOME_TYPE_A]    = m_pcdr[ATTRI_USER_TYPE_A][3];
					return true;
				}
			}
		}
		
		//国际IMSI根据IMSI找国际IMSI表
		if ((strncmp(m_pcdr[ATTRI_IMSI_NUMBER].c_str(),"46000",5) != 0) &&
			(strncmp(m_pcdr[ATTRI_IMSI_NUMBER].c_str(),"46001",5) != 0) &&
			(strncmp(m_pcdr[ATTRI_IMSI_NUMBER].c_str(),"46002",5) != 0) &&
			(strncmp(m_pcdr[ATTRI_IMSI_NUMBER].c_str(),"46003",5) != 0))
		{
			//Modified by Sunam 2005-10-25
			//return (getAHByImsi(m_pcdr[ATTRI_IMSI_NUMBER]));
			flag = getAHByImsi(m_pcdr[ATTRI_IMSI_NUMBER]);
			if (flag)
			{
				return true;
			}
			else
			{
				if(isSelfNumber(m_pcdr[ATTRI_MSISDN_A]))
				{
					m_pcdr[ATTRI_ERROR_CODE] = DEFAULT_ERROR;
					return (getAHByMsisdn(m_pcdr[ATTRI_MSISDN_A]));
				}
				else
				{
					return false;
				}
			}
		}
		
		//判断手机号是否为13开头 (013的号码要在前面标准化处理成13)
		//if (!(strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(),"13",2)))
		if(isSelfNumber(m_pcdr[ATTRI_MSISDN_A]))
		{
			return (getAHByMsisdn(m_pcdr[ATTRI_MSISDN_A]));
		}
		else if(isOtherNumber(m_pcdr[ATTRI_MSISDN_A]))
		{
			return (getAHByMsisdn(m_pcdr[ATTRI_MSISDN_A]));
		}
		
		//手机号既不是13开头IMSI却是460(461)开头,有错
		m_pcdr[ATTRI_ERROR_CODE] = AH_UT_MSISDNN_ERROR;
		return false;
	}
}

// 得到计费方号码的到访地
//  SET:  VISIT_CODE_A
bool AttribUNProc::getVisitAreaCodeA()
{
	// 1.无条件呼转
	//   GSM无条件呼转话单是在发起呼叫的A最近的交换机下话单,
	//   无法判断呼转B的到访地 将M_VISIT_CODE_A=HOME_CODE_A
	if (isUnConditionalForw())
	{
		//国际来访无条件呼转根据交换机代码求到访地
		//国际来访用户根据交换机求到访
		//C网所有呼转都当做无条件呼转,漫游地为归属地
		//G网按原始呼转原因进行判断,漫游地根据交换机
		if (m_pcdr[ATTRI_USER_HOME_TYPE_A] >= AREA_TYPE_NATIONAL)
		{
			//Modified by Sunam 2006-3-27 16:13
			//if (!getAVByMsc())
			if (!getAVByMsc(ALL_MSC))
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		/*
		//add by yangyi  漫游来访按总部到访地要求填交换机的所在地
		else if(m_pcdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_NATIONAL)
		{
		int rd;
		//2007-2-2 17:59 modiby by yangb
		//ATTRI_VISIT_CODE_A_ROAM 暂取消
		rd = m_globalManager->getAVByMscLac(m_pcdr[ATTRI_MSC],
		m_pcdr[ATTRI_LAC_A],m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
		m_pcdr[ATTRI_VISIT_CODE_A]);
        if (rd <= 0)
        {
		m_pcdr[ATTRI_ERROR_CODE] = AV_MSC_CELL_ERROR;
        }
		}
		*/
		//省内用户将其到访置为归属
		else if(m_pcdr[ATTRI_USER_HOME_TYPE_A] < AREA_TYPE_NATIONAL)
		{
			m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
			return true;
		}
	}
	
	//本地化漫游处理
	if (ProcessBaseHelper::ExecuteProcedure("BeforeGetVisitAreaCodeA")==1)
		return true;
	
	// 2.针对其它话单特殊处理
	if ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_POC) || (m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_PTC))
	{
		m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
		return true;
	}
	
	//add by shenglj 2009-04-27 :13:15:01
	/*
	// 3.国际来访收发短信话单处理,没有LAC信息
	if (m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMMO || m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMMT )
	{
	//Modified by Sunam 2006-3-27 16:14
	//if (!getAVByMsc())
	if (!getAVByMsc(ALL_MSC))
	{
	return false;
	}
	else
	{
	return true;
	}
	}
	*/
	
	// 4.正常话单根据运营商指定的方法求取
    switch(m_Config->m_getAVisitMethod)
    {
        //根据MSC,LAC
	case A_VISIT_BY_MSC_LAC:
		if (!getAVByMscLac())
			return false;
		break;
        //根据MSC,CELLID
	case A_VISIT_BY_MSC_CELLID:
		if (!getAVByMscCellId())
			return false;
		break;
        //根据MSC,LAC,CELLID
	case A_VISIT_BY_MSC_LAC_CELLID:
		if (!getAVByMscLacCellId())
			return false;
		break;
        //根据CELL_ID
	case A_VISIT_BY_CELLID:
		if (!getAVByCellId())
			return false;
		break;
        //根据LAC
	case A_VISIT_BY_LAC:
		if (!getAVByLac())
			return false;
		break;
        //根据 LAC,CELL_ID
	case A_VISIT_BY_LAC_CELLID:
		if  (!getAVByLacCellId())
			return false;
		break;
        //根据交换机msc
	case A_VISIT_BY_MSC:
		//Modified by Sunam 2006-3-27 16:09
		//if (!getAVByMsc())
		if (!getAVByMsc(ALL_MSC))
			return false;
		break;
	default:
		if (!getAVByMscLac())
			return false;
		break;
	}
	return true;
}

//判断是否是无条件呼转话单
bool AttribUNProc::isUnConditionalForw()
{
	//C网所有呼转都当做无条件呼转，漫游地为归属地
	//G网按原始呼转原因进行判断，漫游地根据交换机
	//根据用户A的业务类型来判断是否是智能网用户
	if(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW)
	{
		if ((m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNCONDITIONAL) ||
			(m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNCONDITIONAL2)||
			(m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_UNREACHABLE))
		{
			return true;
		}
	}
	return false;
}

// 得到对方号码的归属地
//  set:  HomeAreaCodeB,userTypeB,
//        userAspB,userServiceTypeB,
//        homeTypeB,long_group
//        provinceCodeB(有时置上,有时没置,暂时无用)
bool AttribUNProc::getHomeAreaCodeB()
{
    T_ATTRI_VAL officeCode;
    T_ATTRI_VAL imsi;
    T_ATTRI_VAL msisdn;
    T_ATTRI_VAL m123;
    T_ATTRI_VAL longGroup;
    int     rd;
	
    //##################################################################
    // 1. B 为手机号码
    //if (strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"13",2)==0)
    if (isMobile(m_pcdr[ATTRI_MSISDN_B]))
    {
        if (m_pcdr[ATTRI_MSISDN_B].length() > 11)
        {
            msisdn=m_pcdr[ATTRI_MSISDN_B].substr(0,11);
            m_pcdr[ATTRI_MSISDN_B]=msisdn;
        }

		if(m_pcdr[ATTRI_BIZ_TYPE] == BIZ_TYPE_FIXVOICE 
			|| m_pcdr[ATTRI_BIZ_TYPE] == "12"
			|| m_pcdr[ATTRI_BIZ_TYPE] == "15")
		{
			if(m_pcdr[ATTRI_MSISDN_B].substr(0,11) == "13800138000")
			{
				m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_HOME_CODE_A];
				m_pcdr[ATTRI_LONG_GROUP1] = DEFAULT_GROUP_ID_B;//"20" 省内
				m_pcdr[ATTRI_USER_TYPE_B] = "";
				m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = "01";
				m_pcdr[ATTRI_USER_HOME_TYPE_B] = AREA_TYPE_LOCAL;
				return true;
			}
		}
		
		//Added by Sunam 2005-12-29 先查找td_special_msisdn表，如果能查到B归属则返回，否则继续
		if (m_globalManager->getHomeAreaCodeBySpecialMsisdn(m_pcdr[ATTRI_MSISDN_B],
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			m_pcdr[ATTRI_HOME_CODE_B], m_pcdr[ATTRI_LONG_GROUP1],
			m_pcdr[ATTRI_USER_TYPE_B], m_pcdr[ATTRI_PROVINCE_CODE_B],
			m_pcdr[ATTRI_CALLED_TYPE_B]))
		{
			setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
			m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
			m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
			return true;
		}
		
		/*
        // 1.1. 语音信箱号码
        if (isVoiceBox(m_pcdr[ATTRI_MSISDN_B]))
        {
		//user_type,long_group 应当已置好
		if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
		{
		m_pcdr[ATTRI_ERROR_CODE] = BH_UT_M123_ERROR;
		return false;
		}
		setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
		m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
		m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
		return true;
        }
		
		 // 处理语音信箱号码出错
		 if(m_pcdr[ATTRI_ERROR_CODE] == GET_CITY_CODE_ERROR )
		 {
		 return false;
		 }
		*/
        // 1.2. 本运行商号码
        if (isSelfNumber(m_pcdr[ATTRI_MSISDN_B]))
        {
            // 1.2.1 省内号码段查找
            rd = m_globalManager->getHomeAreaCodeByMsisdnP(m_pcdr[ATTRI_MSISDN_B],
				m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
				m_pcdr[ATTRI_HOME_CODE_B],
				m_pcdr[ATTRI_LONG_GROUP1],
				m_pcdr[ATTRI_USER_TYPE_B],
				officeCode,m_pcdr[ATTRI_CALLED_TYPE_B]);
			
            if (rd<=0)
            {
                // 根据IMSI取对方归属
                if (m_Config->m_getBHomeMethod == B_HOME_BY_MSISDN_IMSI)
                {
                    // 对方号码转化IMSI查找TD_IMSIN
                    //msisdnToImsi(m_pcdr[ATTRI_MSISDN_B].substr(2,m_pcdr[ATTRI_MSISDN_B].length()),imsi);
                    msisdnToImsi(m_pcdr[ATTRI_MSISDN_B],imsi);
					
                    rd = m_globalManager->getHomeAreaCodeByImsiN(imsi,
						m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
						m_pcdr[ATTRI_HOME_CODE_B],
						m_pcdr[ATTRI_LONG_GROUP1],
						m_pcdr[ATTRI_USER_TYPE_B],
						m_pcdr[ATTRI_PROVINCE_CODE_B]);
                    if (rd <= 0)
                    {
                        m_pcdr[ATTRI_ERROR_CODE] = BH_IMSIN_ERROR ;
                        return false;
                    }
                    else
                    {
                        if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
                        {
                            m_pcdr[ATTRI_ERROR_CODE] = BH_UT_M123_ERROR;
                            return false;
                        }
						
						setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
						m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
						m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
                        return true;
                    }
                }
                // 根据 MSISDN 取对方归属
                else
                {
                    // 国内号码段找
                    rd = m_globalManager->getHomeAreaCodeByPhoneN(m_pcdr[ATTRI_MSISDN_B],
						m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
						m_pcdr[ATTRI_HOME_CODE_B],
						m_pcdr[ATTRI_LONG_GROUP1],
						m_pcdr[ATTRI_USER_TYPE_B],
						m_pcdr[ATTRI_PROVINCE_CODE_B],
						m_pcdr[ATTRI_CALLED_TYPE_B]);
					
                    if (rd<=0)
                    {
                        // 智能网号码段找
                        rd = m_globalManager->getHomeAreaCodeBySSP(m_pcdr[ATTRI_MSISDN_B],
							m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
							m_pcdr[ATTRI_HOME_CODE_B],
							m_pcdr[ATTRI_LONG_GROUP1],
							m_pcdr[ATTRI_USER_TYPE_B],
							m_pcdr[ATTRI_PROVINCE_CODE_B],
							m_pcdr[ATTRI_CALLED_TYPE_B]);
                        if (rd<=0)
                        {
							
                            if ( (m_pcdr[ATTRI_MSISDN_B].length() < 11)
								&&(m_pcdr[ATTRI_CALL_TYPE]  == CALL_TYPE_MTC)
								&&((m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_NATIONAL_IN) ||
								(m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_IN)
								)
								)
                            {
                                // 前面 +86 匹配全国交换机定义表
                                m123 = m_pcdr[ATTRI_MSISDN_B];
                                m123.append(0,"86");
								
								
                                rd = m_globalManager->getHomeAreaCodeByM123(m123,
									m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
									m_pcdr[ATTRI_HOME_CODE_B],
									m_pcdr[ATTRI_LONG_GROUP1],
									m_pcdr[ATTRI_USER_TYPE_B]);
								
                                if (rd <= 0)
                                {
                                    m_pcdr[ATTRI_ERROR_CODE] = BH_MSISDNN_ERROR;
                                    return false;
                                }
                                else
                                {
                                    if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
                                    {
                                        m_pcdr[ATTRI_ERROR_CODE] = BH_UT_M123_ERROR;
                                        return false;
                                    }
									
									setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
									m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
									m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
                                    return true;
                                }
                            }
                            else if (m_pcdr[ATTRI_MSISDN_B].length() == 11)
                            {
                                m_pcdr[ATTRI_ERROR_CODE] = BH_MSISDNN_ERROR ;
                                return false;
                            }
                            else
                            {
                                //置缺省
                                m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_VISIT_CODE_A];
								
                                if (m_Config->m_operatorType == OPERATOR_TYPE_UNICOM)
                                    m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_UNICOM;
                                else
                                    m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_CMCC;
								
								setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
								m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
								m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
								
                                m_pcdr[ATTRI_LONG_GROUP1] = DEFAULT_GROUP_ID_B;
                                return true;
								
                            }
							
                        }
                        else
                        {
                            if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
                            {
                                m_pcdr[ATTRI_ERROR_CODE] = BH_UT_M123_ERROR;
                                return false;
                            }
							
							setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
							m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
							m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
                            return true;
							
                        }
						
                    }
                    else
                    {
                        if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
                        {
                            m_pcdr[ATTRI_ERROR_CODE] = BH_UT_M123_ERROR;
                            return false;
                        }
						
						setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
						m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
						m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
                        return true;
                    }
                }
            }
            else
            {
                //USER_TYPE,LONG_GROUP已置好
                if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
                {
                    m_pcdr[ATTRI_ERROR_CODE] = BH_UT_M123_ERROR;
                    return false;
                }
				
				setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
				m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
				m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
                //本省用户置省代码
                // TODO
                m_pcdr[ATTRI_PROVINCE_CODE_B] = m_Config->m_provinceCode;
                return true;
            }
        }
		
        // 1.3 其它运行商移动的号码
        else
        {
            rd = m_globalManager->getHomeAreaCodeByMsisdnO(m_pcdr[ATTRI_MSISDN_B],
				m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
				m_pcdr[ATTRI_HOME_CODE_B],
				m_pcdr[ATTRI_LONG_GROUP1],
				m_pcdr[ATTRI_USER_TYPE_B],
				m_pcdr[ATTRI_CALLED_TYPE_B]);
            if (rd<=0)
            {
                if (m_pcdr[ATTRI_MSISDN_B].length() == 11)
                {
                    m_pcdr[ATTRI_ERROR_CODE] =  BH_MSISDNO_ERROR;
                    return false;
                }
                else
                {
                    //如果对方号码位数不全,补0,查号段表,匹配
                    if (m_pcdr[ATTRI_MSISDN_B].length() < 11
						&& m_pcdr[ATTRI_MSISDN_B].length() >=7)
                    {
						//modify by shenglj 20090224 19:42:00 不补0
                        //msisdn=m_pcdr[ATTRI_MSISDN_B].substr(0,7);
                        //msisdn.append(7,"0000");
                        //m_pcdr[ATTRI_MSISDN_B]=msisdn;
						
                        //Modified by Sunam 2005-12-14
                        rd = m_globalManager->getHomeAreaCodeByMsisdnO(m_pcdr[ATTRI_MSISDN_B],
							m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
							m_pcdr[ATTRI_HOME_CODE_B],
							m_pcdr[ATTRI_LONG_GROUP1],
							m_pcdr[ATTRI_USER_TYPE_B],
							m_pcdr[ATTRI_CALLED_TYPE_B]);
                        if (rd>0)
                        {
                            if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
                            {
                                m_pcdr[ATTRI_ERROR_CODE] = BH_UT_MSISDNO_ERROR;
                                return false;
                            }
							
							setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
							m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
							m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
							
                            return true;
                        }
						
                    }
					
                    //对于来访话单判错单 不上发
                    if  ((m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_NATIONAL_IN)||
						(m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_IN))
                    {
                        m_pcdr[ATTRI_ERROR_CODE] =  BH_MSISDNO_ERROR;
                        return false;
                    }
					
                    //置缺省
                    m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_VISIT_CODE_A];
					
                    if (m_Config->m_operatorType == OPERATOR_TYPE_UNICOM)
                        m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_UNICOM;
                    else
                        m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_CMCC;
					
					setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
					m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
					m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
					
                    m_pcdr[ATTRI_LONG_GROUP1] = DEFAULT_GROUP_ID_B;
                    return true;
					
                }
            }
            else
            {
                if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
                {
                    m_pcdr[ATTRI_ERROR_CODE] = BH_UT_MSISDNO_ERROR;
                    return false;
                }
				
				setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
				m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
				m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
				
                return true;
            }
        }
    }
	
    //##################################################################
    // 2. B 为国际号码 B is international
    if (!(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"00",2))
		&& (strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"000",3)))
    {
        //查找国际区号
        rd = m_globalManager->getHomeAreaCodeByMsisdnI(m_pcdr[ATTRI_MSISDN_B],
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			m_pcdr[ATTRI_HOME_CODE_B],
			m_pcdr[ATTRI_LONG_GROUP1],
			longGroup,
			m_pcdr[ATTRI_LONG_GROUP_EX1],
			m_pcdr[ATTRI_LONG_GROUP_EX2],
			m_pcdr[ATTRI_GPRSGROUPID],
			m_pcdr[ATTRI_USER_TYPE_B],
			m_pcdr[ATTRI_NATIONAL_CODE_B]);
		
        if (rd <= 0)
        {
            if(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MTC)
            {
                //置缺省
                m_pcdr[ATTRI_HOME_CODE_B]         = m_pcdr[ATTRI_VISIT_CODE_A];
                m_pcdr[ATTRI_USER_TYPE_B]         = USER_TYPE_B_DEFAULT_FIX;
				
				setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
				m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
				m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
				
                m_pcdr[ATTRI_LONG_GROUP1]          = DEFAULT_GROUP_ID_B;
                return true;
            }
            else
            {
                m_pcdr[ATTRI_ERROR_CODE] =  BH_MSISDNI_ERROR;
                return false;
            }
        }
        else
        {
            if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
            {
                m_pcdr[ATTRI_ERROR_CODE] = BH_UT_MSISDNI_ERROR;
                return false;
            }
			
            //add by shenglj
			//IP 电话的长途分组与普通的长途分组不同,使用 td_inter_fee_code 中的 group_id_ex
			if (m_pcdr[ATTRI_CARRIER_TYPE] == CARRIER_TYPE_IP_17911)
			{
				m_pcdr[ATTRI_LONG_GROUP1] = longGroup;
			}
			
			setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
            m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
            m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
            return true;
        }
    }
	
    //#############################################################
    // 3. B 国内或省内号码
    if (!(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"0",1))
		&&(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"00",2)))
    {
        //Added By Sunam 2007-6-6 11:35 068300特殊处理为本地号码
		if (strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"068300",6) == 0)
        {
            //置缺省
            m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_VISIT_CODE_A];
            m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_FIX;
            setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
            m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
            m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
            m_pcdr[ATTRI_LONG_GROUP1] = DEFAULT_GROUP_ID_B;
            return true;
        }
		
        //从国内区号表找
        rd = m_globalManager->getHomeAreaCodeByMsisdnN(m_pcdr[ATTRI_MSISDN_B],
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			m_pcdr[ATTRI_HOME_CODE_B],
			m_pcdr[ATTRI_LONG_GROUP1],
			m_pcdr[ATTRI_USER_TYPE_B]);
        if (rd <= 0)
        {
            if(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MTC)
            {
                //置缺省
                m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_VISIT_CODE_A];
				
                m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_FIX;
				
				setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
				m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
				m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
				
                m_pcdr[ATTRI_LONG_GROUP1] = DEFAULT_GROUP_ID_B;
                return true;
            }
            else
            {
                m_pcdr[ATTRI_ERROR_CODE] = BH_MSISDNN_ERROR;
                return false;
            }
        }
        else
        {
            //处理特殊局向
            m_globalManager->specialBureau(m_pcdr[ATTRI_MSISDN_B],
				m_pcdr[ATTRI_HOME_CODE_B]);
			
            if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
            {
                m_pcdr[ATTRI_ERROR_CODE] = BH_UT_MSISDNN_ERROR;
                return false;
            }
			
			setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
			m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
			m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
            return true;
        }
    }
	
    //####################################################################
    // 4. 对方号码没有0打头的
    // 4.1 MTC FORW ROAM 话单的处理
    //呼转和主叫一同处理 modif by yangyi unicom
    //if(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MTC || m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW)
    if(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MTC)
    {
        //这些特殊号码的解析判断目前可通过条件限制住
        // 特殊处理对方号码是 17908 等号码认为是本地
		//Modified By Sunam 2007-10-18 15:44 国际出访被叫话单对端号码不加0匹配
		if ((m_pcdr[ATTRI_MSISDN_B].length() > 8) &&
			(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(),"179",3)!=0) &&
			(strncmp(m_pcdr[ATTRI_MSISDN_B_ORG].c_str(),"068",3)!=0) &&
			m_pcdr[ATTRI_SPE_PHONE_FLAG] == ATTRI_SPECIALPHONE_N)
        {
            //前面添'0'后再匹配
            msisdn = m_pcdr[ATTRI_MSISDN_B];
            msisdn.append(0,"0");
			
            rd = m_globalManager->getHomeAreaCodeByMsisdnN(msisdn,
				m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
				m_pcdr[ATTRI_HOME_CODE_B],
				m_pcdr[ATTRI_LONG_GROUP1],
				m_pcdr[ATTRI_USER_TYPE_B]);
            if (rd <= 0)
            {
				//Modified by Sunam 2006-5-15 9:37
				m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_VISIT_CODE_A];
				msisdn = m_pcdr[ATTRI_MSISDN_B];
				msisdn.append(0,m_pcdr[ATTRI_HOME_CODE_B].c_str());
				m_pcdr[ATTRI_LONG_GROUP1]  = DEFAULT_GROUP_ID_B;
				m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_FIX;
				
				setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
				m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
				m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
				return true;
            }
            else
            {
				//如果加零能匹配到长途区号，则在对方号码前加
				m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = msisdn;
            }
			
        }
        // LIKE :XXXX no include Fee code
        else
        {
            m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_VISIT_CODE_A];
			//边漫拨打特服号码，BH=AH by shenglj 2009-01-13 21:58:01
			if(m_Config->m_provinceCode == "034")
			{
				if(m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_PORVINNER || m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_INTERPROV)
				{
					m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_HOME_CODE_A];
				}
			}
            m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_FIX;
            m_pcdr[ATTRI_LONG_GROUP1]  = DEFAULT_GROUP_ID_B;
            msisdn = m_pcdr[ATTRI_MSISDN_B];
            msisdn.append(0,m_pcdr[ATTRI_HOME_CODE_B].c_str());
        }
		
        // 处理特殊局向
        m_globalManager->specialBureau(msisdn, m_pcdr[ATTRI_HOME_CODE_B]);
        if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
        {
            m_pcdr[ATTRI_ERROR_CODE] = BH_UT_MSISDNN_ERROR;
            return false;
        }
		
        //将匹配的号码置上other_party  如果不置上,特殊号码匹配会有问题
        //如：1001XXXXX  添0成010 但特殊号码查找找到1001 2002/06/20
        // 被叫话单中的对方号码加不加区号主要是为了特殊号码能正确匹配!
        // 如对方号码为12590,12581等,则不应该加区号(因为特殊号码表配的是没有前缀)
        
        //modify by shenglj 20090224 19:03:00 不加0匹配
        //if (strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(),"125",3)!=0)
        //    m_pcdr[ATTRI_MSISDN_B] = msisdn;
		
        //二次拨号以参数表中ASP为准 shenglj 2009-01-06 20:42:01
        if(m_pcdr[ATTRI_DIAL_TYPE]== "2" || m_pcdr[ATTRI_DIAL_TYPE]== "4" || m_pcdr[ATTRI_DIAL_TYPE]== "6" || m_pcdr[ATTRI_DIAL_TYPE]== "8")
        {
			if(m_pcdr[ATTRI_USER_ASP_B] == COMPANY_TYPE_CMCC)
				m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_CMCC;
			else if(m_pcdr[ATTRI_USER_ASP_B] == COMPANY_TYPE_UNICOM)
				m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_UNICOM;
			else
				m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_FIX;
        }
		setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
		m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
		m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
        return true;
    }
	
    // 4.2 主叫号码、呼转号码
    else
    {
		//Added by Sunam 2006-12-8 11:20 一次拔号接入后非国内、非国际、非手机号码判错 联通去掉判断
		/*
		if (m_pcdr[ATTRI_DIAL_TYPE] == "1" ||
		m_pcdr[ATTRI_DIAL_TYPE] == "3" ||
		m_pcdr[ATTRI_DIAL_TYPE] == "5" ||
		m_pcdr[ATTRI_DIAL_TYPE] == "7")
		{
		m_pcdr[ATTRI_ERROR_CODE] = BH_IP_ERROR;
		return false;
		}
		*/
		
        //主叫话单,认为对方号码为本地
        m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_VISIT_CODE_A];
		//边漫拨打特服号码，BH=AH by shenglj 2009-01-13 21:58:01
		if(m_Config->m_provinceCode == "034")
		{
			if(m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_PORVINNER || m_pcdr[ATTRI_EDGE_FLAG] == EDGE_ROAM_INTERPROV)
			{
				m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_HOME_CODE_A];
			}
		}
        msisdn = m_pcdr[ATTRI_MSISDN_B];
        msisdn.append(0,m_pcdr[ATTRI_HOME_CODE_B].c_str());
        m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_FIX;
        m_globalManager->specialBureau(msisdn,m_pcdr[ATTRI_HOME_CODE_B]);
        if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
        {
            m_pcdr[ATTRI_ERROR_CODE] = BH_UT_MSISDNN_ERROR;
            return false;
        }
		
        //二次拨号以参数表中ASP为准 shenglj 2009-01-06 20:42:01
        if(m_pcdr[ATTRI_DIAL_TYPE]== "2" || m_pcdr[ATTRI_DIAL_TYPE]== "4" || m_pcdr[ATTRI_DIAL_TYPE]== "6" || m_pcdr[ATTRI_DIAL_TYPE]== "8")
        {
			if(m_pcdr[ATTRI_USER_ASP_B] == COMPANY_TYPE_CMCC)
				m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_CMCC;
			else if(m_pcdr[ATTRI_USER_ASP_B] == COMPANY_TYPE_UNICOM)
				m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_UNICOM;
			else
				m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_FIX;
        }
		setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
		m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
		m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
        m_pcdr[ATTRI_LONG_GROUP1]         = DEFAULT_GROUP_ID_B;
        return true;
    }
}

// 得到对方号码的到访地
// SET:  VisitAreaCodeB
bool AttribUNProc::getVisitAreaCodeB()
{
	
	if (PrTH)
        {
            m_pcdr[ATTRI_ERROR_CODE] = BH_UT_MSISDNN_ERROR;
            return false;
        }
		
        //浜屾