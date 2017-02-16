#include "SmCDRUNProc.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(SmCDRUNProc);

bool SmCDRUNProc::InitCdrProperty()
{
	reset();
	m_origProperty.clear();
	m_destProperty.clear();
	m_spProperty.clear();

  m_origProperty.call_type  = CALL_TYPE_SMT;
  m_destProperty.call_type  = CALL_TYPE_SMO;
  m_origProperty.begin_time = m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME];
  m_origProperty.end_time   = m_pcdr[ATTRI_END_DATE]+m_pcdr[ATTRI_END_TIME];
  m_destProperty.begin_time = m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME];
  m_destProperty.end_time   = m_pcdr[ATTRI_END_DATE]+m_pcdr[ATTRI_END_TIME];
  m_spProperty.begin_time   = m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME];
  m_spProperty.end_time     = m_pcdr[ATTRI_END_DATE]+m_pcdr[ATTRI_END_TIME];

  return true;
}

bool SmCDRUNProc::getTrash()
{

	 int t_iRet;
	 //本地化sm trash判断
	 t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeSmTrashGet");
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


bool SmCDRUNProc::getSmsMethod()
{
	if(m_Config->m_smsProcessMethod.count(m_pcdr[ATTRI_SOURCE_TYPE].c_str()))
	{
		 m_smsMethod = (m_Config->m_smsProcessMethod)[m_pcdr[ATTRI_SOURCE_TYPE].c_str()];
	}
	else
	{
    theErrorMessages->insert( SM_ERROR_SOURCE_TYPE,"cannot find source_type: "+m_pcdr[ATTRI_SOURCE_TYPE] );
    #ifdef _DEBUG_
      cout<<"cannot find source_type: "<<m_pcdr[ATTRI_SOURCE_TYPE]<<"  "
          <<__FILE__<<__LINE__<<endl;
    #endif

    m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_SOURCE_TYPE;
    return false;
	}
  return true;
}

bool SmCDRUNProc::getDisChannelProperty()
{
	//1. 无效话单
	if(getTrash())
    return true;

	//2. 错误话单
	if(getError())
		 return true;

	switch(m_smsMethod)
	{
		case SM_PROCESS_METHOD:
				  getSmServType();
			break;
		case SP_PROCESS_METHOD:
			    getSpServType();
			break;
		case USSD_PROCESS_METHOD:
			    getUssdServType();
			break;
		case HY_PROCESS_METHOD:
			    getHyOperCode();
			break;
		case UNITE_PPS_PROCESS_METHOD:
			    getPpsSmServType();
			break;
    case FIX_SM_PROCESS_METHOD:
    	    getFixSmServType();
			break;
    case FIX_SP_PROCESS_METHOD:
    	    getFixSpServType();
			break;
		default:
		    #ifdef _DEBUG_
		      cout<<"not define sms deal method: "<<m_smsMethod<<""<<__FILE__<<__LINE__<<endl;
		    #endif
			break;
	}

  return true;
}

bool SmCDRUNProc::getSmServType()
{
	//分析号码
	if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_GSMSC)
	{
		 ParseNumber();
	}
	else if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_PPS1)
	{
		 ParsePpsNumber();
	}
	else
	{
		 ParseLinkage();
	}

  return true;
}

bool SmCDRUNProc::ParseLinkage()
{
   T_ATTRI_VAL   areaCode;
   T_ATTRI_VAL   groupId;
   T_ATTRI_VAL   userType;
   T_ATTRI_VAL   std_number;
   //计费方检查
   if(!checkMsisdn(m_pcdr[ATTRI_MSISDN_A]))
       return false;

	 std_number = m_pcdr[ATTRI_MSISDN_B];

   //固网号码：区号+号码 取消了106并存期
   if(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "0", 1) == 0 &&
   	  m_globalManager->getHomeAreaCodeByMsisdnN(m_pcdr[ATTRI_MSISDN_B],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],areaCode,groupId,userType))
   {
   	   //默认对端电信
       m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]   = m_pcdr[ATTRI_MSISDN_B];
       m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SM_SERV_TYPE_FIX;
       m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B] = "0";
       m_pcdr[ATTRI_USER_ASP_B]          = COMPANY_TYPE_CT;
       m_pcdr[ATTRI_SM_TYPESTAT]         = "CTC";
       return true;
   }

   //查短信中心号码表
   if (m_globalManager->getSmServCode(std_number.c_str(),m_destProperty))
   {
       m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]   = m_pcdr[ATTRI_MSISDN_B];
       m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_destProperty.serv_type;
       m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B] = m_destProperty.serv_code_type;
       m_pcdr[ATTRI_USER_ASP_B]          = m_destProperty.asp;
       m_pcdr[ATTRI_SM_TYPESTAT]         = m_destProperty.sm_typestat;
       return true;
   }
   else
   {
       m_pcdr[ATTRI_ERROR_CODE] = SM_ERROR_PROT;
       return false;
   }

  return true;
}

