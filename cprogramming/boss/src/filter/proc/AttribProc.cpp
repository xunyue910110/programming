#include "AttribProc.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"
#include "FilterError.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"


INSTALL_PROCESSBASE_INSTANCE(AttribProc);

int AttribProc::execute()
{
   return 0;
}

//Added by Sunam 2006-6-22 9:00
bool AttribProc::initialize()
{
	m_specialPhone.push_back("1860");
	m_specialPhone.push_back("1861");
	m_specialPhone.push_back("10080");
	m_specialPhone.push_back("10085");
	m_specialPhone.push_back("10086");
	m_specialPhone.push_back("10088");
	m_specialPhone.push_back("12580");
	m_specialPhone.push_back("96106");
	m_specialPhone.push_back("10010");
	m_specialPhone.push_back("10011");
	m_specialPhone.push_back("10012");
	m_specialPhone.push_back("10016");
	m_specialPhone.push_back("10018");

	return true;
}


//判断此条话单是否合法(是否是错误话单)
bool AttribProc::isValidCDR()
{
	if (m_pcdr[ATTRI_ERROR_CODE] == DEFAULT_ERROR)
	{
    	return true;
  }
	return false;
}

//判断此号码是否是本运行商的号码
bool AttribProc::isSelfNumber(const T_ATTRI_VAL &msisdn)
{
  	T_ASP asp;
  	if (m_globalManager->getAspByMsisdnHeader(msisdn,m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],asp))
  	{
  		if (asp.value() == m_Config->m_operatorType) //本运营商
  		{
  			return true;
  		}
  		else
  		{
  			return false;
  		}
  	}
	return false;
}

//判断此号码为手机号码但不是本运行商
bool AttribProc::isOtherNumber(const T_ATTRI_VAL &msisdn)
{
  	T_ASP asp;
  	if (m_globalManager->getAspByMsisdnHeader(msisdn,m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],asp))
  	{
  		if (asp.value() != m_Config->m_operatorType) //非本运营商
  		{
  			return true;
  		}
  		else
  		{
  			return false;
  		}
  	}
	return false;
}

// 此函数根据MSC,LAC和CELL_ID求取到访地
bool AttribProc::getAVByMscLacCellId()
{
	int rd;

	rd = m_globalManager->getAVByMscLacCellid(m_pcdr[ATTRI_MSC],
			m_pcdr[ATTRI_LAC_A],
			m_pcdr[ATTRI_CELLID_A],
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			m_pcdr[ATTRI_VISIT_CODE_A]);
	if (rd <= 0)
	{
		m_pcdr[ATTRI_ERROR_CODE] = AV_MSC_LAC_CELL_ERROR ;
		return false;
	}
	return true;
}

/*-----------------------------------------------------
**Description	: 根据cellid 判断visit_area_code
------------------------------------------------------ */
bool AttribProc::getAVByCellId()
{
	int rd;
	string msc_tmp,lac_tmp;

	rd = m_globalManager->getAVByCellid(m_pcdr[ATTRI_CELLID_A],
								m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
								m_pcdr[ATTRI_VISIT_CODE_A]);
	if (rd <= 0)
	{
		m_pcdr[ATTRI_ERROR_CODE] = AV_MSC_LAC_CELL_ERROR ;
		return false;
	}
	return true;

}

/*-----------------------------------------------------
**Description	: 根据msc,lac 判断visit_area_code
------------------------------------------------------ */
bool AttribProc::getAVByMscLac()
{
	int rd;
	rd = m_globalManager->getAVByMscLac(m_pcdr[ATTRI_MSC],
			m_pcdr[ATTRI_LAC_A],m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			m_pcdr[ATTRI_VISIT_CODE_A]);
	if (rd <= 0)
	{
		m_pcdr[ATTRI_ERROR_CODE] = AV_MSC_LAC_ERROR;
		return false;
	}
	return true;
}

/*-----------------------------------------------------
**Description	: 根据lac 判断visit_area_code
------------------------------------------------------ */
bool AttribProc::getAVByLac()
{
	int rd;
	rd = m_globalManager->getAVByLac(m_pcdr[ATTRI_LAC_A],
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			m_pcdr[ATTRI_VISIT_CODE_A]);
	if (rd <= 0)
	{
		//add by gux 2006-2-27 21:38 陕西要求求不到lac根据交换机号去求
		if(m_Config->m_provinceCode == "290")
		{
			//Modified by Sunam 2006-3-27 16:09
			/*rd = m_globalManager->getAVByMsc(m_pcdr[ATTRI_MSC],
			 m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_VISIT_CODE_A]);*/
			rd = m_globalManager->getAVByMsc(m_pcdr[ATTRI_MSC],ALL_MSC,
			 		m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],m_pcdr[ATTRI_VISIT_CODE_A]);

			 if (rd<= 0)
			 {
			 	 m_pcdr[ATTRI_ERROR_CODE] = AV_MSC_LAC_ERROR;
			 	 return false;
			 }
			 else
			 {
			 	 if((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW) &&
			   		((m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_BUSY)||
			    	(m_pcdr[ATTRI_FORWARD_CAUSE] == FORW_NOANSWER))&&
			   		(m_pcdr[ATTRI_VISIT_CODE_A] == "0919") &&
			   		(m_pcdr[ATTRI_HOME_CODE_A] == "0914"))
						m_pcdr[ATTRI_VISIT_CODE_A] = m_pcdr[ATTRI_HOME_CODE_A];
			 }
		}
		else
		{
			//根据新疆要求,找不到lac根据交换机号去找
			//由于新疆有虚拟局(一个交换机对应几个地市),因此td_lac表维护
			//非虚拟局交换机
			//非虚拟局交换机在td_lac表中lac 维护的是"*" ,以此匹配非虚拟局交换机

			//Modified by Sunam 2006-3-27 16:12
			/*
			T_ATTRI_VAL lacTmp="*";
			rd = m_globalManager->getAVByMscLac(m_pcdr[ATTRI_MSC],lacTmp,
				m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
				m_pcdr[ATTRI_VISIT_CODE_A]);
			*/
			rd = getAVByMsc(NON_VITRUAL_MSC);
			if (rd<= 0)
			{
				m_pcdr[ATTRI_ERROR_CODE] = AV_LAC_ERROR;
				return false;
			}
	  	}
	}
	return true;
}
/*-----------------------------------------------------
**Description	: 根据msc,cellid 判断visit_area_code
------------------------------------------------------ */
bool AttribProc::getAVByMscCellId()
{
	int rd;
	rd = m_globalManager->getAVByMscCellid(m_pcdr[ATTRI_MSC],
			m_pcdr[ATTRI_CELLID_A],m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			m_pcdr[ATTRI_VISIT_CODE_A]);
	if (rd <= 0)
	{
		m_pcdr[ATTRI_ERROR_CODE] = AV_MSC_CELL_ERROR;
		return false;
	}
	return true;
}

