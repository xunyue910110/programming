#include "SmCDRProc.h"

#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

#include "FilterConfig.h"
#include "PMGlobalManager.h"

INSTALL_PROCESSBASE_INSTANCE(SmCDRProc);

bool SmCDRProc::InitCdrProperty()
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

bool SmCDRProc::getTrash()
{
   if (m_pcdr[ATTRI_CALL_TYPE][0]=='F' ) //互通网关smt话单，无效
   {
   	   m_pcdr[ATTRI_TRASH_FLAG]=ATTRI_TRASH_Y;
       return true;
   }
   return false;
}


bool SmCDRProc::getSmsMethod()
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

bool SmCDRProc::getDisChannelProperty()
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
		default:
		    #ifdef _DEBUG_
		      cout<<"not define sms deal method: "<<m_smsMethod<<""<<__FILE__<<__LINE__<<endl;
		    #endif
			break;
	}

  return true;
}

bool SmCDRProc::getSmServType()
{
	//分析号码
	if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_SMS_ISMG)
	{
	   ParseLinkage();
	}
	else
	{
		 ParseNumber();
	}

  return true;
}

bool SmCDRProc::ParseLinkage()
{
   T_ATTRI_VAL   areaCode;
   T_ATTRI_VAL   groupId;
   T_ATTRI_VAL   userType;
   T_ATTRI_VAL   std_number;
   //计费方检查
   if(!checkMsisdn(m_pcdr[ATTRI_MSISDN_A]))
       return false;

	 std_number = m_pcdr[ATTRI_SERVICE_CODE];

   //固网号码：区号+号码 取消了106并存期
   if(std_number[0]=='0' && m_globalManager->getHomeAreaCodeByMsisdnN(std_number,
   	                        m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],areaCode,groupId,userType))
   {
      if(m_pcdr[ATTRI_SM_USER_TYPE_B][0]=='2')
      {
          std_number = "10602"; //电信号码
      }
      else
      {
          std_number = "10603"; //网通号码
      }
   }

   //查短信中心号码表
   if (m_globalManager->getSmServCode(std_number.c_str(),m_destProperty))
   {
       m_pcdr[ATTRI_MSISDN_B]            = m_pcdr[ATTRI_SERVICE_CODE];
       m_pcdr[ATTRI_SM_SERV_TYPE_B]      = m_destProperty.serv_type;
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

bool SmCDRProc::ParseNumber()
{
   //对于回收的MT话单，需要将对方号码置发送方，在setMsisdn时再置为MT话单
   if(m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMO || m_pcdr[ATTRI_CALL_TYPE].length()==0)
    {
       //分析发送方
       setSmServType(m_pcdr[ATTRI_MSISDN_A],m_origProperty);
       //分析接收方
       setSmServType(m_pcdr[ATTRI_SERVICE_CODE],m_destProperty);
    }
    else
    {
       //分析发送方
       setSmServType(m_pcdr[ATTRI_SERVICE_CODE],m_origProperty);
       //分析接收方
       setSmServType(m_pcdr[ATTRI_MSISDN_A],m_destProperty);
    }

    if(m_pcdr[ATTRI_SOURCE_TYPE]==SOURCE_TYPE_SMS_HYYY)
    {
       m_destProperty.orig_number=m_pcdr[ATTRI_MSISDN_B]; //2A类型B原始号码从话单中获取
    }

    //设定计费方
    if(!setMsisdn())
        return false;

  return true;
}

bool SmCDRProc::setSmServType(const T_ATTRI_VAL &r_msisdn, SmProperty &r_smProperty)
{
   T_ATTRI_VAL   areaCode;
   T_ATTRI_VAL   groupId;
   T_ATTRI_VAL   userType;
   T_ATTRI_VAL   std_number;

	 std_number = r_msisdn;
   //固网号码：区号+号码 取消了106并存期
   if(std_number[0]=='0' && m_globalManager->getHomeAreaCodeByMsisdnN(std_number,
   	                        m_pcdr[ATTRI_BEGIN_DATE]+m_pcdr[ATTRI_BEGIN_TIME],areaCode,groupId,userType))
   {
      if(m_pcdr[ATTRI_SM_USER_TYPE_B][0]=='2')
      {
          std_number = "10602"; //电信号码
      }
      else
      {
          std_number = "10603"; //网通号码
      }
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
       r_smProperty.number      = r_msisdn;
       m_pcdr[ATTRI_ERROR_CODE] = SM_ERROR_PROT;
       return false;
   }

  return true;
}


bool SmCDRProc::setMsisdn()
{

  //行业应用处理 SMO 将端口属性放到B的属性值中
  if(m_origProperty.asp==COMPANY_TYPE_CMCC &&
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
  else if(m_destProperty.asp==COMPANY_TYPE_CMCC &&
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
  else if(m_origProperty.asp==COMPANY_TYPE_CMCC &&
          (m_origProperty.serv_type==SM_SERV_TYPE_GSM ||
           m_origProperty.serv_type==SM_SERV_TYPE_TD))
  {
      setSmCdrProperty(CALL_TYPE_SMO,m_origProperty,m_destProperty);
  }
  //SMT
  else if (m_destProperty.asp==COMPANY_TYPE_CMCC &&
            (m_destProperty.serv_type==SM_SERV_TYPE_GSM ||
             m_destProperty.serv_type==SM_SERV_TYPE_TD) )
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


bool SmCDRProc::setSpMsisdn()
{
  //按sp计费
  if(m_pcdr[ATTRI_SM_SERV_TYPE_B]==SM_SERV_TYPE_0D && m_pcdr[ATTRI_CHARGE_TYPE]==CHARGE_TYPE_01
	&& (m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMT|| m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMTF) )
  {
      m_pcdr[ATTRI_MSISDN_B]=m_pcdr[ATTRI_MSISDN_A];
      m_pcdr[ATTRI_MSISDN_A]=m_pcdr[ATTRI_SP_CODE];
	    m_pcdr[ATTRI_CALL_TYPE]=CALL_TYPE_SMO;
  }
  //按oper_code计费
  else if (m_pcdr[ATTRI_SM_SERV_TYPE_B]==SM_SERV_TYPE_0E &&
           m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMT)
  {
  	  
      m_pcdr[ATTRI_MSISDN_A]=m_pcdr[ATTRI_OPER_CODE];
      m_pcdr[ATTRI_CALL_TYPE]=CALL_TYPE_SMO;
  }

	return true;
}

bool SmCDRProc::setSmCdrProperty(const T_CALL_TYPE &r_calltype,
        const SmProperty &r_msisdnProperty,
        const SmProperty &r_otherProperty)
{
  return true;
}

bool SmCDRProc::checkMsisdn(const T_ATTRI_VAL &r_msisdn)
{
   //按sp或oper_code计费不判计费号码，并且不是手机号码 2006-8-14 13:54
   if(!isSelfMobile(r_msisdn) &&
          (m_pcdr[ATTRI_SM_SERV_TYPE_B]==SM_SERV_TYPE_0D || m_pcdr[ATTRI_SM_SERV_TYPE_B]==SM_SERV_TYPE_0E))
    {
        m_pcdr[ATTRI_SM_SERV_TYPE_A] = m_pcdr[ATTRI_SM_SERV_TYPE_B];
        m_pcdr[ATTRI_USER_ASP_A]     = COMPANY_TYPE_CMCC;
        //m_pcdr[ATTRI_USER_ASP_A]     = m_Config->m_operatorType;
        return true;
    }

   if(!isSelfMobile(r_msisdn))
   {
      if(r_msisdn.length() != 11)
       {
           m_pcdr[ATTRI_ERROR_CODE] = SM_ERROR_MSISDN;
           return false;
       }
       m_pcdr[ATTRI_SM_SERV_TYPE_A] = SM_SERV_TYPE_GSM;
       m_pcdr[ATTRI_USER_ASP_A]     = COMPANY_TYPE_CMCC;
   }
   else
   {
       m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_MSISDN;
       return false;
   }

  return true;
}


//////////////////////////////////////////////////////////////////////////


bool SmCDRProc::getSpServType()
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

bool SmCDRProc::setSpServType()
{
  //查SP号码表 2007.12后旧服务代码不计费 根据新通知 旧代码拦截期 调整为2007年12月1日零点
  if (m_globalManager->getSpServCode(m_pcdr[ATTRI_SP_CODE].c_str(),m_pcdr[ATTRI_OPER_CODE].c_str(),m_spProperty))
  {
     if(m_pcdr[ATTRI_SERVICE_CODE].compare(0,m_spProperty.service_code.length(),m_spProperty.service_code)!=0)
     {
        m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_SERVICE_CODE; //服务代码无法匹配
        return false;
     }
     /*
	   //对2007.11后使用旧服务代码的sp话单不计费的置错单
	   //1:普通SP 2:自有业务 3:终端内置SP业务
	   if(m_spProperty.sp_type == '1' )
     {
        m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_SERVICE_CODE;
        return false;
     }
	   else if(sp_type == '3' &&
	   	      (m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMT ||
             m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMTF ||
             m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMC  ||
             m_pcdr[ATTRI_CALL_TYPE]==CALL_TYPE_SMCF)) //下行信息费不计费
     {
        m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_SERVICE_CODE;
        return false;
     }
     */
     m_pcdr[ATTRI_SM_SERV_TYPE_B]       =m_spProperty.serv_type;
     m_pcdr[ATTRI_SM_SERV_CODE_TYPE_B]  =m_spProperty.serv_code_type;
     m_pcdr[ATTRI_USER_ASP_B]           =m_spProperty.asp;
     m_pcdr[ATTRI_SM_TYPESTAT]          =m_spProperty.sm_typestat;
     m_pcdr[ATTRI_HOME_CODE_B]          =m_spProperty.home_type;
     m_pcdr[ATTRI_SP_SERV_AREA]         =m_spProperty.sp_serv_area;
     m_pcdr[ATTRI_SP_PROV_CODE]         =m_spProperty.prov_code;
  } 
  else
  {
      m_pcdr[ATTRI_ERROR_CODE]=SM_ERROR_SPCODE;
      return false; 
  }
  return true;
}


//////////////////////////////////////////////////////////////////////////

bool SmCDRProc::getUssdServType()
{
  return true;
}

//////////////////////////////////////////////////////////////////////////

bool SmCDRProc::getHyOperCode()
{
  return true;
}

//先求属性后求通道
int SmCDRProc::execute()
{
  if(!getSmsMethod())
  {
     return 0;
  }
  m_FilterInfo->m_smsMethod = m_smsMethod;

  InitCdrProperty();

  //求分通道必要属性
  getDisChannelProperty();


  return 0;
}