bool SmCDRUNProc::ParsePpsNumber()
{
   T_ATTRI_VAL   areaCode;
   T_ATTRI_VAL   groupId;
   T_ATTRI_VAL   userType;
   T_ATTRI_VAL   std_number;
   //计费方检查
   if(!checkMsisdn(m_pcdr[ATTRI_MSISDN_A]))
       return false;

	 std_number = m_pcdr[ATTRI_MSISDN_B];

   //固网号码：区号+号码 取消了106并存期
   if(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "0", 1) == 0)
   {
   	   //默认对端电信
       m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]   = m_pcdr[ATTRI_MSISDN_B];
       m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SM_SERV_TYPE_FIX;
       m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B] = "0";
       m_pcdr[ATTRI_USER_ASP_B]          = COMPANY_TYPE_CT;
       m_pcdr[ATTRI_SM_TYPESTAT]         = "CTC";
       return true;
   }

   //查短信中心号码表
   if (m_globalManager->getSmServCode(std_number.c_str(),m_destProperty))
   {
       m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]   = m_pcdr[ATTRI_MSISDN_B];
       m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_destProperty.serv_type;
       m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B] = m_destProperty.serv_code_type;
       m_pcdr[ATTRI_USER_ASP_B]          = m_destProperty.asp;
       m_pcdr[ATTRI_SM_TYPESTAT]         = m_destProperty.sm_typestat;
       return true;
   }
   else
   {
       m_pcdr[ATTRI_ERROR_CODE] = SM_ERROR_PROT;
       return false;
   }

  return true;
}

bool SmCDRUNProc::ParseNumber()
{
   //对于回收的MT话单，需要将对方号码置发送方，在setMsisdn时再置为MT话单
   if(m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMO || m_pcdr[ATTRI_CALL_TYPE].length()==0)
    {
       //分析发送方
       setSmServType(m_pcdr[ATTRI_MSISDN_A],m_origProperty);
       //分析接收方
       setSmServType(m_pcdr[ATTRI_MSISDN_B],m_destProperty);
    }
    else
    {
       //分析发送方
       setSmServType(m_pcdr[ATTRI_MSISDN_B],m_origProperty);
       //分析接收方
       setSmServType(m_pcdr[ATTRI_MSISDN_A],m_destProperty);
    }

    //设定计费方
    if(!setMsisdn())
        return false;

  return true;
}

