#include "AttribCMProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"
#include "FilterError.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"


INSTALL_PROCESSBASE_INSTANCE(AttribCMProc);

int AttribCMProc::execute()
{
    return 0;
}

//Added by Sunam 2006-6-22 9:00
bool AttribCMProc::initialize()
{
	unique_flag = false;
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
bool AttribCMProc::getHomeAreaCodeA()
{
    T_ATTRI_VAL sTmp;
    T_ATTRI_VAL imsi;
    //Added by Sunam 2005-12-13
    T_ATTRI_VAL calledType;
    bool flag;
    string strTmp;

	// TODO 专网计费可能涉及到
	//湖南专网的特殊处理
	if (m_Config->m_provinceCode == "731")
	{
		if (((strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(),"073",3) == 0) ||
			 (strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(),"074",3) == 0)) &&
			(m_pcdr[ATTRI_MSISDN_A].length() == 11))
		{
			strTmp = m_pcdr[ATTRI_MSISDN_A].substr(0,4);
			m_pcdr[ATTRI_HOME_CODE_A        ] = strTmp;
			m_pcdr[ATTRI_USER_ASP_A         ] = COMPANY_TYPE_CT;
			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SERVICE_TYPE_GSM;
			m_pcdr[ATTRI_USER_HOME_TYPE_A   ] = AREA_TYPE_LOCAL;
			return true;
		}
		//***************************************************************
		//处理综合接入专网交换机  2004-12-28  xiel
		//***************************************************************
		//是专网话单 P ,并且不是13的号码，即为PBX用户
		if ((m_pcdr[ATTRI_USER_SERVICE_TYPE_A_YZ] == SERVICE_TYPE_PSTN) &&
			//Modified by Sunam 2005-12-30 10:50 增加159号段
			//(strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(),"13",2) != 0) &&
			(strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(),"0",1) == 0))
		{
			strTmp = m_pcdr[ATTRI_MSISDN_A].substr(0,4);
			m_pcdr[ATTRI_HOME_CODE_A        ] = strTmp;
			m_pcdr[ATTRI_USER_ASP_A         ] = COMPANY_TYPE_CT;
			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SERVICE_TYPE_GSM;
			m_pcdr[ATTRI_USER_HOME_TYPE_A   ] = AREA_TYPE_LOCAL;
			return true;
		}
	}

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

            //天津专网不查A的归属地，直接置为022
            if (m_Config->m_provinceCode == "220")
            {
            	m_pcdr[ATTRI_USER_ASP_A         ] = COMPANY_TYPE_CMCC;
            	m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SERVICE_TYPE_GSM;
            	m_pcdr[ATTRI_USER_HOME_TYPE_A   ] = AREA_TYPE_LOCAL;
            	m_pcdr[ATTRI_HOME_CODE_A        ] = "022";
            	return true;
            }

            //海南NGN不查A的归属地，直接置为0898
            if (m_Config->m_provinceCode == "898")
            {
            	if (m_pcdr[ATTRI_SOURCE_TYPE] == "1O")
            	{
            		m_pcdr[ATTRI_USER_ASP_A         ] = COMPANY_TYPE_CMCC;
            		m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SERVICE_TYPE_GSM;
            		m_pcdr[ATTRI_USER_HOME_TYPE_A   ] = AREA_TYPE_LOCAL;
            		m_pcdr[ATTRI_HOME_CODE_A        ] = "0898";
            		return true;
            	}
            	//Added by Sunam 2007-5-25 10:24
            	//海南PTC话单，不查A的归属地
            	if (m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_PTC)
            	{
            		m_pcdr[ATTRI_USER_ASP_A         ] = COMPANY_TYPE_CMCC;
            		m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SERVICE_TYPE_GSM;
            		m_pcdr[ATTRI_USER_HOME_TYPE_A   ] = AREA_TYPE_LOCAL;
            		m_pcdr[ATTRI_HOME_CODE_A        ] = "0898";
            		return true;
            	}
            }

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
			(strncmp(m_pcdr[ATTRI_IMSI_NUMBER].c_str(),"46002",5) != 0))
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

		//手机号既不是13开头IMSI却是460(461)开头,有错
		m_pcdr[ATTRI_ERROR_CODE] = AH_UT_MSISDNN_ERROR;
		return false;
	}
}

