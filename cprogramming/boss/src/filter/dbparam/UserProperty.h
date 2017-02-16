#ifndef _USERPROPERTY_H_200800815_
#define _USERPROPERTY_H_200800815_

#include "FieldsXX.h"

const string BIND_TIME_SECTION ("5"); //绑资料时间分段标志 >=5 先严格按精确时间匹配资料,找不到再按设定方式找

//用户资料全量表
class UserServInfo
{
public:
	bool isValid(const T_DATE_TIME &beginTime)
	{
		if ((beginTime < m_beginTime) || (beginTime > m_endTime))
			return false;
		else
			return true;
	}
	bool isValid(const T_DATE_TIME &beginTime,const T_DATE_TIME &endTime)
    {
		if ((beginTime < m_beginTime) || (beginTime > m_endTime))
			return false;
		else
			return true;
    }
    bool isValid(const T_DATE_TIME &beginTime,const T_DATE_TIME &endTime,T_FLAG &flag)
    {
		if(flag == "1" || flag == "5") //1:不判时间有效范围
		{
			return true;
		}
		else if(flag == "2" || flag == "6") //2:判断时间有效范围精确到当月
		{
			if ((beginTime.substr(0,6) < m_beginTime.substr(0,6)) || (beginTime.substr(0,6) > m_endTime.substr(0,6)))
				return false;
			else
				return true;
		}
		else if(flag == "3" || flag == "7") //3:判断时间有效范围到上个月（话单上个月月份 >= 资料结束月份）
		{
			long r_lastmonth,r_crm_end_month;
			r_lastmonth = beginTime.substr(0,6).value();   //话单本月月份
			r_crm_end_month = m_endTime.substr(0,6).value(); //资料结束月份
			addMonth(-1,r_lastmonth);                        //话单上月月份
			
			if (r_lastmonth > r_crm_end_month)
				return false;
			else
				return true;
		}
		else //0:判断时间有效范围精确到秒，正常情况
		{
			if ((beginTime < m_beginTime) || (beginTime > m_endTime))
				return false;
			else
				return true;
		}
    }
	//add by dph, 增加客户资料时间前后偏移判断
	bool isValid(const T_DATE_TIME &beginTime,const T_DATE_TIME &endTime,T_FLAG &flag,int pre_offset,int post_offset)
    {
		if(flag == "1" || flag == "5") //1:不判时间有效范围
		{
			return true;
		}
		else if(flag == "2" || flag == "6") //2:判断时间有效范围精确到当月
		{
			if ((beginTime.substr(0,6) < m_beginTime.substr(0,6)) || (beginTime.substr(0,6) > m_endTime.substr(0,6)))
				return false;
			else
				return true;
		}
		else if(flag == "3" || flag == "7") //3:判断时间有效范围到上个月（话单上个月月份 >= 资料结束月份）
		{
			long r_lastmonth,r_crm_end_month;
			r_lastmonth = beginTime.substr(0,6).value();   //话单本月月份
			r_crm_end_month = m_endTime.substr(0,6).value(); //资料结束月份
			addMonth(-1,r_lastmonth);                        //话单上月月份
			
			if (r_lastmonth > r_crm_end_month)
				return false;
			else
				return true;
		}
		//add by dph,增加客户资料时间偏移，可通过参数设置前偏和后偏
		else if(flag == "4" || flag == "8")
		{
			long pre_month, post_month;
			pre_month = m_beginTime.substr(0,6).value();
			post_month = m_endTime.substr(0,6).value();
			addMonth(pre_offset, pre_month);
			addMonth(post_offset, post_month);
			if((beginTime.substr(0,6).value() < pre_month) || (beginTime.substr(0,6).value() > post_month))
				return false;
			else
				return true;
		}
		else //0:判断时间有效范围精确到秒，正常情况
		{
			if ((beginTime < m_beginTime) || (beginTime > m_endTime))
				return false;
			else
				return true;
		}
    }
	//end of add by dph, 增加客户资料时间前后偏移判断
	