bool SmCDRUNProc::setSmServType(const T_ATTRI_VAL &r_msisdn, SmProperty &r_smProperty)
{
   T_ATTRI_VAL   areaCode;
   T_ATTRI_VAL   groupId;
   T_ATTRI_VAL   userType;
   T_ATTRI_VAL   std_number;

	 std_number = r_msisdn;
   //固网号码：区号+号码 取消了106并存期
   if(strncmp(std_number.c_str(), "0", 1) == 0
   	  && m_globalManager->getHomeAreaCodeByMsisdnN(std_number,
   	          m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],areaCode,groupId,userType))
   {
   	   //默认对端电信
   	   r_smProperty.home_type      = AREA_TYPE_LOCAL;
   	   r_smProperty.orig_number    = r_msisdn;
   	   r_smProperty.number         = r_msisdn;
   	   r_smProperty.serv_type      = SM_SERV_TYPE_FIX;
   	   r_smProperty.serv_code_type = "0";
   	   r_smProperty.asp            = COMPANY_TYPE_CT;
   	   r_smProperty.sm_typestat    = "CTC";
       return true;
   }

   //查短信中心号码表
   if(m_globalManager->getSmServCode(std_number.c_str(),r_smProperty))
   {
      r_smProperty.orig_number=r_msisdn;
      //集团短信端口需要规范，其他短信保留原始值
      if(r_smProperty.serv_type!=SM_SERV_TYPE_VPN &&
         r_smProperty.serv_type!=SM_SERV_TYPE_HY  &&
         r_smProperty.serv_type!=SM_SERV_TYPE_PROT)
      {
      	 r_smProperty.number = r_msisdn; //非 04 06 0A类型号码重置
      }
      return true;
   }
   else
   {
   	   r_smProperty.orig_number = r_msisdn;
       r_smProperty.number      = r_msisdn;
       m_pcdr[ATTRI_ERROR_CODE] = SM_ERROR_PROT;
       return false;
   }

  return true;
}


bool SmCDRUNProc::setMsisdn()
{

  //行业应用处理 SMO 将端口属性放到B的属性值中
  if(m_origProperty.asp==m_Config->m_operatorType &&
     (m_origProperty.serv_type==SM_SERV_TYPE_VPN ||
      m_origProperty.serv_type==SM_SERV_TYPE_0C ||
      m_origProperty.serv_type==SM_SERV_TYPE_0F ||
      m_origProperty.serv_type==SM_SERV_TYPE_HY) )
  {
      m_destProperty.serv_type      = m_origProperty.serv_type;
      m_destProperty.serv_code_type = m_origProperty.serv_code_type;
      m_destProperty.sm_typestat    = m_origProperty.sm_typestat;
      setSmCdrProperty(CALL_TYPE_SMO,m_origProperty,m_destProperty);
  }
  //行业应用处理  SMT
  else if(m_destProperty.asp==m_Config->m_operatorType &&
          (m_destProperty.serv_type==SM_SERV_TYPE_VPN ||
           m_destProperty.serv_type==SM_SERV_TYPE_0C ||
           m_destProperty.serv_type==SM_SERV_TYPE_0F ||
           m_destProperty.serv_type==SM_SERV_TYPE_HY) )
  {
      m_origProperty.serv_type      = m_destProperty.serv_type;
      m_origProperty.serv_code_type = m_destProperty.serv_code_type;
      m_origProperty.sm_typestat    = m_destProperty.sm_typestat;
      setSmCdrProperty(CALL_TYPE_SMT,m_destProperty,m_origProperty);
  }
  //SMO
  else if(m_origProperty.asp==m_Config->m_operatorType &&
          (m_origProperty.serv_type==SM_SERV_TYPE_GSM) &&
          (m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMO || m_pcdr[ATTRI_CALL_TYPE].length()==0))
  {
      setSmCdrProperty(CALL_TYPE_SMO,m_origProperty,m_destProperty);
  }
  //SMT
  else if (m_destProperty.asp==m_Config->m_operatorType &&
            (m_destProperty.serv_type==SM_SERV_TYPE_GSM) )
  {
      setSmCdrProperty(CALL_TYPE_SMT,m_destProperty,m_origProperty);
  }
  else
  {
      setSmCdrProperty(CALL_TYPE_SMO,m_origProperty,m_destProperty);
      return false;
  }

  return true;
}


bool SmCDRUNProc::setSpMsisdn()
{
  //按sp计费
  if(m_pcdr[ATTRI_USER_SERVICE_TYPE_B]==SM_SERV_TYPE_0D && m_pcdr[ATTRI_CHARGE_TYPE]==CHARGE_TYPE_01
	&& (m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMT|| m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMTF) )
  {
      m_pcdr[ATTRI_MSISDN_B]=m_pcdr[ATTRI_MSISDN_A];
      m_pcdr[ATTRI_MSISDN_A]=m_pcdr[ATTRI_SP_CODE];
	    m_pcdr[ATTRI_CALL_TYPE]=CALL_TYPE_SMO;
  }
  //按oper_code计费
  else if (m_pcdr[ATTRI_USER_SERVICE_TYPE_B]==SM_SERV_TYPE_0E &&
           m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMT)
  {
  	  
      m_pcdr[ATTRI_MSISDN_A]=m_pcdr[ATTRI_OPER_CODE];
      m_pcdr[ATTRI_CALL_TYPE]=CALL_TYPE_SMO;
  }

	return true;
}