/*-----------------------------------------------------
**Description	: 根据msc 判断visit_area_code
------------------------------------------------------ */
bool AttribProc::getAVByMsc(const MSC_TYPE &msc_type)
{
	int rd;
	rd = m_globalManager->getAVByMsc(m_pcdr[ATTRI_MSC],msc_type,
			m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			m_pcdr[ATTRI_VISIT_CODE_A]);
	if (rd <= 0)
	{
		m_pcdr[ATTRI_ERROR_CODE] = AV_MSC_LAC_ERROR;
		return false;
	}
	return true;
}

// 根据LAC,CELLID 判断 VISIT_AREA_CODE
bool AttribProc::getAVByLacCellId()
{
	int rd;
	rd = m_globalManager->getAVByLacCellid(m_pcdr[ATTRI_LAC_A],m_pcdr[ATTRI_CELLID_A],
										m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
										m_pcdr[ATTRI_VISIT_CODE_A]);

	if (rd <= 0)
	{
		/*
			根据青海要求,根据LAC,CELLID找不到到访地，根据交换机号去找
			由于青海有虚拟局(一个交换机对应几个地市),因此td_lac表维护
			非虚拟局交换机
			非虚拟局交换机在TD_LAC表中LAC,及CELLID维护的是"*" ,以此匹配非虚拟局交换机
		*/

		//Modified by Sunam 2006-3-27 16:13
		/*
		T_ATTRI_VAL lacTmp="*";
		rd = m_globalManager->getAVByMscLac(m_pcdr[ATTRI_MSC],lacTmp,
										m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
										m_pcdr[ATTRI_VISIT_CODE_A]);
		*/

		//海南按lac cell查找后，不再查msc
		if (m_Config->m_provinceCode != "898")
		{
		    rd = getAVByMsc(NON_VITRUAL_MSC);
		}

		if (rd<= 0)
		{
			m_pcdr[ATTRI_ERROR_CODE] = AV_LAC_ERROR;
			return false;
		}
	}

	return true;
}

//根据MSISDN找B的HOMEAREACODE
bool AttribProc::getBHByMsisdn(const T_ATTRI_VAL &msisdn)
{
	T_ATTRI_VAL groupATmp;
  T_ATTRI_VAL officeATmp;
	T_ATTRI_VAL calledType;
	int rd;
  rd = m_globalManager->getHomeAreaCodeByMsisdnP(msisdn,
                    m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                    m_pcdr[ATTRI_HOME_CODE_B],
                    groupATmp,
                    m_pcdr[ATTRI_USER_TYPE_B],
                    officeATmp,calledType);

	if (rd > 0)
	{
		if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
		{
			//m_pcdr[ATTRI_ERROR_CODE] = AH_UT_MSISDNP_ERROR;
			return false;
		}
		//m_pcdr[ATTRI_USER_ASP_B]          = m_pcdr[ATTRI_USER_TYPE_B][0];
		m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
		m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];

		return true;
	}
	else
	{
    rd = m_globalManager->getHomeAreaCodeByPhoneN(msisdn,
											m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
                      m_pcdr[ATTRI_HOME_CODE_B],
                      groupATmp,
                      m_pcdr[ATTRI_USER_TYPE_B],
                      officeATmp,calledType);

		if (rd <= 0)
		{
			//m_pcdr[ATTRI_ERROR_CODE] = AH_MSISDNN_ERROR;
			return false;
		}
		else
		{
			if (m_pcdr[ATTRI_USER_TYPE_B].length() != USER_TYPE_LENGTH)
			{
  				//m_pcdr[ATTRI_ERROR_CODE] = AH_UT_MSISDNN_ERROR;
  				return false;
			}
		  //m_pcdr[ATTRI_USER_ASP_B]          = m_pcdr[ATTRI_USER_TYPE_B][0];
		  m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_pcdr[ATTRI_USER_TYPE_B].substr(1,2);
		  m_pcdr[ATTRI_USER_HOME_TYPE_B]    = m_pcdr[ATTRI_USER_TYPE_B][3];

			return true;
		}
	}
 return true;
}