	void reset()
	{
		m_eparchy_code = "";
		m_office_code  = "";
		m_userId       = 0;
		m_custId       = 0;
		m_productId    = 0;
		m_channelNo    = 0;
		m_msisdn       = "";
		m_imsi         = "";
		m_brand_code   = "";
		m_beginTime    = "";
		m_endTime      = "";
		m_inTime       = "";
		m_openTime     = "";
		m_doubleMode   = "0";
		m_acctTag      = "9"; //置未知标识
		m_net_type_code= ""; 
		m_prepayTag    = "";
		m_userTypeCode = "";
	}
	
    T_EPARCHY_CODE m_eparchy_code; //地市编码 用于分省内通道
    T_OFFICE_CODE  m_office_code;  //市县编码 用户资料中获取
	long           m_userId;       //用户标识
	long           m_custId;       //客户标识
	int            m_productId;    //产品标识
    int            m_channelNo;    //省内通道号 需要求取
    T_MSISDN		 	 m_msisdn;       //服务号码
    T_IMSI				 m_imsi;         //IMSI
    T_BRAND_CODE   m_brand_code;   //品牌编码
	T_DATE_TIME		 m_beginTime;    //生效时间
	T_DATE_TIME		 m_endTime;      //结束时间
	T_DATE_TIME		 m_inTime;       //建档时间
    T_DATE_TIME		 m_openTime;     //开户时间
    T_FLAG         m_doubleMode;   //双模标志
    T_FLAG         m_acctTag;      //出帐标志：0-正常处理，1-定时激活，2-待激活用户，Z-不出帐(2 标识预开卡用户)
    T_BRAND_CODE   m_net_type_code;//网别编码
    T_FLAG         m_prepayTag;    //预付费标志：0-后付费，1-预付费
    T_SERVICE_CODE m_userTypeCode;  //用户类型 0-普通用户
	
    void operator=(const UserServInfo& right)
	{
		m_eparchy_code = right.m_eparchy_code;
		m_office_code  = right.m_office_code;
		m_userId       = right.m_userId;
		m_custId       = right.m_custId;
		m_productId    = right.m_productId;
		m_channelNo    = right.m_channelNo;
		m_msisdn       = right.m_msisdn;
		m_imsi         = right.m_imsi;
		m_brand_code   = right.m_brand_code;
		m_beginTime    = right.m_beginTime;
		m_endTime      = right.m_endTime;
		m_inTime       = right.m_inTime;
		m_openTime     = right.m_openTime;
		m_doubleMode   = right.m_doubleMode;
		m_acctTag      = right.m_acctTag;
		m_net_type_code= right.m_net_type_code;
		m_prepayTag    = right.m_prepayTag;
		m_userTypeCode = right.m_userTypeCode;
	}
	friend bool operator == (const UserServInfo &left,const UserServInfo& right)
	{
		return (left.m_msisdn == right.m_msisdn);
	}
    friend bool operator < (const UserServInfo &left,const UserServInfo& right)
	{
		if (left.m_msisdn < right.m_msisdn)
		{
			return true;
		}
		else if (left.m_msisdn > right.m_msisdn)
		{
			return false;
		}
		if (left.m_beginTime < right.m_beginTime)
		{
			return true;
		}
		else if (left.m_beginTime > right.m_beginTime)
		{
			return false;
		}
		if (left.m_endTime < right.m_endTime)
		{
			return true;
		}
		return false;
	}
    friend ostream& operator<<(ostream& os,const UserServInfo& right)
	{
		os << right.m_eparchy_code<< " , "
			<< right.m_office_code << " , "
			<< right.m_userId      << " , "
			<< right.m_custId      << " , "
			<< right.m_productId   << " , "
			<< right.m_channelNo   << " , "
			<< right.m_msisdn      << " , "
			<< right.m_brand_code  << " , "
			<< right.m_imsi        << " , "
			<< right.m_doubleMode  << " , "
			<< right.m_acctTag     << " , "
			<< right.m_net_type_code << " , "
			<< right.m_beginTime   << " , "
			<< right.m_endTime     << " , "
			<< right.m_inTime      << " , "
			<< right.m_openTime;
		return os;
	}
	