bool SmCDRUNProc::checkFixPhone(const T_ATTRI_VAL &r_phone)
{
   T_ATTRI_VAL   areaCode;
   T_ATTRI_VAL   groupId;
   T_ATTRI_VAL   userType;
   T_ATTRI_VAL   std_number;
   int rd;

	 std_number = r_phone;
   if(strncmp(std_number.c_str(), "0", 1) == 0)
   {
      rd = m_globalManager->getHomeAreaCodeByMsisdnN(std_number,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
    	   	                                       areaCode,groupId,userType);
       if(rd >0 && m_globalManager->isProvInnerCode(areaCode))
       {
   	     //默认对端电信
         m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SM_SERV_TYPE_FIX;
         m_pcdr[ATTRI_SM_SERV_CODE_TYPE_A] = "0";
         m_pcdr[ATTRI_USER_ASP_A]          = COMPANY_TYPE_CT;
         return true;
       }
   }
   else if(std_number.length()>=9 && std_number[0] != '0')
   {
   	  std_number = std_number.append(0,"0");
      rd = m_globalManager->getHomeAreaCodeByMsisdnN(std_number,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
    	   	                                       areaCode,groupId,userType);
      if(rd >0 && m_globalManager->isProvInnerCode(areaCode))
      {
     		m_pcdr[ATTRI_MSISDN_A] = std_number;
   	   //默认对端电信
       m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = SM_SERV_TYPE_FIX;
       m_pcdr[ATTRI_SM_SERV_CODE_TYPE_A] = "0";
       m_pcdr[ATTRI_USER_ASP_A]          = COMPANY_TYPE_CT;
   	    return true;
      }
   }

   m_pcdr[ATTRI_ERROR_CODE] = SM_ERROR_MSISDN;
	 return false;
}

bool SmCDRUNProc::setSmCdrProperty(const T_CALL_TYPE &r_calltype,
        const SmProperty &r_msisdnProperty,
        const SmProperty &r_otherProperty)
{
    m_pcdr[ATTRI_CALL_TYPE]            = r_calltype;
    m_pcdr[ATTRI_MSISDN_A]             = r_msisdnProperty.number;
    m_pcdr[ATTRI_USER_SERVICE_TYPE_A]  = r_msisdnProperty.serv_type;
    m_pcdr[ATTRI_SM_SERV_CODE_TYPE_A]  = r_msisdnProperty.serv_code_type;
    m_pcdr[ATTRI_USER_ASP_A]           = r_msisdnProperty.asp;

    m_pcdr[ATTRI_MSISDN_B]             = r_otherProperty.orig_number;
   	m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]    = r_otherProperty.number;
    m_pcdr[ATTRI_USER_SERVICE_TYPE_B]  = r_otherProperty.serv_type;
    m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B]  = r_otherProperty.serv_code_type;
    m_pcdr[ATTRI_USER_ASP_B]           = r_otherProperty.asp;
    m_pcdr[ATTRI_SM_TYPESTAT]          = r_otherProperty.sm_typestat;

    if (m_destProperty.serv_type==SM_SERV_TYPE_UNKNOWN)
    {
        m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_PROT;
    }

  return true;
}