//将msisdn根据规则转化成imsi
//134号段应用msisdn与imsi对应关系重定义
bool AttribProc::msisdnToImsi(const T_ATTRI_VAL &msisdn, T_ATTRI_VAL &imsi)
{
	char cTmp = '0';
	string phone = msisdn;

	switch(m_Config->m_operatorType)
	{
		case OPERATOR_TYPE_CMCC:
		/*
			if(msisdn[0]== '4' && msisdn[1] != '9') //1340-1348移动号段
			{
				imsi = imsi + '0';
				imsi = imsi + msisdn[1];
				imsi = imsi + msisdn[2];
				imsi = imsi + msisdn[3];
				imsi = imsi + msisdn[4];
				imsi = imsi + ("46002");
				imsi = imsi + ("00000");
			}
			else //135-139移动号段
			{
				if(msisdn[1] == '0')  //136051579722
				{
					imsi = imsi + msisdn[2];  //5
					imsi = imsi + msisdn[3];  //1
					imsi = imsi + msisdn[4];  //5
					imsi = imsi + msisdn[0];  //6
					imsi = imsi + msisdn[1];  //0
				}
				else  //13851579712
				{
					imsi = imsi + msisdn[2];   //1
					imsi = imsi + msisdn[3];   //5
					imsi = imsi + msisdn[4];   //7
					cTmp = msisdn[0] - 5;
					imsi = imsi + cTmp;
					imsi = imsi + msisdn[1];   //5
				}
				imsi = imsi + ("46000");
				imsi = imsi + ("00000");
			}
		*/
			//1340-1348移动号段
			if (!(phone.compare(0,3,"134")) && (phone.compare(0,4,"1349")))
			{
				imsi = imsi + '0';
				imsi = imsi + msisdn[3];
				imsi = imsi + msisdn[4];
				imsi = imsi + msisdn[5];
				imsi = imsi + msisdn[6];
				imsi = ("46002") + imsi + ("00000");
			}
			//159号段
			else if(!(phone.compare(0,3,"159")))
			{
				imsi = imsi + '9';
				imsi = imsi + msisdn[3];
				imsi = imsi + msisdn[4];
				imsi = imsi + msisdn[5];
				imsi = imsi + msisdn[6];
				imsi = ("46002") + imsi + ("00000");
			}
			//Added by Sunam 增加158号段的处理
			else if(!(phone.compare(0,3,"158")))
			{
				imsi = imsi + '8';
				imsi = imsi + msisdn[3];
				imsi = imsi + msisdn[4];
				imsi = imsi + msisdn[5];
				imsi = imsi + msisdn[6];
				imsi = ("46002") + imsi + ("00000");
			}
			//Added by Sunam 2007-6-27 15:00 增加150号段的处理
			else if(!(phone.compare(0,3,"150")))
			{
				imsi = imsi + '3';
				imsi = imsi + msisdn[3];
				imsi = imsi + msisdn[4];
				imsi = imsi + msisdn[5];
				imsi = imsi + msisdn[6];
				imsi = ("46002") + imsi + ("00000");
			}

			//151号段 Added by liyp 2008-2-20 17:14 增加151号段的处理
			else if(!(phone.compare(0,3,"151")))
			{
				imsi = imsi + '1';
				imsi = imsi + msisdn[3];
				imsi = imsi + msisdn[4];
				imsi = imsi + msisdn[5];
				imsi = imsi + msisdn[6];
				imsi = ("46002") + imsi + ("00000");
			}

			//135-139移动号段
			else
			{
				if(msisdn[3] == '0')  //136051579722
				{
					imsi = imsi + msisdn[4];  //5
					imsi = imsi + msisdn[5];  //1
					imsi = imsi + msisdn[6];  //5
					imsi = imsi + msisdn[2];  //6
					imsi = imsi + msisdn[3];  //0
				}
				else  //13851579712
				{
					imsi = imsi + msisdn[4];   //1
					imsi = imsi + msisdn[5];   //5
					imsi = imsi + msisdn[6];   //7
					cTmp = msisdn[2] - 5;
					imsi = imsi + cTmp;
					imsi = imsi + msisdn[3];   //5
				}
				imsi = ("46000") + imsi + ("00000");
			}

			return true;
			break;
		case OPERATOR_TYPE_UNICOM:
		/*
			imsi = imsi + msisdn[2];
			imsi = imsi + msisdn[3];
			imsi = imsi + msisdn[4];
			imsi = imsi + msisdn[1];
			imsi = imsi + msisdn[0];
		*/
			imsi = imsi + msisdn[4];
			imsi = imsi + msisdn[5];
			imsi = imsi + msisdn[6];
			imsi = imsi + msisdn[3];
			imsi = imsi + msisdn[2];
			return true;
			break;
	}
	return true;
}