// 得到计费方号码的到访地
//  SET:  VISIT_CODE_A
bool AttribCMProc::getVisitAreaCodeA()
{

	if (ProcessBaseHelper::ExecuteProcedure("BeforeGetVisitAreaCodeA")==1)
		return true;

	// 1.无条件呼转
	//   GSM无条件呼转话单是在发起呼叫的A最近的交换机下话单,
	//   无法判断呼转B的到访地 将M_VISIT_CODE_A=HOME_CODE_A
	if (isUnConditionalForw())
	{
		//国际来访无条件呼转根据交换机代码求到访地
		//国际来访用户根据交换机求到访
		if (m_pcdr[ATTRI_USER_HOME_TYPE_A] > AREA_TYPE_NATIONAL)
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
		//国内用户将其到访置为归属
		else
		{
			m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
			return true;
		}
	}

	// 2.针对其它话单特殊处理
	if ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_POC) || (m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_PTC))
	{
		m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
		return true;
	}

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

	//added by liyp 2008-7-8 18:13卫星通信车
	if(m_globalManager->getAVBySatelCar(m_pcdr[ATTRI_MSC],m_pcdr[ATTRI_LAC_A],m_pcdr[ATTRI_CELLID_A],m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_VISIT_CODE_A]))
	{
			m_pcdr[ATTRI_EDGE_FLAG] = EDGE_ROAM_SATELCAR;
	}

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
bool AttribCMProc::isUnConditionalForw()
{
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
bool AttribCMProc::getHomeAreaCodeB()
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
        if(m_pcdr[ATTRI_ERROR_CODE] == GET_CITY_CODE_ERROR ) {
            return false;
        }

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
                        msisdn=m_pcdr[ATTRI_MSISDN_B].substr(0,7);
                        msisdn.append(7,"0000");
                        m_pcdr[ATTRI_MSISDN_B]=msisdn;

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
        	//Added By Sunam 2006-5-15 9:55
			if ((m_Config->m_provinceCode == "871") &&
            	(m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_NATIONAL_IN) &&
            	(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MTC))
			{
				m_pcdr[ATTRI_HOME_CODE_B]  = "";
				m_pcdr[ATTRI_VISIT_CODE_B] = "";
				m_pcdr[ATTRI_LONG_GROUP1]  = GROUP_ID_B_N;
				//置B的属性
				m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_FIX_NATIONAL;
				setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
      	m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
      	m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
      	return true;
			}

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

          	//Added by Sunam 2005-11-3 国际来访用户LongGroup置为groupId2
          	if ((m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_IN) && (m_pcdr[ATTRI_USER_SERVICE_TYPE_A] != SERVICE_TYPE_KOREA))
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
			//陕西 来访短信 对方号码求不到归属置空
			//漫游来访置缺省号码
			if ((m_Config->m_provinceCode == "290")
			     && ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMMO) || (m_pcdr[ATTRI_CALL_TYPE]== CALL_TYPE_SMMT))
			     && (m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_IN))
			{
				m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]="";
				m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
				m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_VISIT_CODE_A];
				m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_DEFAULT_FIX;

				setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);

				m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
				m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];
				m_pcdr[ATTRI_LONG_GROUP1]  = DEFAULT_GROUP_ID_B;
				return true;
			}
			//Added By Sunam 2006-5-6 20:37
			//云南国际来访短信，对方号求不到归属置为短信中心号码,对端归属地置为计费方费归属地
			if ((m_Config->m_provinceCode == "871") &&
				((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMMO) ||
				 (m_pcdr[ATTRI_CALL_TYPE]== CALL_TYPE_SMMT)) &&
				(m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_IN))
			{
				if (m_pcdr[ATTRI_ORG_PHONE].length() > 0)
				{
					m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_ORG_PHONE];
					m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];
					m_pcdr[ATTRI_ERROR_CODE]  = DEFAULT_ERROR;
					m_pcdr[ATTRI_HOME_CODE_B] = m_pcdr[ATTRI_HOME_CODE_A];
					m_pcdr[ATTRI_USER_TYPE_B] = "0037";
					m_pcdr[ATTRI_USER_ASP_B]  = "0";
					m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = "03";
					m_pcdr[ATTRI_USER_HOME_TYPE_B]    = "7";
					//根据B的归属地查ATTRI_LONG_GROUP1
					m_globalManager->getGroupIdByAreaCodeI(m_pcdr[ATTRI_HOME_CODE_B],
								m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
								m_pcdr[ATTRI_LONG_GROUP1]);
					return true;
				}
			}

			//Added By Sunam 2006-5-15 9:55
			if ((m_Config->m_provinceCode == "871") &&
            	(m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_NATIONAL_IN) &&
            	(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MTC))
			{
				m_pcdr[ATTRI_HOME_CODE_B]  = "";
				m_pcdr[ATTRI_VISIT_CODE_B] = "";
				m_pcdr[ATTRI_LONG_GROUP1]  = GROUP_ID_B_N;
				//置B的属性
				m_pcdr[ATTRI_USER_TYPE_B] = USER_TYPE_B_FIX_NATIONAL;

				setUserAspB(m_pcdr[ATTRI_USER_TYPE_B][0]);
      	m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTR灞炲湴鏌