bool SmCDRUNProc::checkMsisdn(const T_ATTRI_VAL &r_msisdn)
{
   //按sp或oper_code计费不判计费号码，并且不是手机号码 2006-8-14 13:54
   if(!isSelfMobile(r_msisdn) &&
          (m_pcdr[ATTRI_USER_SERVICE_TYPE_B]==SM_SERV_TYPE_0D || m_pcdr[ATTRI_USER_SERVICE_TYPE_B]==SM_SERV_TYPE_0E))
    {
        m_pcdr[ATTRI_USER_SERVICE_TYPE_A] = m_pcdr[ATTRI_USER_SERVICE_TYPE_B];
        m_pcdr[ATTRI_USER_ASP_A]          = m_Config->m_operatorType;
        return true;
    }

   if(isMobile(r_msisdn))
   {
      if(r_msisdn.length() != 11)
       {
           m_pcdr[ATTRI_ERROR_CODE] = SM_ERROR_MSISDN;
           return false;
       }
   }
   else
   {
      m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_MSISDN;
      return false;
   }

  return true;
}

bool SmCDRUNProc::getSpServType()
{
  //求sp_code属性
  setSpServType();

  //按sp或oper_code计费
  setSpMsisdn();
  
  //计费方检查
  if(!checkMsisdn(m_pcdr[ATTRI_MSISDN_A]))
      return false;  

  return true;
}

bool SmCDRUNProc::setSpServType()
{
	 int t_iRet;
	 //江苏联通要求SP_NAME 特殊处理
	 t_iRet=ProcessBaseHelper::ExecuteProcedure("LocalizeSpSpecialProc");
	 if(t_iRet==1) return true;

  //查SP号码表 2007.12后旧服务代码不计费 根据新通知 旧代码拦截期 调整为2007年12月1日零点
  if (m_globalManager->getSpServCode(m_pcdr[ATTRI_CALL_TYPE],
  	       m_pcdr[ATTRI_SP_CODE],m_pcdr[ATTRI_OPER_CODE],m_pcdr[ATTRI_SERVICE_CODE],m_spProperty))
  {
     m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]    = m_spProperty.service_code;
     m_pcdr[ATTRI_USER_SERVICE_TYPE_B]  = m_spProperty.serv_type;
     m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B]  = m_spProperty.serv_code_type;
     m_pcdr[ATTRI_USER_ASP_B]           = m_spProperty.asp;
     m_pcdr[ATTRI_SM_TYPESTAT]          = m_spProperty.sm_typestat;
     m_pcdr[ATTRI_USER_HOME_TYPE_B]     = m_spProperty.home_type;
     m_pcdr[ATTRI_SP_SERV_AREA]         = m_spProperty.sp_serv_area;
     m_pcdr[ATTRI_SP_PROV_CODE]         = m_spProperty.prov_code;
  }
  else
  {
      m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_SPCODE;
      return false; 
  }
  return true;
}

bool SmCDRUNProc::getUssdServType()
{
  return true;
}

bool SmCDRUNProc::getHyOperCode()
{
  return true;
}

bool SmCDRUNProc::getPpsSmServType()
{
  if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_PPS1 &&
  	(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMO || m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMOF))
  {
  	if(isMobile(m_pcdr[ATTRI_MSISDN_B]) ||
  		 strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "0", 1) == 0 ||
  	   strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "1060", 4) == 0)
  	{
  		//发手机发小灵通处理逻辑
      ParsePpsNumber();
  	}
  	else
  	{
  		//SP上行
  		getSpServType();
  	}
  }
  else if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_PPS1 &&
  	(m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMT || m_pcdr[ATTRI_CALL_TYPE] == CALL_TYPE_SMTF))
  {
  		//SP下行
  		getSpServType();
  }
  	 
  return true;
}