bool AttribProc::imsiToMsisdn(const T_ATTRI_VAL &imsi, T_ATTRI_VAL &msisdn)
{
    string simsi = imsi;
	switch(m_Config->m_operatorType)
	{
		case OPERATOR_TYPE_CMCC:
			return true;
			break;
		case OPERATOR_TYPE_UNICOM:
		/*
		    133010  133019       460030900+msisdn+5
        133020  133499       4600309  +msisdn+3
        133500  133599       46003030 +msisdn+4
        133600  133699       4600309  +msisdn+3
        133700  133739       4600309  +msisdn+3
        133750  133799       4600309  +msisdn+3
        133800  133899       4600309  +msisdn+3
        133900  133979       4600309  +msisdn+3
        133980  133989       460030953+msisdn+5
        133990  133999       460030954+msisdn+5

        if (simsi.compare(0,9,"460030954")==0)
		        msisdn = ("13399") + simsi.substr(9,6);
		    else if (simsi.compare(0,9,"460030953")==0)
		        msisdn = ("13398") + simsi.substr(9,6);
		    else if (simsi.compare(0,9,"460030900")==0)
		        msisdn = ("13301") + simsi.substr(9,6);
		    else if (simsi.compare(0,8,"46003030")==0)
		        msisdn = ("1335") + simsi.substr(8,7);
		    else if (simsi.compare(0,7,"4600309")==0)
		        msisdn = ("133") + simsi.substr(7,8);
		    else if (simsi.compare(0,6,"460036")==0 &&
			  (simsi[8]=='0' || simsi[8]=='1'))
		        msisdn = ("153") + simsi.substr(6,2) + simsi.substr(9,6);
		*/

		/*
		    13+0+ABCD+XXXX       46001+DABC+0
		    13+0+ABCD+XXXX       46001+DABC+1
		    13+1+ABCD+XXXX       46001+DABC+9
		    13+2+ABCD+XXXX       46001+DABC+2
		    15+6+ABCD+XXXX       46001+DABC+3
		    15+5+ABCD+XXXX       46001+DABC+4
		    
		    18+6+ABCD+XXXX       46001+DABC+6  // add 2009-7-7 10:04:02
		    14+5+ABCD+XXXX       46001+DABC+7  // add 2009-7-7 10:04:02
		*/
      if (simsi.compare(0,5,"46001")==0)
      {
        if(simsi.compare(9,1,"0")==0 || simsi.compare(9,1,"1")==0)
          	msisdn = ("130") + simsi.substr(8,1) + simsi.substr(5,3) + simsi.substr(9,4);
        else if(simsi.compare(9,1,"9")==0)
          	msisdn = ("131") + simsi.substr(8,1) + simsi.substr(5,3) + simsi.substr(9,4);
        else if(simsi.compare(9,1,"2")==0)
          	msisdn = ("132") + simsi.substr(8,1) + simsi.substr(5,3) + simsi.substr(9,4);
        else if(simsi.compare(9,1,"3")==0)
          	msisdn = ("156") + simsi.substr(8,1) + simsi.substr(5,3) + simsi.substr(9,4);
        else if(simsi.compare(9,1,"4")==0)
          	msisdn = ("155") + simsi.substr(8,1) + simsi.substr(5,3) + simsi.substr(9,4);
        else if(simsi.compare(9,1,"6")==0) // add 2009-7-7 10:04:16
          	msisdn = ("186") + simsi.substr(8,1) + simsi.substr(5,3) + simsi.substr(9,4);
        else if(simsi.compare(9,1,"7")==0) // add 2009-7-7 10:04:21
          	msisdn = ("145") + simsi.substr(8,1) + simsi.substr(5,3) + simsi.substr(9,4);
        else
          	return false;
        return true;
      }
			break;
	}
	return false;
}