  private:
	  void addMonth(const long &r_addMonth,long &r_month) const
	  {
		  struct tm t_tm;
		  char t_tmp[6+1];
		  t_tm.tm_year=r_month/100-1900;
		  t_tm.tm_mon =r_month%100-1+r_addMonth;
		  t_tm.tm_mday=1;
		  t_tm.tm_hour=0;
		  t_tm.tm_min=0;
		  t_tm.tm_sec=0;
		  t_tm.tm_isdst = -1;
		  
		  mktime(&t_tm);
		  sprintf(t_tmp,"%04d%02d",t_tm.tm_year+1900,t_tm.tm_mon+1);
		  r_month = atol(t_tmp);
	  }
};

class SpecialPhoneInfo
{
public:
	void reset()
	{
		m_matchType    = "";
		m_preFixType   = "";
		m_condition    = 0;
		m_msisdn       = "";
		m_areacode     = "";
		m_priority     = "";
		m_servType     = "";
		m_asp          = "";
		m_homeType     = "";
		m_groupId      = 0;
		m_length       = 0;
		m_calledType   = "";
		m_isNoNeedTp   = "";
		m_telitem      = "";
		m_beginTime    = "";
		m_endTime      = "";
		
	}
	
	T_FLAG        m_matchType;
	T_PREFIX_TYPE m_preFixType;
	int           m_condition;
	T_MSISDN      m_msisdn;
	T_AREA_CODE   m_areacode;
	T_PRI         m_priority;
	T_SERV_TYPE   m_servType;
	T_ASP         m_asp;
	T_HOME_TYPE   m_homeType;
	int           m_groupId;
	int           m_length;
	T_CALLED_TYPE m_calledType;
	T_FLAG        m_isNoNeedTp;
	T_ATTRI_VAL   m_telitem;    //山东联通特殊标识
	T_DATE_TIME   m_beginTime;
	T_DATE_TIME   m_endTime;
	
    void operator=(const SpecialPhoneInfo& right)
	{
		m_matchType    = right.m_matchType;
		m_preFixType   = right.m_preFixType;
		m_condition    = right.m_condition;
		m_msisdn       = right.m_msisdn;
		m_areacode     = right.m_areacode;
		m_priority     = right.m_priority;
		m_servType     = right.m_servType;
		m_asp          = right.m_asp;
		m_homeType     = right.m_homeType;
		m_groupId      = right.m_groupId;
		m_length       = right.m_length;
		m_calledType   = right.m_calledType;
		m_isNoNeedTp   = right.m_isNoNeedTp;
		m_telitem      = right.m_telitem;
		m_beginTime    = right.m_beginTime;
		m_endTime      = right.m_endTime;
	}
	friend bool operator == (const SpecialPhoneInfo &left,const SpecialPhoneInfo& right)
	{
		return (left.m_msisdn == right.m_msisdn);
	}
    friend bool operator < (const SpecialPhoneInfo &left,const SpecialPhoneInfo& right)
	{
		return (left.m_msisdn < right.m_msisdn);
	}
    friend ostream& operator<<(ostream& os,const SpecialPhoneInfo& right)
	{
		os << right.m_matchType    << " , "
			<< right.m_preFixType   << " , "
			<< right.m_condition    << " , "
			<< right.m_msisdn       << " , "
			<< right.m_areacode     << " , "
			<< right.m_priority     << " , "
			<< right.m_servType     << " , "
			<< right.m_asp          << " , "
			<< right.m_homeType     << " , "
			<< right.m_groupId      << " , "
			<< right.m_length       << " , "
			<< right.m_calledType   << " , "
			<< right.m_isNoNeedTp   << " , "
			<< right.m_telitem      << " , "
			<< right.m_beginTime    << " , "
			<< right.m_endTime;
		return os;
	}
};

#endif //_USERPROPERTY_H_200800815_