bool SmCDRUNProc::getFixSmServType()
{
   T_ATTRI_VAL   areaCode;
   T_ATTRI_VAL   groupId;
   T_ATTRI_VAL   userType;
   T_ATTRI_VAL   std_number;
  //固网计费方检查
  if(!checkFixPhone(m_pcdr[ATTRI_MSISDN_A]))
      return false;  

   //固网对端允许出现手机号码前带0，截取0  add by shenglj 2009-04-17 11:19:01
   if ((strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "0", 1) == 0) && (isMobile(m_pcdr[ATTRI_MSISDN_B].c_str() + 1)))
   {
      m_pcdr[ATTRI_MSISDN_B] = m_pcdr[ATTRI_MSISDN_B].c_str() + 1;
   }

	 std_number = m_pcdr[ATTRI_MSISDN_B];

   //固网号码：区号+号码 取消了106并存期
   if(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "0", 1) == 0 &&
   	  m_globalManager->getHomeAreaCodeByMsisdnN(m_pcdr[ATTRI_MSISDN_B],m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],areaCode,groupId,userType))
   {
   	   //默认对端电信
       m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]   = m_pcdr[ATTRI_MSISDN_B];
       m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SM_SERV_TYPE_FIX;
       m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B] = "0";
       m_pcdr[ATTRI_USER_ASP_B]          = COMPANY_TYPE_CNC;
       m_pcdr[ATTRI_SM_TYPESTAT]         = "CNC";
       return true;
   }
   //固网没带区号 算本地通话
   else if(strncmp(m_pcdr[ATTRI_MSISDN_B].c_str(), "0", 1) != 0 &&
           m_pcdr[ATTRI_MSISDN_B].length()>=7 && m_pcdr[ATTRI_MSISDN_B].length()<=8)
   {
   	   //默认对端电信
       m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]   = m_pcdr[ATTRI_MSISDN_B];
       m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = SM_SERV_TYPE_FIX;
       m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B] = "0";
       m_pcdr[ATTRI_USER_ASP_B]          = COMPANY_TYPE_CNC;
       m_pcdr[ATTRI_SM_TYPESTAT]         = "CNC";
       return true;
   }

   //查短信中心号码表
   if (m_globalManager->getSmServCode(std_number.c_str(),m_destProperty))
   {
       m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]   = m_pcdr[ATTRI_MSISDN_B];
       m_pcdr[ATTRI_USER_SERVICE_TYPE_B] = m_destProperty.serv_type;
       m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B] = m_destProperty.serv_code_type;
       m_pcdr[ATTRI_USER_ASP_B]          = m_destProperty.asp;
       m_pcdr[ATTRI_SM_TYPESTAT]         = m_destProperty.sm_typestat;
       return true;
   }
   else
   {
       //本地化固网短信对端号码是否判错
	     if(ProcessBaseHelper::ExecuteProcedure("LocalizeFixSmCdrJudge")==1)
	     {
		     return true;
	     }
       m_pcdr[ATTRI_ERROR_CODE] = SM_ERROR_PROT;
       return false;
   }

   return true;
}

bool SmCDRUNProc::getFixSpServType()
{
   T_ATTRI_VAL   areaCode;
   T_ATTRI_VAL   groupId;
   T_ATTRI_VAL   userType;
   T_ATTRI_VAL   std_number;
  //固网计费方检查
  if(!checkFixPhone(m_pcdr[ATTRI_MSISDN_A]))
      return false;  

  m_pcdr[ATTRI_MSISDN_B_FOR_ROAM]    = m_pcdr[ATTRI_MSISDN_B];
  m_pcdr[ATTRI_USER_SERVICE_TYPE_B]  = SM_SERV_TYPE_SP;
  m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B]  = "0";
  m_pcdr[ATTRI_USER_ASP_B]           = m_Config->m_operatorType;
  m_pcdr[ATTRI_SM_TYPESTAT]          = "CNC";
  m_pcdr[ATTRI_USER_HOME_TYPE_B]     = AREA_TYPE_LOCAL;
  m_pcdr[ATTRI_SP_SERV_AREA]         = "0";
  m_pcdr[ATTRI_SP_PROV_CODE]         = m_Config->m_provinceCode;

   return true;
}

//先求属性后求通道
int SmCDRUNProc::execute()
{
 #ifdef FP_DEBUG
   FP_BEGIN(SmCDRUNProc)
 #endif

  //特殊短信处理，如果符合规则直接返回，不按短信处理规则继续
	if(ProcessBaseHelper::ExecuteProcedure("LocalizeSmCdrGet")==1)
	{
		return 0;
	}

  if(!getSmsMethod())
  {
     return 0;
  }
  m_FilterInfo->m_smsMethod = m_smsMethod;

  InitCdrProperty();

  //求分通道必要属性
  getDisChannelProperty();

 #ifdef FP_DEBUG
   FP_END(SmCDRUNProc)
 #endif
  return 0;
}