//根据IMSI找A的HOMEAREACODE
bool AttribProc::getAHByImsi(const T_ATTRI_VAL &imsi_number)
{
	T_ATTRI_VAL sTmp;
	T_ATTRI_VAL groupATmp;//A group ID 从参数表取出,不用
	T_ATTRI_VAL imsi;
	int rd;

	//传进来的imsi_number一定是15位

	//15位的IMSI,先从td_imsip找,再从td_imsin找,最后td_imsii
	{
		//国内
		if (!strncmp(imsi_number.c_str(),"46000",5)
		   || !strncmp(imsi_number.c_str(),"46001",5)
	     || !strncmp(imsi_number.c_str(),"46002",5)
	     || !strncmp(imsi_number.c_str(),"46003",5))
  		{
    		//从省内IMSI表找
        rd = m_globalManager->getHomeAreaCodeByImsiP(imsi_number,
                                m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
                                m_pcdr[ATTRI_HOME_CODE_A],
                                groupATmp,
                                m_pcdr[ATTRI_USER_TYPE_A],
                                m_pcdr[ATTRI_OFFICE_CODE]);

    		if (rd > 0)
    		{
    			if (m_pcdr[ATTRI_USER_TYPE_A].length() != USER_TYPE_LENGTH)
    			{
      				m_pcdr[ATTRI_ERROR_CODE] = AH_UT_IMSIP_ERROR;
      				return false;
    			}
    			m_pcdr[ATTRI_USER_ASP_A]  = m_pcdr[ATTRI_USER_TYPE_A][0];
  				m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_USER_TYPE_A].substr(1,2);
  				m_pcdr[ATTRI_USER_HOME_TYPE_A]    = m_pcdr[ATTRI_USER_TYPE_A][3];
    			return true;
    		}
    		else
    		{
  			 //从国内IMIS表找
            rd = m_globalManager->getHomeAreaCodeByImsiN(imsi_number,
                                  m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
                                  m_pcdr[ATTRI_HOME_CODE_A],groupATmp,
                                  m_pcdr[ATTRI_USER_TYPE_A],
                                  m_pcdr[ATTRI_OFFICE_CODE]);

      			if (rd <= 0)
      			{
        			m_pcdr[ATTRI_ERROR_CODE] = AH_IMSIN_ERROR;
        			return false;
      			}
      			else
      			{
        			if (m_pcdr[ATTRI_USER_TYPE_A].length() != USER_TYPE_LENGTH)
	    			{
	      				m_pcdr[ATTRI_ERROR_CODE] = AH_UT_IMSIN_ERROR;
	      				return false;
	    			}
	    			m_pcdr[ATTRI_USER_ASP_A]          = m_pcdr[ATTRI_USER_TYPE_A][0];
  					m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_USER_TYPE_A].substr(1,2);
  					m_pcdr[ATTRI_USER_HOME_TYPE_A]    = m_pcdr[ATTRI_USER_TYPE_A][3];
	    			return true;
      			}
    		}
  		}
  		//国际用户的处理
  		else
  		{
  			//imsi扩充为6位
  			//imsi = imsi_number.substr(0,5);
        rd = m_globalManager->getAreaCodeByImsiI(imsi_number, m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME], m_pcdr[ATTRI_HOME_CODE_A],
                                                 groupATmp, m_pcdr[ATTRI_USER_TYPE_A],m_pcdr[ATTRI_INTER_ASPCODE],m_pcdr[ATTRI_INTER_ASPGROUP]);

    		if (rd <= 0)
    		{
      			m_pcdr[ATTRI_ERROR_CODE] = AH_IMSII_ERROR;
      			return false;
    		}
    		if (m_pcdr[ATTRI_USER_TYPE_A].length() != USER_TYPE_LENGTH)
			  {
  				m_pcdr[ATTRI_ERROR_CODE] = AH_UT_IMSII_ERROR;
  				return false;
			  }
  			m_pcdr[ATTRI_USER_ASP_A]          = m_pcdr[ATTRI_USER_TYPE_A][0];
  			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_USER_TYPE_A].substr(1,2);
  			m_pcdr[ATTRI_USER_HOME_TYPE_A]    = m_pcdr[ATTRI_USER_TYPE_A][3];

    		m_pcdr[ATTRI_OFFICE_CODE] =  DEFAULT_OFFICECODE;
    		return true;
  		}
	}
}

//根据MSISDN找A的HOMEAREACODE
bool AttribProc::getAHByMsisdn(const T_ATTRI_VAL &msisdn)
{
	T_ATTRI_VAL groupATmp;
	//Added by Sunam 2005-12-13
	T_ATTRI_VAL calledType;
	int rd;
  rd = m_globalManager->getHomeAreaCodeByMsisdnP(msisdn,
                    m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                    m_pcdr[ATTRI_HOME_CODE_A],
                    groupATmp,
                    m_pcdr[ATTRI_USER_TYPE_A],
                    m_pcdr[ATTRI_OFFICE_CODE],calledType);

	if (rd > 0)
	{
		if (m_pcdr[ATTRI_USER_TYPE_A].length() != USER_TYPE_LENGTH)
		{
			m_pcdr[ATTRI_ERROR_CODE] = AH_UT_MSISDNP_ERROR;
			return false;
		}
		m_pcdr[ATTRI_USER_ASP_A]          = m_pcdr[ATTRI_USER_TYPE_A][0];
		m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_USER_TYPE_A].substr(1,2);
		m_pcdr[ATTRI_USER_HOME_TYPE_A]    = m_pcdr[ATTRI_USER_TYPE_A][3];

		return true;
	}
	else
	{
    rd = m_globalManager->getHomeAreaCodeByPhoneN(msisdn,
											m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
                      m_pcdr[ATTRI_HOME_CODE_A],
                      groupATmp,
                      m_pcdr[ATTRI_USER_TYPE_A],
                      m_pcdr[ATTRI_OFFICE_CODE],calledType);

		if (rd <= 0)
		{
			m_pcdr[ATTRI_ERROR_CODE] = AH_MSISDNN_ERROR;
			return false;
		}
		else
		{
			if (m_pcdr[ATTRI_USER_TYPE_A].length() != USER_TYPE_LENGTH)
			{
  				m_pcdr[ATTRI_ERROR_CODE] = AH_UT_MSISDNN_ERROR;
  				return false;
			}
			m_pcdr[ATTRI_USER_ASP_A]          = m_pcdr[ATTRI_USER_TYPE_A][0];
			m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_USER_TYPE_A].substr(1,2);
			m_pcdr[ATTRI_USER_HOME_TYPE_A]    = m_pcdr[ATTRI_USER_TYPE_A][3];

			return true;
		}
	}
}

