#include "FilterCdr.h"
#include "FilterError.h"
#include "ConstDef.h"

#include "ProcessBase.h"
#include "base/TimeUtil.h"
#include "base/StringUtil.h"

FilterCdr::FilterCdr()
{
}
FilterCdr::~FilterCdr()
{
}

//读取话单前一些属性初始化
bool FilterCdr::reset() 
{
  bool t_bRet = true;
  if(CdrBase::reset()==false)
  {
    t_bRet = false;
  }

	int i,j;
	T_DATE beginDate,endDate;
	
	//1.属性初始化
  m_attrList[ATTRI_ROAM_TYPE]      = ROAM_TYPE_NONE;
  m_attrList[ATTRI_LONG_TYPE1]     = LONG_TYPE_NONE;
  m_attrList[ATTRI_LONG_TYPE2]     = LONG_TYPE_NONE;
  m_attrList[ATTRI_LONG_GROUP1]    = DEFAULT_GROUP_ID_B;
  m_attrList[ATTRI_LONG_GROUP2]    = DEFAULT_GROUP_ID_B;
  //m_attrList[ATTRI_LONG_GROUP1]    = DEFAULT_GROUP_ID_B;
  //m_attrList[ATTRI_LONG_GROUP2]    = DEFAULT_GROUP_ID_B;
  m_attrList[ATTRI_CARRIER_TYPE]   = CARRIER_TYPE_IP_DEFAULT;
  m_attrList[ATTRI_TRAN_TYPE_B]    = "0";
  m_attrList[ATTRI_PA_TYPE]        = "2"; //无区间概念

  m_attrList[ATTRI_SPECIAL_PHONE_GROUP] = DEFAULT_SPECIAL_PHONE_GROUP;

//如果没有话单合并标志，将其置为正常"0"
/*
  if (m_attrList[ATTRI_RESULT].length() == 0)
  {
  	m_attrList[ATTRI_RESULT] = "0";
  }
*/

  //初始化边界漫游标志
  m_attrList[ATTRI_EDGE_FLAG] = EDGE_ROAM_NO;
	
	//特殊号码标识
  m_attrList[ATTRI_SPE_PHONE_FLAG] = ATTRI_SPECIALPHONE_N;
    
  m_attrList[ATTRI_CALLED_TYPE_B]  = "00";
  if(m_attrList[ATTRI_ERROR_CODE].length() == 0)
  {
  	m_attrList[ATTRI_ERROR_CODE] = DEFAULT_ERROR;
  }

  //分拣共同属性初始化
  m_attrList[ATTRI_ERROR_FLAG]   = ATTRI_ERROR_N;
	m_attrList[ATTRI_TRASH_FLAG]   = ATTRI_TRASH_N;
	m_attrList[ATTRI_CDR_CHANNEL]  = "";
	m_attrList[ATTRI_MUX_CHANNEL]  = "";
	m_attrList[ATTRI_BACK_CHANNEL] = "";

	//2.本地化的话单初始化过程
	ProcessBaseHelper::ExecuteProcedure("CdrAttribInit");

  return t_bRet;
}
//读取话单后规整
bool FilterCdr::canonicalize()
{
  bool t_bRet = true;
  if(CdrBase::canonicalize()==false)
  {
    t_bRet = false;
  }

	string gprsApnNi;
	if (m_attrList[ATTRI_BIZ_TYPE] == BIZ_TYPE_GPRS)
	{
		//GPRS话单的APNNI地址转换成大写，输出用原始的输出
		gprsApnNi = m_attrList[ATTRI_GPRS_APNNI];
		m_attrList[ATTRI_GPRS_APNNI] = StringUtil::toUpperCase(gprsApnNi);
	}
	
	string smsOperCode;
	if (m_attrList[ATTRI_BIZ_TYPE] == BIZ_TYPE_SMS)
	{
		//短信话单的OPER_CODE地址转换成大写
		smsOperCode = m_attrList[ATTRI_OPER_CODE];
		//modify by shenglj 2009-05-23 22:56:01
		//m_attrList[ATTRI_OPER_CODE] = StringUtil::toUpperCase(smsOperCode);
	}

  //开始日期、开始时间备份 add by shenglj 2009-08-15 11:15:01
  m_beginDate = m_attrList[ATTRI_BEGIN_DATE];
  m_beginTime = m_attrList[ATTRI_BEGIN_TIME];

  return true;
}

//输出话单前规整
bool FilterCdr::finalize()
{
  bool t_bRet = true;
  if(CdrBase::finalize()==false)
  {
    t_bRet = false;
  }

  //开始日期、开始时间恢复 add by shenglj 2009-08-15 11:15:01
  m_attrList[ATTRI_BEGIN_DATE] = m_beginDate;
  m_attrList[ATTRI_BEGIN_TIME] = m_beginTime;

  //省内号码号码规整 去接入号
  if(m_attrList[ATTRI_BIZ_TYPE] == BIZ_TYPE_GSM)
  {
  	if(strncasecmp(m_attrList[ATTRI_CDR_CHANNEL].c_str(),CHANNEL_NR.c_str(),CHANNEL_NR.length())==0 ||
  		 strncasecmp(m_attrList[ATTRI_CDR_CHANNEL].c_str(),CHANNEL_IR.c_str(),CHANNEL_IR.length())==0 ||
  		 strncasecmp(m_attrList[ATTRI_CDR_CHANNEL].c_str(),CHANNEL_BR.c_str(),CHANNEL_BR.length())==0)
  	{
      return true;
  	}
  	m_attrList[ATTRI_MSISDN_B_FOR_ROAM] = m_attrList[ATTRI_MSISDN_B];
  }

  return true;
}