bool AttribProc::getBVByMsrn()
{
	T_ATTRI_VAL m123;
	T_ATTRI_VAL sTmp1;
	T_ATTRI_VAL sTmp2;
	int rd;

	#ifdef UC_NOMSRN
		m_pcdr[ATTRI_VISIT_CODE_B] = m_pcdr[ATTRI_HOME_CODE_B];
		return true;
	#endif /* UC_NOMSRN */

  switch(m_Config->m_operatorType)
	{
		case OPERATOR_TYPE_CMCC:
				// GSM Mobile MOC 注:呼转话单不判动态漫游号(漫游上发会有问题),漫游将呼转的msrn置空
				// 省内呼转还是要根据msrn判断对方所在地
				//Modified By Sunam 2006-11-7 10:30 D文件检错标准 F072错单
				//如果是以13900或1374开头的数字，且被叫是中国移动用户时，前8位必须为“交换机号与省市长途区号对照表”中的有效交换机代码
				if (((!m_pcdr[ATTRI_MSRN].compare(0, 5, "13900")) ||
						 (!m_pcdr[ATTRI_MSRN].compare(0, 4, "1374")) ||
						 (!m_pcdr[ATTRI_MSRN].compare(0, 4, "1344")))
					    && ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MOC) || ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW)
						  &&(m_pcdr[ATTRI_ROAM_TYPE] < ROAM_TYPE_NATIONAL_IN)))
				  	  && (isSelfNumber(m_pcdr[ATTRI_MSISDN_B]) && (m_pcdr[ATTRI_USER_SERVICE_TYPE_B] != SERVICE_TYPE_TDSCDMA)))
				{
					m123 = m_pcdr[ATTRI_MSRN].substr(0, 8);
					m123.append(0,"86");
			
					rd = m_globalManager->getHomeAreaCodeByM123(m123, m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
															m_pcdr[ATTRI_VISIT_CODE_B], sTmp1, sTmp2);
			
					if (rd <= 0)
					{
				  		m_pcdr[ATTRI_ERROR_CODE] = BV_M123_ERROR;
				  		return false;
					}
					return true;
				}
				else
				{
					m_pcdr[ATTRI_VISIT_CODE_B] = m_pcdr[ATTRI_HOME_CODE_B] ;
					return true;
				}
    		break;
		case OPERATOR_TYPE_UNICOM:
				// GSM Mobile MOC 注:呼转话单不判动态漫游号(漫游上发会有问题),漫游将呼转的msrn置空
				// 省内呼转还是要根据msrn判断对方所在地
				if ((m_pcdr[ATTRI_MSRN].compare(0, 2, S13)==0 || m_pcdr[ATTRI_MSRN].compare(0, 2, S15)==0)
					  && ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MOC) || ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW)
						&&(m_pcdr[ATTRI_ROAM_TYPE] < ROAM_TYPE_NATIONAL_IN)))
				  	&& (isSelfNumber(m_pcdr[ATTRI_MSISDN_B]) && (m_pcdr[ATTRI_USER_SERVICE_TYPE_B] != SERVICE_TYPE_TDSCDMA)))
				{
					m123 = m_pcdr[ATTRI_MSRN].substr(0, 8);
					m123.append(0,"86");

					rd = m_globalManager->getHomeAreaCodeByM123(m123, m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
															m_pcdr[ATTRI_VISIT_CODE_B], sTmp1, sTmp2);

					if (rd <= 0)
					{
				  		m_pcdr[ATTRI_ERROR_CODE] = BV_M123_ERROR;
				  		return false;
					}
					return true;
				}
				else
				{
					m_pcdr[ATTRI_VISIT_CODE_B] = m_pcdr[ATTRI_HOME_CODE_B] ;
					return true;
				}
    		break;
	}
	return false;

}

bool AttribProc::getBVByLacMsrn()
{
	T_ATTRI_VAL m123;
	T_ATTRI_VAL sTmp1;
	T_ATTRI_VAL sTmp2;
	int rd;

	rd = m_globalManager->getAVByMscLacCellid(m_pcdr[ATTRI_MSC],
	                                      m_pcdr[ATTRI_LAC_B],
	                                      m_pcdr[ATTRI_CELLID_B],
	                                      m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
	                                      m_pcdr[ATTRI_VISIT_CODE_B]);
	if (rd <= 0)
	{
		if ((!m_pcdr[ATTRI_MSRN].compare(0, 2, S13)) && (isOriginalCall())
	    	&& ((!m_pcdr[ATTRI_MSISDN_B].compare(0, 2, S13))
	        	|| (!m_pcdr[ATTRI_MSISDN_B].compare(0, 3, S013))))
		{
	    m123 = m_pcdr[ATTRI_MSRN].substr(0, 8);
			m123.append(0,"86");

			rd = m_globalManager->getHomeAreaCodeByM123(m123, m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
										  m_pcdr[ATTRI_VISIT_CODE_B], sTmp1, sTmp2);

	  		if (rd <= 0)
	  		{
	    		m_pcdr[ATTRI_ERROR_CODE] = BV_M123_ERROR;
	    		return false;
	  		}
		}
		else
		{
	  		m_pcdr[ATTRI_VISIT_CODE_B] = m_pcdr[ATTRI_HOME_CODE_B] ;
	  		return true;
		}
	}
	return true;
}



//到访地=归属地
bool AttribProc::getBVByHome()
{
	m_pcdr[ATTRI_VISIT_CODE_B] = m_pcdr[ATTRI_HOME_CODE_B] ;
	return true;
}

bool AttribProc::isVoiceBox(const T_ATTRI_VAL &msisdn)
{
	if (msisdn.length() < 8)
		return false;

  switch(m_Config->m_operatorType)
	{
		case OPERATOR_TYPE_CMCC:
			if (strncmp(msisdn.c_str(),MVOICEBOX_NUMBER.c_str(),5)==0 )
    		{
				// 海南移动要求根据城市代码找区号和省代码等
				T_ATTRI_VAL citycode;
				citycode = msisdn.substr(5,3);

        if (m_globalManager->getAreacodeByCityCode(citycode,
                             m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
                             m_pcdr[ATTRI_HOME_CODE_B],
                             m_pcdr[ATTRI_PROVINCE_CODE_B],
                             m_pcdr[ATTRI_LONG_GROUP1],
                             m_pcdr[ATTRI_USER_TYPE_B]))
				{
					//use_type重置
					m_pcdr[ATTRI_USER_TYPE_B][0] = COMPANY_TYPE_CMCC[0];
					m_pcdr[ATTRI_USER_TYPE_B][1] = SERVICE_TYPE_GSM[0];
					m_pcdr[ATTRI_USER_TYPE_B][2] = SERVICE_TYPE_GSM[1];
					return true;
				}
				else
				{
					m_pcdr[ATTRI_ERROR_CODE] = GET_CITY_CODE_ERROR ;
					return false;
				}
			}
    		break;
		case OPERATOR_TYPE_UNICOM:
			if(strncmp(msisdn.c_str(),UVOICEBOX_NUMBER.c_str(),5)==0 )
    		{
    			T_ATTRI_VAL citycode;
      			if (msisdn[7] == '0')
        		{
        			m_pcdr[ATTRI_HOME_CODE_B] = msisdn.substr(5,2);
      				m_pcdr[ATTRI_HOME_CODE_B].append(0,'0');
      			}
      			else
        		{
        			m_pcdr[ATTRI_HOME_CODE_B] = msisdn.substr(5,3);
      				m_pcdr[ATTRI_HOME_CODE_B].append(0,'0');
        		}

            if (m_globalManager->getCitycodeByAreaCode(citycode,
                                  m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],
                                  m_pcdr[ATTRI_HOME_CODE_B],
                                  m_pcdr[ATTRI_PROVINCE_CODE_B],
                                  m_pcdr[ATTRI_LONG_GROUP1],
                                  m_pcdr[ATTRI_USER_TYPE_B]))

	  				{
	  					//use_type重置
	  					m_pcdr[ATTRI_USER_TYPE_B][0] = COMPANY_TYPE_UNICOM[0];
					    m_pcdr[ATTRI_USER_TYPE_B][1] = SERVICE_TYPE_GSM[0];
					    m_pcdr[ATTRI_USER_TYPE_B][2] = SERVICE_TYPE_GSM[1];
	  					return true;
	  				}
	  				else
	  				{
	  					m_pcdr[ATTRI_ERROR_CODE] = GET_CITY_CODE_ERROR ;
	  					return false;
						}
    		}
    		break;
		default:
			break;
	}
	return false;
}

bool AttribProc::isOriginalCall()
{
	if ((m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_MOC) ||
		(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_FORW)||
		(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMMO)||
		(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMMT))
	{
		return true;
	}
	return false;
}


//判断漫游标志
void AttribProc::setRoamType()
{
  //在GetHomeCode里,HOME_TYPE_A 已经定义了是本省用户,国内用户,和国际用户

  //将本省用户区分出是否漫游
  if (m_pcdr[ATTRI_USER_HOME_TYPE_A] < AREA_TYPE_NATIONAL)
  {
      if (isRoam())//本省用户漫游,则重置地区类型
          m_pcdr[ATTRI_USER_HOME_TYPE_A] = AREA_TYPE_PROVINCE;
      else
          m_pcdr[ATTRI_USER_HOME_TYPE_A] = AREA_TYPE_LOCAL;
  }

  if (m_pcdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_LOCAL)
  {
      m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NONE; //非漫游 - 0
  }
  else if (m_pcdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_PROVINCE)
  {
      m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_PRO_IN;//省内漫游　- 1
  }
  else if (m_pcdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_NATIONAL)
  {
      m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NATIONAL_IN;  //国内来访　- 3
  }
  else
  {
      m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_INTERNATIONAL_IN; //国际来访　-5
  }

  //本地化的漫游类型处理
  ProcessBaseHelper::ExecuteProcedure("AfterSetRoamType");
}

//判断两个地方是否是虚拟局
bool AttribProc::isVirtualArea(const T_ATTRI_VAL &r_areaCode1,const T_ATTRI_VAL &r_areaCode2,const T_ATTRI_VAL &r_hometype)
{
	return	m_globalManager->isVirtualArea(r_areaCode1,r_areaCode2,r_hometype,m_pcdr[ATTRI_USER_ASP_A],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME]);
}

//判断两个地区是否同一地
bool AttribProc::isSameArea(const T_ATTRI_VAL &areaCode1, const T_ATTRI_VAL &areaCode2)
{
	if (areaCode1 == areaCode2 || isVirtualArea(areaCode1, areaCode2,m_pcdr[ATTRI_USER_HOME_TYPE_A]))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 在求属性之前特殊处理
// TODO ??????
bool AttribProc::specialPreProcessCdr()
{
    int rd;
    char sTmp1[24 + 1];
    char sTmp2[24 + 1];
    T_ATTRI_VAL tTmp;
    memset(sTmp1,0, sizeof(sTmp1));
    memset(sTmp2,0, sizeof(sTmp2));

  //1.计费号码标准化 (手机号码去除 0086,086,86)
	if ((strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(),"0086",4) == 0) && (isSelfNumber(m_pcdr[ATTRI_MSISDN_A].c_str() + 4)))
	{
		m_pcdr[ATTRI_MSISDN_A] = m_pcdr[ATTRI_MSISDN_A].c_str() + 4;
	}
	else if ((strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(),"086",3) == 0) && (isSelfNumber(m_pcdr[ATTRI_MSISDN_A].c_str() + 3)))
	{
		m_pcdr[ATTRI_MSISDN_A] = m_pcdr[ATTRI_MSISDN_A].c_str() + 3;
	}
	else if ((strncmp(m_pcdr[ATTRI_MSISDN_A].c_str(),"86",2) == 0) && (isSelfNumber(m_pcdr[ATTRI_MSISDN_A].c_str() + 2)))
	{
		m_pcdr[ATTRI_MSISDN_A] = m_pcdr[ATTRI_MSISDN_A].c_str() + 2;
	}

   	//3 交换机号 针对西门子话单交换机号下1EFF1C 需转换成标准格式,从td_prov_msc里根据别名转成msc号
	T_ATTRI_VAL msc;
	if (strncmp(m_pcdr[ATTRI_MSC].c_str(),"8613",4) != 0)
	{
		if (m_globalManager->getMscByAlias(m_pcdr[ATTRI_MSC],m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],msc))
		{
			m_pcdr[ATTRI_MSC] = msc;
		}
	}

	return true;
}

bool AttribProc::otherPartyStandardization()
{
	string otherParty;
	int rd;
    char sTmp1[24 + 1];
    char sTmp2[24 + 1];
    T_ATTRI_VAL tTmp;

    memset(sTmp1,0, sizeof(sTmp1));
    memset(sTmp2,0, sizeof(sTmp2));
	//对方号码标准化
	//1.去0086
	if (strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "0086", 4) == 0)
	{
		m_pcdr[ATTRI_MSISDN_B]  = m_pcdr[ATTRI_MSISDN_B].c_str() + 4;
		if(m_pcdr[ATTRI_MSISDN_B].length() > 0)
		{
			if(m_pcdr[ATTRI_MSISDN_B][0] != '0')
			{
				m_pcdr[ATTRI_MSISDN_B].append(0,"0");
			}
		}
		m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];
	}
	//1.去+86
	if (strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "+86", 3) == 0)
	{
		m_pcdr[ATTRI_MSISDN_B]  = m_pcdr[ATTRI_MSISDN_B].c_str() + 3;
		if(m_pcdr[ATTRI_MSISDN_B].length() > 0)
		{
			if(m_pcdr[ATTRI_MSISDN_B][0] != '0')
			{
				m_pcdr[ATTRI_MSISDN_B].append(0,"0");
			}
		}
		m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];
	}
	//2.去86
	if (strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "86", 2) == 0)
	{
		otherParty = m_pcdr[ATTRI_VISIT_CODE_A] + m_pcdr[ATTRI_MSISDN_B];

		//613或者86159开始...，并且长度大于等于13，不去找省内号码段
		if ((m_pcdr[ATTRI_MSISDN_B].length() < 13) || (!isMobile(m_pcdr[ATTRI_MSISDN_B].c_str() + 2)))
		{
			//如果是省内固定号码,则不做处理
			if (m_globalManager->isProvFixTelephone(otherParty,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME]))
			{
				return true;
			}
		}
		if (strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "860", 3) == 0)
		{
			//if ((strlen(m_pcdr[ATTRI_MSISDN_B].c_str()) > 8) || (strlen(m_pcdr[ATTRI_MSISDN_B].c_str()) < 7))
			//add by shenglj 20090205 16:08:01 号码>=10位才去86
			if ((strlen(m_pcdr[ATTRI_MSISDN_B].c_str()) >= 10) || (strlen(m_pcdr[ATTRI_MSISDN_B].c_str()) < 7))
			{
				m_pcdr[ATTRI_MSISDN_B]     = m_pcdr[ATTRI_MSISDN_B].c_str() + 2;
				m_pcdr[ATTRI_MSISDN_B_FOR_ROAM] = m_pcdr[ATTRI_MSISDN_B];
			}
		}
		//86+
		else if (strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "86", 2) == 0)
		{
			//86+1
			if (strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "861", 3) == 0)
			{
				//a.8610 且不是861001 且长度>8 ( 我们认为应该是 010 86==>0)
				if ((strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "8610",4) == 0) &&
					(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "861001", 6) != 0) &&
					(strlen(m_pcdr[ATTRI_MSISDN_B].c_str()) > 8))
				{
					m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_MSISDN_B].c_str() + 2;
					m_pcdr[ATTRI_MSISDN_B].append(0,"0");
				}
				//b.861,且长度>8或者<7 (如8617951 86172等特殊号码 或者86139 (去86))
				//add by shenglj 20090201 22:05:01 号码>=13位才去86
				//else if ((strlen( m_pcdr[ATTRI_MSISDN_B].c_str()) > 8) || (strlen(m_pcdr[ATTRI_MSISDN_B].c_str()) < 7))
				else if ((strlen( m_pcdr[ATTRI_MSISDN_B].c_str()) >= 13) || (strlen(m_pcdr[ATTRI_MSISDN_B].c_str()) < 7))
				{
					m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_MSISDN_B].c_str() + 2;
				}
			}
			//86但不是861 也不是860 大于8位(86==>0)
			//江苏联通出现长度>=10位的86开头的固话 加判是否手机号 by shenglj 2009-01-09 23:58:01
			else if dr[ATTRI_MSISDN_B].c_str(), "861001", 6) != 0) &&
					(strlen(m_pcdr[ATTRI_MSISDN_B].c_str()) > 8))
				{
					m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_MSISDN_B].c_str() + 2;
					m_pcdr[ATTRI_MSISDN_B].append(0,"0");
				}
				//b.861,涓旈暱搴